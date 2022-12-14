/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef MICRO_LOGGER_THREAD_INFO_H
#define MICRO_LOGGER_THREAD_INFO_H

#include <string>
#include <thread>
#include <unistd.h>

namespace micro_logger {
class ThreadInfo {
public:
  ThreadInfo();

  pid_t pid;
  std::thread::id tid;
  const std::string info;

private:
  std::string formatter();
};
} // namespace micro_logger

#endif // MICRO_LOGGER_THREAD_INFO_H
