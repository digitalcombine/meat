/*                                                                  -*- c++ -*-
 * Copyright (c) 2017 Ron R Wills <ron.rwsoft@gmail.com>
 *
 * This file is part of Meat.
 *
 * Meat is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Meat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Meat.  If not, see <http://www.gnu.org/licenses/>.
 */

/** @file */

#include <meat.h>
#include <meat/types.h>
#include <iostream>

#ifndef _UTILITIES_H
#define _UTILITIES_H

/** @ns */
namespace endian {

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define read_be(value) (value)
#define write_be(value) (value)
#else
  DECLSPEC meat::int32_t  read_be(meat::int32_t value);
  DECLSPEC meat::uint32_t read_be(meat::uint32_t value);
  DECLSPEC meat::int16_t  read_be(meat::int16_t value);
  DECLSPEC meat::uint16_t read_be(meat::uint16_t value);
  DECLSPEC meat::float_t  read_be(meat::float_t value);

  DECLSPEC meat::int32_t  write_be(meat::int32_t value);
  DECLSPEC meat::uint32_t write_be(meat::uint32_t value);
  DECLSPEC meat::int16_t  write_be(meat::int16_t value);
  DECLSPEC meat::uint16_t write_be(meat::uint16_t value);
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  inline int read_le(int value) { return value; }
#define write_le(value) (value)
#else
  DECLSPEC meat::int32_t  read_le(meat::int32_t value);
  DECLSPEC meat::uint32_t read_le(meat::uint32_t value);
  DECLSPEC meat::int16_t  read_le(meat::int16_t value);
  DECLSPEC meat::uint16_t read_le(meat::uint16_t value);
  DECLSPEC meat::float_t  read_le(meat::float_t value);

  DECLSPEC meat::int32_t  write_le(meat::int32_t value);
  DECLSPEC meat::uint32_t write_le(meat::uint32_t value);
  DECLSPEC meat::int16_t  write_le(meat::int16_t value);
  DECLSPEC meat::uint16_t write_le(meat::uint16_t value);
#endif

#if __FLOAT_WORD_ORDER__ == __ORDER_BIG_ENDIAN__
#else
  DECLSPEC meat::float_t write_be(meat::float_t value);
#endif

#if __FLOAT_WORD_ORDER__ == __ORDER_LITTLE_ENDIAN__
#else
  DECLSPEC meat::float_t write_le(meat::float_t value);
#endif

}

/** Creates an integer hash for any give string.
 * @param word The string to hash.
 * @param table_size The maximum size of the hash value.
 * @return Unsigned integer hash value.
 */
DECLSPEC meat::uint32_t hash(const std::string &word,
														 size_t table_size = 0x7fffffff);

/** Converts an hexidecimal integer value to a string.
 */
std::string itohex(unsigned int value, size_t width = 8);

#endif /* _UTILITIES_H */
