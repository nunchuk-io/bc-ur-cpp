// Copyright (c) 2020 Enigmo
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef NUNCHUK_BCR_UTILS_H
#define NUNCHUK_BCR_UTILS_H

#include <stdint.h>
#include <string>
#include <vector>

namespace nunchuk {
namespace bcr {

std::vector<uint8_t> HexToBytes(const std::string &hex);

std::string BytesToHex(const std::vector<uint8_t> &bytes);

std::vector<std::string> StringSplit(const std::string &s,
                                     const std::string &delim);

std::string StringJoin(const std::vector<std::string> &v,
                       const std::string &delim);

std::string StringToLower(const std::string &s);

} // namespace bcr
} // namespace nunchuk

#endif // NUNCHUK_BCR_UTILS_H
