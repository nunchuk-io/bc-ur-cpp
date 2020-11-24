// Copyright (c) 2020 Enigmo
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef NUNCHUK_BCR_BC32_H
#define NUNCHUK_BCR_BC32_H

#include <stdint.h>
#include <string>
#include <vector>

namespace nunchuk {
namespace bcr {

std::string EncodeBc32Data(const std::vector<uint8_t> &values);

std::vector<uint8_t> DecodeBc32Data(const std::string &str);

} // namespace bcr
} // namespace nunchuk

#endif // NUNCHUK_BCR_BC32_H
