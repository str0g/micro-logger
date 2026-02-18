/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "common.h"
#include "micro_logger.hpp"
//
#include <gtest/gtest.h>
#include <ranges>
#include <regex>
#include <string>
#include <thread>
#include <unistd.h>

using namespace std::string_literals;
using namespace std::chrono_literals;

std::regex regex_pattern(
    R"(\[(\d{2}/\d{2}/\d{2})[ ](\d{2}:\d{2}:\d{2}\.\d{3})\]\[(TRACE|DEBUG|INFO[ ]|WARN[ ]|ERROR|CRITI)\]\[pid:(\d{8})\]\[tid:(\d{16})\]\[(\w+[-\w]*\.[cp]{1,3}):(\d{1,3})::([^\]]+)\]\[(.*?)\])",
    std::regex::ECMAScript);

void setup_logger() { micro_logger::initialize(TestWriter::get_instance()); }

class TestPatterns : public ::testing::Test {
public:
protected:
  static void SetUpTestSuite() { setup_logger(); }
};

TEST_F(TestPatterns, generic_pattern) {
  auto &obj = TestWriter::get_instance();

  int line_num = __LINE__;
  MSG_DEBUG("debug");       // 0
  MSG_INFO("info");         // 1
  MSG_WARN("warning");      // 2
  MSG_ERROR("error");       // 3
  MSG_CRITICAL("critical"); // 4

  std::string filename_exp{"test_pattern.cpp"s};

  auto tid = get_tid();
  pid_t pid = getpid();

  std::vector<std::smatch> matches;
  matches.resize(5);

  const std::vector<logged_data_exp> exp{
      {
          .log_level = "DEBUG",
          .pid = pid,
          .tid = tid,
          .filename = filename_exp,
          .line_number = "0" + std::to_string(++line_num),
          .function = __FUNCTION__,
          .message = "debug",
      },
      {
          .log_level = "INFO ",
          .pid = pid,
          .tid = tid,
          .filename = filename_exp,
          .line_number = "0" + std::to_string(++line_num),
          .function = __FUNCTION__,
          .message = "info",
      },
      {
          .log_level = "WARN ",
          .pid = pid,
          .tid = tid,
          .filename = filename_exp,
          .line_number = "0" + std::to_string(++line_num),
          .function = __FUNCTION__,
          .message = "warning",
      },
      {
          .log_level = "ERROR",
          .pid = pid,
          .tid = tid,
          .filename = filename_exp,
          .line_number = "0" + std::to_string(++line_num),
          .function = __FUNCTION__,
          .message = "error",
      },
      {
          .log_level = "CRITI",
          .pid = pid,
          .tid = tid,
          .filename = filename_exp,
          .line_number = "0" + std::to_string(++line_num),
          .function = __FUNCTION__,
          .message = "critical",
      }};

  for (const auto [index, line] : std::views::enumerate(obj.line_buffer)) {
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

TEST_F(TestPatterns, tid) {
  auto &obj = TestWriter::get_instance();

  int line_num = __LINE__;
  MSG_INFO("info");

  std::string filename_exp{"test_pattern.cpp"s};

  auto tid = get_tid();
  pid_t pid = getpid();
  size_t thread_tid;
  int thread_line_num;
  std::string function_name;

  std::thread th([&thread_tid, &thread_line_num, &function_name]() {
    thread_tid = get_tid();
    function_name = __FUNCTION__;
    thread_line_num = __LINE__;
    MSG_WARN("warning");
    std::this_thread::sleep_for(10ms);
  });
  // reason for this construction fast enough so thread is still alive slow
  // enought it's not yet ready to be joined.
  std::this_thread::sleep_for(2ms);
  int line_num_after_thread = __LINE__;
  MSG_INFO("info");
  th.join();

  std::vector<std::smatch> matches;
  matches.resize(3);

  const std::vector<logged_data_exp> exp{
      {
          .log_level = "INFO ",
          .pid = pid,
          .tid = tid,
          .filename = filename_exp,
          .line_number = std::to_string(++line_num),
          .function = __FUNCTION__,
          .message = "info",
      },
      {
          .log_level = "WARN ",
          .pid = pid,
          .tid = thread_tid,
          .filename = filename_exp,
          .line_number = std::to_string(++thread_line_num),
          .function = function_name,
          .message = "warning",
      },
      {
          .log_level = "INFO ",
          .pid = pid,
          .tid = tid,
          .filename = filename_exp,
          .line_number = std::to_string(++line_num_after_thread),
          .function = __FUNCTION__,
          .message = "info",
      },
  };

  for (const auto [index, line] : std::views::enumerate(obj.line_buffer)) {
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
