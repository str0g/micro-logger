/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "micro_logger/micro_logger_tools.hpp"
//
#include <endian.h>
#include <gtest/gtest.h>
#include <stdexcept>
#include <vector>

class TestHex : public ::testing::Test {
public:
};

TEST_F(TestHex, hex_to_bytes_invalid_input) {
  {
    std::string in{"6f757"};
    EXPECT_THROW(micro_logger::hex_to_bytes(in), std::invalid_argument);
  }
  {
    std::string in{""};
    std::vector<uint8_t> exp;
    EXPECT_EQ(micro_logger::hex_to_bytes(in), exp);
  }
  {
    std::string in{"6f757Z"};
    EXPECT_THROW(micro_logger::hex_to_bytes(in), std::invalid_argument);
  }
}

TEST_F(TestHex, hex_conversion_text) {
  {
    std::string in{"out"};
    std::string exp{"6f7574"};
    auto out = micro_logger::bytes_to_hex(
        reinterpret_cast<const uint8_t *>(in.c_str()), in.size());

    EXPECT_EQ(out, exp);
    std::vector<uint8_t> exp_bytes{0x6f, 0x75, 0x74};
    EXPECT_EQ(micro_logger::hex_to_bytes(out), exp_bytes);
  }
  { // capital letter support
    std::string in{"6F7574"};
    std::vector<uint8_t> exp_bytes{0x6f, 0x75, 0x74};
    EXPECT_EQ(micro_logger::hex_to_bytes(in), exp_bytes);
  }
}

TEST_F(TestHex, hex_conversion_positive) {
  {
    int in{htole32(666)};
    std::string exp{"9a020000"};
    auto out = micro_logger::bytes_to_hex(
        reinterpret_cast<const uint8_t *>(&in), sizeof(in));

    EXPECT_EQ(out, exp);
    std::vector<uint8_t> exp_bytes{0x9a, 0x02, 0x00, 0x00};
    EXPECT_EQ(micro_logger::hex_to_bytes(out), exp_bytes);

    int revert = micro_logger::bytes_to_integral<int>(exp_bytes);

    EXPECT_EQ(be32toh(revert), in);
  }
  {
    int in{htobe32(666)};
    std::string exp{"0000029a"};
    auto out = micro_logger::bytes_to_hex(
        reinterpret_cast<const uint8_t *>(&in), sizeof(in));

    EXPECT_EQ(out, exp);
    std::vector<uint8_t> exp_bytes{0x00, 0x00, 0x02, 0x9a};
    EXPECT_EQ(micro_logger::hex_to_bytes(out), exp_bytes);

    int revert = micro_logger::bytes_to_integral<int>(exp_bytes);

    EXPECT_EQ(be32toh(revert), in);
  }
}

TEST_F(TestHex, hex_conversion_negative) {
  {
    int in{htole32(-666)};
    std::string exp{"66fdffff"};
    auto out = micro_logger::bytes_to_hex(
        reinterpret_cast<const uint8_t *>(&in), sizeof(in));

    EXPECT_EQ(out, exp);
    std::vector<uint8_t> exp_bytes{0x66, 0xfd, 0xff, 0xff};
    EXPECT_EQ(micro_logger::hex_to_bytes(out), exp_bytes);

    int revert = micro_logger::bytes_to_integral<int>(exp_bytes);

    EXPECT_EQ(be32toh(revert), in);
  }
  {
    int in{htobe32(-666)};
    std::string exp{"fffffd66"};
    auto out = micro_logger::bytes_to_hex(
        reinterpret_cast<const uint8_t *>(&in), sizeof(in));

    EXPECT_EQ(out, exp);
    std::vector<uint8_t> exp_bytes{0xff, 0xff, 0xfd, 0x66};
    EXPECT_EQ(micro_logger::hex_to_bytes(out), exp_bytes);

    int revert = micro_logger::bytes_to_integral<int>(exp_bytes);

    EXPECT_EQ(be32toh(revert), in);
  }
}
