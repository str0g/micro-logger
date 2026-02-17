/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "micro_logger.hpp"
#include "micro_logger_writer.hpp"
//
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <thread>
#include <unordered_map>

using namespace std::literals;

std::shared_ptr<micro_logger::BaseWriter> writer;

std::string g_address{"127.0.0.1"};
int g_port{6024};
std::string g_file{"log.txt"};

void set_network_writer() {
  writer = std::make_shared<micro_logger::NetworkWriter>(g_address, g_port);
  micro_logger::initialize(*writer);
}

void set_file_writer() {
  writer = std::make_shared<micro_logger::FileWriter>(g_file.c_str());
  micro_logger::initialize(*writer);
}

void set_stdo_writer() {
  writer = std::make_shared<micro_logger::StandardOutWriter>();
  micro_logger::initialize(*writer);
};

void set_silent_writer() {
  writer = std::make_shared<micro_logger::SilentWriter>();
  micro_logger::initialize(*writer);
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

void msg_threads() {
  std::thread th1([&]() {
    auto tid = std::this_thread::get_id();
    MSG_INFO("hello %llu", tid);
  });
  std::thread th2([&]() {
    auto tid = std::this_thread::get_id();
    MSG_WARN("world %llu", tid);
  });
  th1.join();
  th2.join();
}

class SampleSingletonClass {
public:
  static SampleSingletonClass &get_instance() {
    static SampleSingletonClass obj;
    return obj;
  }
  ~SampleSingletonClass() {
    MSG_ENTER();
    MSG_EXIT();
  }

private:
  SampleSingletonClass(const SampleSingletonClass &) = delete;
  SampleSingletonClass(SampleSingletonClass &&) = delete;
  SampleSingletonClass &operator=(const SampleSingletonClass &) = delete;
  SampleSingletonClass &operator=(SampleSingletonClass &&) = delete;
  SampleSingletonClass() = default;
};

void singleton() { auto &obj = SampleSingletonClass::get_instance(); }

/**
 * for main out_of_range error is reserved exception.
 * If being throw in other case then "option all" expect unexpected.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char **argv) {
  std::unordered_map<std::string, void (*)()> options = {
      {"--msg_hello_world", msg_hello_world},
      {"--msg_null", msg_null},
      {"--msg_trace", msg_trace},
      {"--msg_critical", msg_critical},
      {"--msg_threads", msg_threads},
      {"--singleton", singleton},
      {"--net", set_network_writer},
      {"--file", set_file_writer},
      {"--stdo", set_stdo_writer},
      {"--silent", set_silent_writer},
  };

  std::set<std::string> opt_writers{
      {"--stdo"},
      {"--file"},
      {"--net"},
      {"--silent"},
  };

  for (int i = 1; i < argc; i++) {
    try {
      std::string opt{argv[i]};
      auto index = opt.find('=');
      if (index != std::string::npos) {
        auto sub_args = opt.substr(index + 1);
        opt.erase(index);
        if (opt == "--net") {
          auto separator = sub_args.find(':');
          if (separator != std::string::npos) {
            auto port_str = sub_args.substr(separator + 1);
            size_t idx{};
            g_port = std::stoi(port_str, &idx);
            auto new_address = sub_args.substr(0, separator);
            g_address.swap(new_address);
          }
        }
        if (opt == "--file") {
          g_file.swap(sub_args);
        }
      }
      options.at(opt)();
    } catch (const std::out_of_range &e) {
      if (argv[i] == "all"s) {
        for (auto &option : options) {
          if (opt_writers.contains(option.first)) {
            continue;
          }
          option.second();
        }
      }
      if (argv[i] == "--help"s) {
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
        std::cout << "example: --stdo --msg_hello_world" << std::endl;
        break;
      }
    } catch (const std::domain_error &e) {
      std::cerr << e.what() << std::endl;
      break;
    }
  }

  return 0;
}
