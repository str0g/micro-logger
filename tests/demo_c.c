/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "micro_logger.h"
#include <errno.h>
#include <getopt.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

const char *default_address = "127.0.0.1";
int default_port = 6024;
const char *default_file = "log.txt";

static void usage(const char *prog) {
  fprintf(stderr,
          "Usage: %s [OPTIONS] [--] [writer] [predefined message scenerio]\n\n"
          "Options:\n"
          "  -h, --help             Show this help\n"
          "  -n, --net              Write network, default: %s:%d\n"
          "  -f, --file             Writer to file, default: %s\n"
          "  -o, --stdo             Writer standard out\n"
          "  -s, --silent           Writer output is turned off\n"
          "      --msg_hello_world  Prints \"hello world\"\n"
          "      --msg_null         Prints message with NULL value\n"
          "      --msg_trace        Prints enter/exit traces\n"
          "      --msg_critical     Prints message on critical level\n"
          "      --threads          Prints message from 2 threads\n",
          prog, default_address, default_port, default_file);
}

static int worker(void *arg) {
  MSG_INFO("%s %016llu", arg, pthread_self());
  return 0;
}
static void threads() {
  thrd_t t1, t2;
  thrd_create(&t1, worker, "hello");
  thrd_create(&t2, worker, "world");

  thrd_join(t1, NULL);
  thrd_join(t2, NULL);
}

enum Scenerios {
  DO_NOTHING = 0x0,
  MSG_HELLO_WORLD = 0x01,
  MSG_NULL = 0x02,
  MSG_TRACE = 0x04,
  MSG_CRITICAL_ = 0x08,
  THREADS = 0x10
};

int main(int argc, char *argv[]) {
  int opt;
  int long_index = 0;

  static struct option long_opts[] = {
      {"help", no_argument, NULL, 'h'},
      {"net", optional_argument, NULL, 'n'},
      {"file", optional_argument, NULL, 'f'},
      {"stdo", no_argument, NULL, 'o'},
      {"silent", no_argument, NULL, 's'},
      {"msg_hello_world", no_argument, NULL, MSG_HELLO_WORLD},
      {"msg_null", no_argument, NULL, MSG_NULL},
      {"msg_trace", no_argument, NULL, MSG_TRACE},
      {"msg_critical", no_argument, NULL, MSG_CRITICAL_},
      {"threads", no_argument, NULL, THREADS},
      {NULL, 0, NULL, 0}};

  const char *short_opts = "hn::f::os";
  while ((opt = getopt_long(argc, argv, short_opts, long_opts, &long_index)) !=
         -1) {
    switch (opt) {
    case 'h':
      usage(argv[0]);
      return 0;
    case 'n':
      if (optarg) {
        int i = 0;
        while (optarg[i] && optarg[i++] != ':')
          ;
        char *end;
        default_port = strtol(&optarg[i], &end, 10);
        if ((errno != 0) | (*end != '\0')) {
          fprintf(stderr, "failed to convert port to number [%s][%p]\n",
                  strerror(errno), end);
          return 1;
        }
        optarg[i - 1] = '\0';
        default_address = optarg;
      }
      micro_logger_set_writer(
          micro_logger_get_net_writer(default_address, default_port));
      break;
    case 'f':
      if (optarg) {
        default_file = optarg;
      }
      micro_logger_set_writer(micro_logger_get_file_writer(default_file));
      break;
    case 'o':
      micro_logger_set_writer(micro_logger_get_stdout_writer());
      break;
    case 's':
      micro_logger_set_writer(micro_logger_get_silent_writer());
      break;
    case MSG_HELLO_WORLD:
      MSG_DEBUG("hello world");
      break;
    case MSG_NULL:
      MSG_ERROR("%s", NULL);
      break;
    case MSG_TRACE:
      MSG_ENTER();
      MSG_EXIT();
      break;
    case MSG_CRITICAL_:
      MSG_CRITICAL("%s %d %s", "run out of chocolate for", 1, "time!");
      break;
    case THREADS:
      threads();
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
