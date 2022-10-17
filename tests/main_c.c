/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "micro_logger.h"

int main() {
    void *writer = micro_logger_get_stdout_writer();
    micro_logger_set_writer(writer);

    MSG_DEBUG("checked with C11");
    MSG_DEBUG("%s%d", "checked with C", 11);

    return 0;
}
