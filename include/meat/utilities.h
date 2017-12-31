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
#include <meat/numeric.h>
#include <iostream>

#ifndef _UTILITIES_H
#define _UTILITIES_H

/** @ns */
namespace endian {

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define read_be(value) (value)
#define write_be(value) (value)
#else
  DECLSPEC std::int64_t  read_be(std::int64_t value);
  DECLSPEC std::uint64_t read_be(std::uint64_t value);
  DECLSPEC std::int32_t  read_be(std::int32_t value);
  DECLSPEC std::uint32_t read_be(std::uint32_t value);
  DECLSPEC std::int16_t  read_be(std::int16_t value);
  DECLSPEC std::uint16_t read_be(std::uint16_t value);

  DECLSPEC std::int64_t  write_be(std::int64_t value);
  DECLSPEC std::uint64_t write_be(std::uint64_t value);
  DECLSPEC std::int32_t  write_be(std::int32_t value);
  DECLSPEC std::uint32_t write_be(std::uint32_t value);
  DECLSPEC std::int16_t  write_be(std::int16_t value);
  DECLSPEC std::uint16_t write_be(std::uint16_t value);
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  inline int read_le(int value) { return value; }
#define write_le(value) (value)
#else
  DECLSPEC std::int64_t  read_le(std::int64_t value);
  DECLSPEC std::uint64_t read_le(std::uint64_t value);
  DECLSPEC std::int32_t  read_le(std::int32_t value);
  DECLSPEC std::uint32_t read_le(std::uint32_t value);
  DECLSPEC std::int16_t  read_le(std::int16_t value);
  DECLSPEC std::uint16_t read_le(std::uint16_t value);

  DECLSPEC std::int64_t  write_le(std::int64_t value);
  DECLSPEC std::uint64_t write_le(std::uint64_t value);
  DECLSPEC std::int32_t  write_le(std::int32_t value);
  DECLSPEC std::uint32_t write_le(std::uint32_t value);
  DECLSPEC std::int16_t  write_le(std::int16_t value);
  DECLSPEC std::uint16_t write_le(std::uint16_t value);
#endif
}

/** Creates an integer hash for any give string.
 * @param word The string to hash.
 * @param table_size The maximum size of the hash value.
 * @return Unsigned integer hash value.
 */
DECLSPEC std::uint32_t hash(const std::string &word,
                            size_t table_size = 0x7fffffff);

/** Converts an hexidecimal integer value to a string.
 */
std::string itohex(unsigned int value, size_t width = 8);

namespace utf8 {

  class Iterator : public std::iterator<std::bidirectional_iterator_tag,
                                        std::string> {
  public:
    Iterator(const std::string &value);
    Iterator(const Iterator &other);

    Iterator &operator ++();
    Iterator operator ++(int);
    Iterator &operator --();
    Iterator operator --(int);

    bool operator ==(const Iterator &other) const;
    bool operator !=(const Iterator &other) const;

    const std::string &operator *() const;

    Iterator end() const;
    size_t position() const;
    size_t length() const;

    friend std::string substr(const Iterator &start,
                              const Iterator &end);

    friend std::string replace(const Iterator &start,
                               const Iterator &end,
                               const std::string &value);
  private:
    const std::string &_value;

    size_t _index, _length;
    std::string _current;
  };

  std::string substr(const Iterator &start, const Iterator &end);

  /** Returns a new std::string with the text from start to end replaced
   * with the given value.
   */
  std::string replace(const Iterator &start, const Iterator &end,
                      const std::string &value);
}

#endif /* _UTILITIES_H */
