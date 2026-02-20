/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "common.h"
#include "micro_logger/micro_logger.hpp"
//
#include <gtest/gtest.h>
#include <regex>

class NoOperator {
public:
  const std::string world{"world"};
};

class Hello : public NoOperator {
public:
  friend std::ostream &operator<<(std::ostream &os, const Hello &obj) {
    os << obj.world;
    return os;
  }
};

void setup_logger() { micro_logger::initialize(TestWriter::get_instance()); }

class TestToString : public ::testing::Test {
public:
protected:
  static void SetUpTestSuite() { setup_logger(); }
};

TEST_F(TestToString, without_operator_address_only) {
  NoOperator nop;
  MSG_INFO(micro_logger::to_string(&nop).c_str());
  const auto &line_buffer = TestWriter::get_instance().line_buffer;
  ASSERT_EQ(line_buffer.size(), 1);
  constexpr size_t number_of_postfix_characters = 2;
  const auto word_start_possition =
      line_buffer[0].size() - 8 - number_of_postfix_characters;
  auto substr = line_buffer[0].substr(word_start_possition, 8);
  std::regex pattern("[a-z0-9]+");
  EXPECT_EQ(std::regex_match(substr, pattern), true);
}

TEST_F(TestToString, with_operator) {
  Hello hello;
  MSG_INFO(micro_logger::to_string(hello).c_str());

  const auto &line_buffer = TestWriter::get_instance().line_buffer;
  ASSERT_EQ(line_buffer.size(), 1);
  constexpr size_t number_of_postfix_characters = 2;
  const auto word_start_possition =
      line_buffer[0].size() - hello.world.size() - number_of_postfix_characters;
  EXPECT_EQ(line_buffer[0].substr(word_start_possition, hello.world.size()),
            hello.world);
}
