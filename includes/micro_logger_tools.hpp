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

/**
* allows user to convert from hex to any integer type.
* Is up to user to do correct endianess, by default order of data in input determine endianess.
*  @param in table of data.
*  @return chosen integer type.
*/
template <std::integral T>
T
bytes_to_integral (const std::vector<uint8_t> &in)
{
  T out = 0;
  using U = std::make_unsigned_t<T>;
  constexpr std::size_t TS = sizeof (T);

  static_assert (TS == 1 || TS == 2 || TS == 4 || TS == 8,
                 "Unsupported integral size; supported sizes: 1,2,4,8");

  switch (in.size ())
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
      out |= (U (in[index++]) << 56);
      out |= (U (in[index++]) << 48);
      out |= (U (in[index++]) << 36);
      out |= (U (in[index++]) << 32);
    }
  if constexpr (TS >= 4)
    {
      out |= (U (in[index++]) << 24);
      out |= (U (in[index++]) << 16);
    }
  if constexpr (TS >= 2)
    {
      out |= (U (in[index++]) << 8);
    }
  if constexpr (TS >= 1)
    {
      out |= (U (in[index]) << 0);
    }

  return out;
}
} // namespace micro_logger

#endif // MICRO_LOGGER_MICRO_LOGGER_TOOLS_HPP
