// Copyright (c) 2020 Enigmo
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <bc32.h>

namespace nunchuk {
namespace bcr {

const char *CHARSET = "qpzry9x8gf2tvdw0s3jn54khce6mua7l";

const int8_t CHARSET_REV[128] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 15, -1, 10, 17, 21, 20, 26, 30, 7,
    5,  -1, -1, -1, -1, -1, -1, -1, 29, -1, 24, 13, 25, 9,  8,  23, -1, 18, 22,
    31, 27, 19, -1, 1,  0,  3,  16, 11, 28, 12, 14, 6,  4,  2,  -1, -1, -1, -1,
    -1, -1, 29, -1, 24, 13, 25, 9,  8,  23, -1, 18, 22, 31, 27, 19, -1, 1,  0,
    3,  16, 11, 28, 12, 14, 6,  4,  2,  -1, -1, -1, -1, -1};

uint32_t PolyMod(const std::vector<uint8_t> &v) {
  uint32_t c = 1;
  for (const auto v_i : v) {
    uint8_t c0 = c >> 25;
    c = ((c & 0x1ffffff) << 5) ^ v_i;
    if (c0 & 1)
      c ^= 0x3b6a57b2;
    if (c0 & 2)
      c ^= 0x26508e6d;
    if (c0 & 4)
      c ^= 0x1ea119fa;
    if (c0 & 8)
      c ^= 0x3d4233dd;
    if (c0 & 16)
      c ^= 0x2a1462b3;
  }
  return c;
}

std::vector<uint8_t> ConvertBits(const std::vector<uint8_t> data, int fromBits,
                                 int toBits, bool pad) {
  int acc = 0;
  int bits = 0;
  int maxv = (1 << toBits) - 1;
  std::vector<uint8_t> ret;

  for (auto &&value : data) {
    short b = (short)(value & 0xff);
    if ((b >> fromBits) > 0) {
      return {};
    }

    acc = (acc << fromBits) | b;
    bits += fromBits;
    while (bits >= toBits) {
      bits -= toBits;
      ret.push_back(((acc >> bits) & maxv));
    }
  }

  if (pad && (bits > 0)) {
    ret.push_back(((acc << (toBits - bits)) & maxv));
  } else if (bits >= fromBits || (((acc << (toBits - bits)) & maxv)) != 0) {
    return {};
  }
  return ret;
}

bool VerifyChecksum(const std::vector<uint8_t> &values) {
  std::vector<uint8_t> enc = {0};
  enc.insert(enc.end(), values.begin(), values.end());
  return PolyMod(enc) == 0x3fffffff;
}

std::vector<uint8_t> CreateChecksum(const std::vector<uint8_t> &values) {
  std::vector<uint8_t> enc = {0};
  enc.insert(enc.end(), values.begin(), values.end());
  enc.resize(enc.size() + 6);
  uint32_t mod = PolyMod(enc) ^ 0x3fffffff;
  std::vector<uint8_t> ret(6);
  for (size_t i = 0; i < 6; ++i) {
    ret[i] = (mod >> (5 * (5 - i))) & 31;
  }
  return ret;
}

std::string EncodeBc32Data(const std::vector<uint8_t> &values) {
  std::vector<uint8_t> data = ConvertBits(values, 8, 5, true);
  if (data.empty()) {
    throw std::runtime_error("Invalid input");
  }
  std::vector<uint8_t> checksum = CreateChecksum(data);

  std::vector<uint8_t> combined;
  combined.insert(combined.end(), data.begin(), data.end());
  combined.insert(combined.end(), checksum.begin(), checksum.end());
  std::string ret;
  ret.reserve(combined.size());
  for (const auto c : combined) {
    ret += CHARSET[c];
  }
  return ret;
}

std::vector<uint8_t> DecodeBc32Data(const std::string &str) {
  bool lower = false, upper = false;
  for (size_t i = 0; i < str.size(); ++i) {
    unsigned char c = str[i];
    if (c >= 'a' && c <= 'z')
      lower = true;
    else if (c >= 'A' && c <= 'Z')
      upper = true;
    else if (c < 33 || c > 126)
      return {};
  }
  if (lower && upper)
    return {};

  std::vector<uint8_t> values(str.size());
  for (size_t i = 0; i < str.size(); ++i) {
    unsigned char c = str[i];
    int8_t rev = CHARSET_REV[c];
    if (rev == -1) {
      return {};
    }
    values[i] = rev;
  }
  if (!VerifyChecksum(values)) {
    return {};
  }
  return ConvertBits(std::vector<uint8_t>(values.begin(), values.end() - 6), 5,
                     8, false);
}

} // namespace bcr
} // namespace nunchuk
