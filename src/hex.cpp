/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#include "micro_logger_tools.hpp"
#include <format>
#include <iomanip>
#include <ostream>
#include <stdexcept>

namespace micro_logger {
std::string bytes_to_hex(const uint8_t *bytes, size_t size) {
  std::ostringstream os;
  for (size_t i = 0; i < size; i++) {
    os << std::setfill('0') << std::setw(2) << std::hex
       << static_cast<unsigned int>(bytes[i]);
  }
  return os.str();
}

std::vector<uint8_t> hex_to_bytes(const std::string &hex) {
  if (hex.length() % 2) {
    throw std::invalid_argument("hex is missing one character please use bytes_to_hex function");
  }
  std::vector<uint8_t> out;
  out.reserve(hex.length() / 2);
  size_t idx;
  for (size_t i = 0; i < hex.length(); i += 2) {
    auto bytes_str = hex.substr(i, 2);
    out.emplace_back(std::stoul(bytes_str, &idx, 16));
    if(idx != 2) {
      throw std::invalid_argument(std::format("Failed to parse hex data [{}]", bytes_str));
    }
  }

  return out;
}
} // namespace micro_logger
