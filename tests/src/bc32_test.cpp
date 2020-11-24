// Copyright (c) 2020 Enigmo
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <bc32.h>
#include <doctest.h>
#include <utils.h>

using namespace nunchuk::bcr;

TEST_CASE("testing encode bc-bech32 data") {
  CHECK(EncodeBc32Data(HexToBytes("48656c6c6f20776f726c64")) ==
        "fpjkcmr0ypmk7unvvsh4ra4j");
  CHECK(EncodeBc32Data(HexToBytes(
            "d934063e82001eec0585ee41ab5d8e4b703a4be1f73aec21e143912c56")) ==
        "my6qv05zqq0wcpv9aeq6khvwfdcr5jlp7uawcg0pgwgjc4shjm6xu");
}

TEST_CASE("testing decode bc-bech32 data") {
  CHECK(BytesToHex(DecodeBc32Data("fpjkcmr0ypmk7unvvsh4ra4j")) ==
        "48656c6c6f20776f726c64");
  CHECK(BytesToHex(DecodeBc32Data(
            "my6qv05zqq0wcpv9aeq6khvwfdcr5jlp7uawcg0pgwgjc4shjm6xu")) ==
        "d934063e82001eec0585ee41ab5d8e4b703a4be1f73aec21e143912c56");
}
