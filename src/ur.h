// Copyright (c) 2020 Enigmo
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef NUNCHUK_BCR_UR_H
#define NUNCHUK_BCR_UR_H

#include <stdint.h>
#include <string>
#include <vector>

namespace nunchuk {
namespace bcr {

std::vector<std::string>
EncodeUniformResource(const std::vector<uint8_t> &payload, int capacity = 200);

std::vector<uint8_t>
DecodeUniformResource(const std::vector<std::string> &workloads,
                      const std::string &type = "bytes");

} // namespace bcr
} // namespace nunchuk

#endif // NUNCHUK_BCR_UR_H
