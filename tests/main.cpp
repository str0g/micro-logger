/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include <iostream>
#include <memory>
#include <thread>
#include <unordered_map>

#include "micro_logger.hpp"
#include "micro_logger_tools.hpp"
#include "micro_logger_writer.hpp"

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
  std::unordered_map<std::string, void (*)()> options;
  options["msg_hello_world"] = msg_hello_world;
  options["msg_null"] = msg_null;
  options["msg_trace"] = msg_trace;
  options["msg_critical"] = msg_critical;
  options["threads"] = threads;
  options["net"] = set_network_writer;
  options["file"] = set_file_writer;
  options["stdo"] = set_stdo_writer;

  for (int i = 1; i < argc; i++) {
    try {
      options.at(argv[i])();
    } catch (const std::out_of_range &e) {
      if (std::string("all") == argv[i]) {
        for (auto &option : options) {
          if (option.first == "net" or option.first == "file" or
              option.first == "stdo") {
            continue;
          }

          option.second();
        }
        break;
      }
    } catch (const std::domain_error &e) {
      std::cerr << e.what() << std::endl;
      break;
    }
  }

  return 0;
}
