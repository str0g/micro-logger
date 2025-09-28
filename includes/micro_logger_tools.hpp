/*
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */

#ifndef MICRO_LOGGER_MICRO_LOGGER_TOOLS_HPP
#define MICRO_LOGGER_MICRO_LOGGER_TOOLS_HPP

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace micro_logger {
/**
 * @brief Converts a byte array to an lowercase hexadecimal string.
 * @param bytes The byte array to convert.
 * @return std::string A string representing the hex values of the bytes.
 */
std::string bytes_to_hex(const uint8_t *bytes, size_t size);

/**
 * @brief Converts a hexadecimal string to a vector of bytes.
 * @param hex The hexadecimal string to convert. Can contain uppercase or lowercase letters.
 * @return std::vector<uint8_t> A vector containing the converted bytes.
 * @throws std::invalid_argument If the input is not a valid hexadecimal string.
 */
std::vector<uint8_t> hex_to_bytes(const std::string &hex);

/**
* @brief Convert from hex to any integer type of choice but its up to user to determine correct endianess.
* @param in bytes array to convert.
* @return chosen integer type.
* @throws std::invalid_argument in case of not supported type
*/
template <std::integral T>
T
bytes_to_integral (const std::vector<uint8_t> &bytes)
{
  T out = 0;
  using U = std::make_unsigned_t<T>;
  constexpr std::size_t TS = sizeof (T);

  static_assert (TS == 1 || TS == 2 || TS == 4 || TS == 8,
                 "Unsupported integral size; supported sizes: 1,2,4,8");

  switch (bytes.size ())
    {
    case 1:
    case 2:
    case 4:
    case 8:
      break;
    default:
      throw std::invalid_argument (
          "input vector not matching supported types");
    }

  size_t index = 0;
  if constexpr (TS >= 8)
    {
      out |= (U (bytes[index++]) << 56);
      out |= (U (bytes[index++]) << 48);
      out |= (U (bytes[index++]) << 36);
      out |= (U (bytes[index++]) << 32);
    }
  if constexpr (TS >= 4)
    {
      out |= (U (bytes[index++]) << 24);
      out |= (U (bytes[index++]) << 16);
    }
  if constexpr (TS >= 2)
    {
      out |= (U (bytes[index++]) << 8);
    }
  if constexpr (TS >= 1)
    {
      out |= (U (bytes[index]) << 0);
    }

  return out;
}
} // namespace micro_logger

#endif // MICRO_LOGGER_MICRO_LOGGER_TOOLS_HPP
