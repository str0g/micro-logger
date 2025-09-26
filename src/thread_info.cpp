/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "thread_info.h"
#include "micro_logger_tools.hpp"
#include <format>

namespace micro_logger {
ThreadInfo::ThreadInfo()
    : tid(std::this_thread::get_id()), pid(getpid()), info(formatter()) {}

std::string ThreadInfo::formatter() {
  return std::format("[pid:{:<8}][tid:{:<16}]",
                micro_logger::bytes_to_hex(
                    reinterpret_cast<const uint8_t *>(&pid), sizeof(pid)),
                micro_logger::bytes_to_hex(
                    reinterpret_cast<const uint8_t *>(&tid), sizeof(tid)));
}
} // namespace micro_logger
