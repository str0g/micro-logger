/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "thread_info.h"

namespace micro_logger {

std::string ThreadInfo::formatter() {
  char buf[128];
  std::snprintf(buf, sizeof(buf), "[pid:%08u][tid:%016llu]", pid, tid);
  return buf;
}
} // namespace micro_logger
