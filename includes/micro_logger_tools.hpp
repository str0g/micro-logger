/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef MICRO_LOGGER_MICRO_LOGGER_TOOLS_HPP
#define MICRO_LOGGER_MICRO_LOGGER_TOOLS_HPP

#include <string>
#include <vector>

namespace micro_logger {
/**
 * common function to dump object as hex
 * @param size
 * @return
 */
std::string bytes_to_hex(const uint8_t *, size_t size);

/**
 * reinterpret hex to something else in further step
 * @param hex
 * @return
 */
std::vector<uint8_t> hex_to_bytes(const std::string &hex);
} // namespace micro_logger

#endif // MICRO_LOGGER_MICRO_LOGGER_TOOLS_HPP
