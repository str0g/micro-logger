/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "thread_info.h"
#include <iomanip>
#include <format>

namespace micro_logger {

std::string ThreadInfo::formatter() {
  std::stringstream os;
  os << std::setw(16) << std::setfill('0') << tid;
  return std::format("[pid:{:08}]{}", pid ,os.str());
}

} // namespace micro_logger
