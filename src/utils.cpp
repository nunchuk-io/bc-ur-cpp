// Copyright (c) 2020 Enigmo
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <utils.h>

#include <algorithm>
#include <cctype>
#include <sstream>

namespace nunchuk {
namespace bcr {

std::vector<uint8_t> HexToBytes(const std::string &hex) {
  std::vector<uint8_t> bytes;
  for (unsigned int i = 0; i < hex.length(); i += 2) {
    std::string byteString = hex.substr(i, 2);
    uint8_t byte = (uint8_t)strtol(byteString.c_str(), NULL, 16);
    bytes.push_back(byte);
  }
  return bytes;
}

std::string BytesToHex(const std::vector<uint8_t> &bytes) {
  std::string rv;
  static constexpr char hexmap[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                      '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  rv.reserve(bytes.size() * 2);
  for (uint8_t v : bytes) {
    rv.push_back(hexmap[v >> 4]);
    rv.push_back(hexmap[v & 15]);
  }
  return rv;
}

std::vector<std::string> StringSplit(const std::string &s,
                                     const std::string &delim) {
  std::vector<std::string> pieces;
  auto start = 0U;
  auto end = s.find(delim);

  while (end != std::string::npos) {
    pieces.push_back(s.substr(start, end - start));
    start = end + delim.size();
    end = s.find(delim, start);
  }

  pieces.push_back(s.substr(start, end));
  return pieces;
}

std::string StringJoin(const std::vector<std::string> &v,
                       const std::string &delim) {
  std::stringstream s;
  for (const auto &i : v) {
    if (&i != &v[0]) {
      s << delim;
    }
    s << i;
  }
  return s.str();
}

std::string StringToLower(const std::string &s) {
  std::string rs(s);
  std::transform(s.begin(), s.end(), rs.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return rs;
}

} // namespace bcr
} // namespace nunchuk