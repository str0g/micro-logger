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
  ThreadInfo() = default;
  ThreadInfo(const ThreadInfo&) = delete;
  ThreadInfo(ThreadInfo&&) = delete;
  ThreadInfo& operator=(const ThreadInfo&) = delete;
  ThreadInfo& operator=(ThreadInfo&&) = delete;

  const pid_t pid = getpid();
  const std::thread::id tid = std::this_thread::get_id();
  const std::string info = std::move(formatter());

private:
  std::string formatter();
};
} // namespace micro_logger

#endif // MICRO_LOGGER_THREAD_INFO_H
