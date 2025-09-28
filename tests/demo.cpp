/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <iostream>
#include <memory>
#include <set>
#include <thread>
#include <unordered_map>

#include "micro_logger.hpp"
#include "micro_logger_tools.hpp"
#include "micro_logger_writer.hpp"

using namespace std::literals;

std::shared_ptr<micro_logger::BaseWriter> writer;

void set_network_writer() {
  writer = std::make_shared<micro_logger::NetworkWriter>("127.0.0.1", 6024);
  set_writer(*writer);
}

void set_file_writer() {
  writer = std::make_shared<micro_logger::FileWriter>("log.txt");
  set_writer(*writer);
}

void set_stdo_writer() {
  writer = std::make_shared<micro_logger::StandardOutWriter>();
  set_writer(*writer);
};

void set_silent_writer() {
  writer = std::make_shared<micro_logger::SilentWriter>();
  set_writer(*writer);
};

void msg_hello_world() { MSG_DEBUG("hello world"); }

void msg_null() { MSG_ERROR("%s", nullptr); }

void msg_trace() {
  MSG_ENTER();
  MSG_EXIT();
}

void msg_critical() {
  MSG_CRITICAL("%s %d %s", "run out of chocolate for", 1, "time!");
}

void threads() {
  std::thread th1([&]() {
    auto tid = std::this_thread::get_id();
    auto tid_to_text = micro_logger::bytes_to_hex(
        reinterpret_cast<const uint8_t *>(&tid), sizeof(tid));
    MSG_INFO("hello %s", tid_to_text.c_str());
  });
  std::thread th2([&]() {
    auto tid = std::this_thread::get_id();
    auto tid_to_text = micro_logger::bytes_to_hex(
        reinterpret_cast<const uint8_t *>(&tid), sizeof(tid));
    MSG_WARN("world %s", tid_to_text.c_str());
  });
  th1.join();
  th2.join();
}

/**
 * for main out_of_range error is reserved exception.
 * If being throw in other case then "option all" expect unexpected.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char **argv) {
  std::unordered_map<std::string, void (*)()> options = {
      {"msg_hello_world", msg_hello_world},
      {"msg_null", msg_null},
      {"msg_trace", msg_trace},
      {"msg_critical", msg_critical},
      {"threads", threads},
      {"net", set_network_writer},
      {"file", set_file_writer},
      {"stdo", set_stdo_writer},
      {"silent", set_silent_writer},
  };

  std::set<std::string> opt_writers{
      {"stdo"},
      {"file"},
      {"net"},
      {"silent"},
  };

  for (int i = 1; i < argc; i++) {
    try {
      options.at(argv[i])();
    } catch (const std::out_of_range &e) {
      if (argv[i] == "all"s) {
        for (auto &option : options) {
          if (opt_writers.contains(option.first)) {
            continue;
          }
          option.second();
        }
      }
      if (argv[i] == "help"s) {
        std::cout << "Pick writer and then msg to write" << std::endl;
        std::cout << "writers:";
        for (auto &opt_w : opt_writers) {
          std::cout << " [" << opt_w << "]";
        }
        std::cout << std::endl;
        std::cout << "msg to write:";
        for (auto &option : options) {
          if (opt_writers.contains(option.first)) {
            continue;
          }
          std::cout << " [" << option.first << "]";
        }
        std::cout << std::endl;
        std::cout << "example: stdo msg_hello_world" << std::endl;
        break;
      }
    } catch (const std::domain_error &e) {
      std::cerr << e.what() << std::endl;
      break;
    }
  }

  return 0;
}
