/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "micro_logger.hpp"
#include "micro_logger_writer.hpp"
//
#include <cstdlib>
#include <cstring>
#include <getopt.h>
#include <pthread.h>
#include <string>
#include <thread>

std::shared_ptr<micro_logger::BaseWriter> writer;

std::string g_address = "127.0.0.1";
int g_port{6024};
const char* g_file = "log.txt";
int writer_not_set = 1;

void set_network_writer() {
  writer = std::make_shared<micro_logger::NetworkWriter>(g_address, g_port);
  micro_logger::initialize(*writer);
}

void set_file_writer() {
  writer = std::make_shared<micro_logger::FileWriter>(g_file);
  micro_logger::initialize(*writer);
}

void set_stdo_writer() {
  writer = std::make_shared<micro_logger::StandardOutWriter>();
  micro_logger::initialize(*writer);
}

void set_silent_writer() {
  writer = std::make_shared<micro_logger::SilentWriter>();
  micro_logger::initialize(*writer);
}

static void worker_warn() {
  MSG_WARN("%s %016llu", "world", (unsigned long long)pthread_self());
}

void msg_threads() {
  std::thread th1([]() {
    MSG_INFO("%s %016llu", "hello", (unsigned long long)pthread_self());
  });
  std::thread th2(worker_warn);
  th1.join();
  th2.join();
}

void msg_hello_world() { MSG_DEBUG("hello world"); }

void msg_null() { MSG_ERROR("%s", nullptr); }

void msg_trace() {
  MSG_ENTER();
  MSG_EXIT();
}

void msg_critical() {
  MSG_CRITICAL("%s %d %s", "run out of chocolate for", 1, "time!");
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

static void usage(const char *prog) {
  fprintf(stderr,
          "Usage: %s [OPTIONS] [--] [writer] [predefined message scenerio]\n\n"
          "Options:\n"
          "  -h, --help             Show this help\n"
          "  -n, --net              Write network, default: %s:%d\n"
          "  -f, --file             Writer to file, default: %s\n"
          "  -o, --stdo             Writer standard out\n"
          "  -s, --silent           Writer output is turned off\n"
          "  -c, --crash            Try to log without writer\n"
          "      --msg_hello_world  Prints \"hello world\"\n"
          "      --msg_null         Prints message with NULL value\n"
          "      --msg_trace        Prints enter/exit traces\n"
          "      --msg_critical     Prints message on critical level\n"
          "      --msg_threads      Prints message from 2 threads\n"
          "      --msg_all          Prints all message types\n"
          "      --msg_singleton     Demonstrates singleton pattern\n",
          prog, g_address.c_str(), g_port, g_file);
}

static void exit_if_writer_not_set(const char *prog) {
  if (writer_not_set) {
    usage(prog);
    exit(2);
  }
}

enum Scenarios {
  DO_NOTHING_OPT = 0x0,
  MSG_HELLO_WORLD_OPT = 0x01,
  MSG_NULL_OPT = 0x02,
  MSG_TRACE_OPT = 0x04,
  MSG_CRITICAL_OPT = 0x08,
  MSG_THREADS_OPT = 0x10,
  MSG_ALL_OPT = 0x40,
  SINGLETON_OPT = 0x80,
};

int main(int argc, char **argv) {
  int opt;
  int long_index = 0;

  static struct option long_opts[] = {
      {"help", no_argument, NULL, 'h'},
      {"net", optional_argument, NULL, 'n'},
      {"file", optional_argument, NULL, 'f'},
      {"stdo", no_argument, NULL, 'o'},
      {"silent", no_argument, NULL, 's'},
      {"crash", no_argument, NULL, 'c'},
      {"msg_hello_world", no_argument, NULL, MSG_HELLO_WORLD_OPT},
      {"msg_null", no_argument, NULL, MSG_NULL_OPT},
      {"msg_trace", no_argument, NULL, MSG_TRACE_OPT},
      {"msg_critical", no_argument, NULL, MSG_CRITICAL_OPT},
      {"msg_threads", no_argument, NULL, MSG_THREADS_OPT},
      {"msg_all", no_argument, NULL, MSG_ALL_OPT},
      {"msg_singleton", no_argument, NULL, SINGLETON_OPT},
      {NULL, 0, NULL, 0}};

  const char *short_opts = "hn::f::osc";
  while ((opt = getopt_long(argc, argv, short_opts, long_opts, &long_index)) !=
         -1) {
    switch (opt) {
    case 'h':
      usage(argv[0]);
      return 0;
    case 'n':
      if (optarg) {
        std::string arg{optarg};
        auto sep = arg.find(':');
        if (sep != std::string::npos) {
          g_address = arg.substr(0, sep);
          g_port = std::stoi(arg.substr(sep + 1));
        }
      }
      set_network_writer();
      writer_not_set = 0;
      break;
    case 'f':
      if (optarg) {
        g_file = optarg;
      }
      set_file_writer();
      writer_not_set = 0;
      break;
    case 'o':
      set_stdo_writer();
      writer_not_set = 0;
      break;
    case 's':
      set_silent_writer();
      writer_not_set = 0;
      break;
    case 'c':
      MSG_ERROR("expected to crash");
      break;
    case MSG_HELLO_WORLD_OPT:
      exit_if_writer_not_set(argv[0]);
      msg_hello_world();
      break;
    case MSG_NULL_OPT:
      exit_if_writer_not_set(argv[0]);
      msg_null();
      break;
    case MSG_TRACE_OPT:
      exit_if_writer_not_set(argv[0]);
      msg_trace();
      break;
    case MSG_CRITICAL_OPT:
      exit_if_writer_not_set(argv[0]);
      msg_critical();
      break;
    case MSG_THREADS_OPT:
      exit_if_writer_not_set(argv[0]);
      msg_threads();
      break;
    case MSG_ALL_OPT:
      exit_if_writer_not_set(argv[0]);
      msg_hello_world();
      msg_null();
      msg_trace();
      msg_critical();
      msg_threads();
      break;
    case SINGLETON_OPT:
      exit_if_writer_not_set(argv[0]);
      singleton();
      break;
    case '?':
    default:
      if (optopt) {
        fprintf(stderr, "Unknown option `-%c'.\n", optopt);
      } else {
        fprintf(stderr, "Unknown option.\n");
      }
      usage(argv[0]);
      return 2;
    }
  }
  return 0;
}
