/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "common.h"
#include "micro_logger.hpp"
//
#include <gtest/gtest.h>
#include <regex>

using namespace std::string_literals;

std::regex regex_pattern(
    R"(\[(\d{2}/\d{2}/\d{2})[ ](\d{2}:\d{2}:\d{2}\.\d{3})\]\[(TRACE|DEBUG|INFO[ ]|WARN[ ]|ERROR|CRITI)\]\[pid:(\d{8})\]\[tid:(\d{16})\]\[(\w+[-\w]*\.[cp]{1,3}):(\d{1,5})::([^\]]+)\]\[(.*?)\])",
    std::regex::ECMAScript);

constexpr micro_logger_CustomParameters new_parameters{
    .header_size = 128,
    .message_size = 6,
    .header_pattern = "[%%s]%s[%%%ss:%%%sd::%%s][%%s]\n",
    .align_filename_length = "",
    .align_lines_length = "05",
    .time_format = "[%D %T",
    .milliseconds_format = ".%03ld]",
};

void setup_logger() {
  micro_logger::initialize(TestWriter::get_instatnce(), &new_parameters);
}

class TestCustomParamters : public ::testing::Test {
public:
protected:
  static void SetUpTestSuite() { setup_logger(); }
};

TEST_F(TestCustomParamters, new_parameters) {
  auto msg{"hello world"s};
  int line_num = __LINE__;
  MSG_INFO(msg.c_str());

  const auto &line_bufer = TestWriter::get_instatnce().line_bufer;
  ASSERT_EQ(line_bufer.size(), 1);
  std::vector<std::smatch> matches;
  matches.resize(1);
  auto tid = get_tid();
  pid_t pid = getpid();
  std::string filename_exp{"test_custom_parameters.cpp"s};
  const std::vector<logged_data_exp> exp{
      {
          .log_level = "INFO ",
          .pid = pid,
          .tid = tid,
          .filename = filename_exp,
          .line_number = "000" + std::to_string(++line_num),
          .function = __FUNCTION__,
          .message = "hello",
      },
  };

  for (const auto [index, line] : std::views::enumerate(line_bufer)) {
    std::cout << line << std::endl;
    std::cout << "---" << std::endl;
    EXPECT_TRUE(std::regex_search(line, matches[index], regex_pattern));
    const logged_data out{
        .data = matches[index][1],
        .time = matches[index][2],
        .log_level = matches[index][3],
        .pid = std::stoi(matches[index][4]),
        .tid = std::stoul(matches[index][5]),
        .filename = matches[index][6],
        .line_number = matches[index][7],
        .function = matches[index][8],
        .message = matches[index][9],
    };
    EXPECT_EQ(out, exp[index]);
  }
}
