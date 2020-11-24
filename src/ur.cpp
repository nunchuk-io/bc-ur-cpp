// Copyright (c) 2020 Enigmo
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <bc32.h>
#include <picosha2.h>
#include <ur.h>
#include <utils.h>

#include <algorithm>
#include <sstream>

namespace nunchuk {
namespace bcr {

std::vector<uint8_t> ComposeHeader(size_t length) {
  if (length > 0 && length <= 23) {
    return {(uint8_t)(0x40 + length)};
  } else if (length >= 24 && length <= 255) {
    return {0x58, (uint8_t)length};
  } else if (length >= 256 && length <= 65535) {
    return {0x59, (uint8_t)(length >> 8), (uint8_t)length};
  } else if (length >= 65536 && length <= 4294967295) {
    return {0x60, (uint8_t)(length >> 24), (uint8_t)(length >> 16),
            (uint8_t)(length >> 8), (uint8_t)length};
  }
  throw std::runtime_error("length exceeded");
}

std::vector<uint8_t> EncodeSimpleCBOR(const std::vector<uint8_t> &bufferData) {
  if (bufferData.empty() || bufferData.size() >= 4294967296) {
    throw std::runtime_error("data is too large");
  }
  auto header = ComposeHeader(bufferData.size());
  header.insert(header.end(), bufferData.begin(), bufferData.end());
  return header;
}

std::vector<uint8_t> DecodeSimpleCBOR(const std::vector<uint8_t> &data) {
  if (data.empty()) {
    throw std::runtime_error("invalid input");
  }
  uint8_t header = data[0];
  if (header < 0x58) {
    return {data.begin() + 1, data.end()};
  } else if (header == 0x58) {
    return {data.begin() + 2, data.end()};
  } else if (header == 0x59) {
    return {data.begin() + 3, data.end()};
  } else if (header == 0x60) {
    return {data.begin() + 5, data.end()};
  }
  throw std::runtime_error("invalid input");
}

std::vector<std::string>
ComposeHeadersToFragments(const std::vector<std::string> &fragments,
                          const std::string &digest,
                          const std::string &type = "bytes") {
  std::vector<std::string> rs;
  if (fragments.size() == 1) {
    std::stringstream urData;
    urData << "ur:" << type << "/" << fragments[0];
    rs.push_back(urData.str());
  } else {
    size_t total = fragments.size();
    for (size_t i = 0; i < total; i++) {
      std::stringstream urData;
      urData << "ur:" << type << "/" << (i + 1) << "of" << total << "/"
             << digest << "/" << fragments[i];
      rs.push_back(urData.str());
    }
  }
  return rs;
}

std::vector<std::string>
EncodeUniformResource(const std::vector<uint8_t> &payload, int capacity) {
  std::vector<uint8_t> cborPayload = EncodeSimpleCBOR(payload);
  std::string bc32Payload = EncodeBc32Data(cborPayload);
  std::vector<uint8_t> digest(picosha2::k_digest_size);
  picosha2::hash256(cborPayload.begin(), cborPayload.end(), digest.begin(),
                    digest.end());
  std::string bc32Digest = EncodeBc32Data(digest);

  std::vector<std::string> fragments;
  for (int i = 0; i * capacity < bc32Payload.size() - 1; i++) {
    fragments.push_back(bc32Payload.substr(i * capacity, capacity));
  }
  return ComposeHeadersToFragments(fragments, bc32Digest);
}

void CheckURHeader(const std::string &ur, const std::string &type) {
  if (StringToLower(ur) != StringToLower("ur:" + type)) {
    throw std::runtime_error("invalid UR header");
  }
}

void CheckDigest(const std::string &digest, const std::string &payload) {
  std::vector<uint8_t> decoded = DecodeBc32Data(payload);
  if (decoded.empty()) {
    throw std::runtime_error("can not decode payload");
  }
  std::vector<uint8_t> hash(picosha2::k_digest_size);
  picosha2::hash256(decoded.begin(), decoded.end(), hash.begin(), hash.end());
  if (DecodeBc32Data(digest) != hash) {
    throw std::runtime_error("invalid digest");
  }
}

std::pair<int, int> CheckAndGetSequence(const std::string &sequence) {
  std::vector<std::string> pieces = StringSplit(StringToLower(sequence), "of");
  if (pieces.size() != 2) {
    throw std::runtime_error("invalid sequence");
  }
  return {std::stoi(pieces[0]), std::stoi(pieces[1])};
}

std::string DealWithSingleWorkload(const std::string &workload,
                                   const std::string &type) {
  std::vector<std::string> pieces = StringSplit(workload, "/");
  switch (pieces.size()) {
  case 2:
    // ur:type/fragment
    CheckURHeader(pieces[0], type);
    return pieces[1];
  case 3:
    // ur:type/digest/fragment
    CheckURHeader(pieces[0], type);
    CheckDigest(pieces[1], pieces[2]);
    return pieces[2];
  case 4:
    // ur:type/sequencing/digest/fragment
    CheckURHeader(pieces[0], type);
    CheckAndGetSequence(pieces[1]);
    CheckDigest(pieces[2], pieces[3]);
    return pieces[3];
  default:
    throw std::runtime_error("invalid workload pieces length");
  }
}

std::string DealWithMultipleWorkloads(const std::vector<std::string> &workloads,
                                      const std::string &type) {
  int length = workloads.size();
  std::vector<std::string> fragments(length, "");
  std::string digest = {};
  for (auto &&workload : workloads) {
    std::vector<std::string> pieces = StringSplit(workload, "/");
    if (pieces.size() != 4) {
      throw std::runtime_error("invalid workload pieces length");
    }
    CheckURHeader(pieces[0], type);
    std::pair<int, int> indexAndTotal = CheckAndGetSequence(pieces[1]);
    if (indexAndTotal.second != length) {
      throw std::runtime_error("invalid workloads: total not equal length");
    }
    if (!digest.empty() && digest != pieces[2]) {
      throw std::runtime_error("invalid workloads: checksum changed");
    }
    digest = pieces[2];
    if (!fragments[indexAndTotal.first - 1].empty()) {
      throw std::runtime_error("invalid workloads: index has already been set");
    }
    fragments[indexAndTotal.first - 1] = pieces[3];
  }
  std::string payload = StringJoin(fragments, "");
  CheckDigest(digest, payload);
  return payload;
}

std::vector<uint8_t>
DecodeUniformResource(const std::vector<std::string> &workloads,
                      const std::string &type) {
  std::string bc32Payload = workloads.size() == 1
                                ? DealWithSingleWorkload(workloads[0], type)
                                : DealWithMultipleWorkloads(workloads, type);
  std::vector<uint8_t> cborPayload = DecodeBc32Data(bc32Payload);
  if (cborPayload.empty()) {
    throw std::runtime_error("invalid data");
  }
  return DecodeSimpleCBOR(cborPayload);
}

} // namespace bcr
} // namespace nunchuk