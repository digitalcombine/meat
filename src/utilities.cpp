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

#include <meat/utilities.h>
#include <cstring>
//#ifdef DEBUG
#include <iostream>
//#endif
#include <sstream>
#include <iomanip>

using namespace meat;

/**********
 * swap64 *
 **********/

static inline std::uint64_t swap64(std::uint64_t value) {
  std::uint64_t result = value;

  result = (result & 0x00000000ffffffff) << 32 |
    (result & 0xffffffff00000000) >> 32;
  result = (result & 0x0000ffff0000ffff) << 16 |
    (result & 0xffff0000ffff0000) >> 16;
  result = (result & 0x00ff00ff00ff00ff) << 8  |
    (result & 0xff00ff00ff00ff00) >> 8;

  return result;
}

static inline std::uint32_t swap32(std::uint32_t value) {
  return ((value >> 24) & 0x000000ff) | ((value >> 8) & 0x0000ff00) |
    ((value << 24) & 0xff000000) | ((value << 8) & 0x00ff0000);
}

static inline std::uint16_t swap16(std::uint16_t value) {
  return ((value & 0x00ff) << 8) | ((value & 0xff00) >> 8);
}

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

std::int64_t endian::read_be(std::int64_t value) {
  return swap64(value);
}

std::uint64_t endian::read_be(std::uint64_t value) {
  return swap64(value);
}

std::int32_t endian::read_be(std::int32_t value) {
  return swap32(value);
}

std::uint32_t endian::read_be(std::uint32_t value) {
  return swap32(value);
}

std::int16_t endian::read_be(std::int16_t value) {
  return swap16(value);
}

std::uint16_t endian::read_be(std::uint16_t value) {
  return swap16(value);
}

std::int64_t endian::write_be(std::int64_t value) {
  return swap64(value);
}

std::uint64_t endian::write_be(std::uint64_t value) {
  return swap64(value);
}

std::int32_t endian::write_be(std::int32_t value) {
  return swap32(value);
}

std::uint32_t endian::write_be(std::uint32_t value) {
  return swap32(value);
}

std::int16_t endian::write_be(std::int16_t value) {
  return swap16(value);
}

std::uint16_t endian::write_be(std::uint16_t value) {
  return swap16(value);
}

#endif /* LITTLE_ENDIAN */

#if __FLOAT_WORD_ORDER__ == __ORDER_BIG_ENDIAN__

int32_t endian::read_le(int32_t value) {
  return swap32(value);
}

uint32_t endian::read_le(uint32_t value) {
  return swap32(value);
}

int16_t endian::read_le(int16_t value) {
  return swap16(value);
}

uint16_t endian::read_le(uint16_t value) {
  return swap16(value);
}

int32_t endian::write_le(int32_t value) {
  return swap32(value);
}

uint32_t endian::write_le(uint32_t value) {
  return swap32(value);
}

int16_t endian::write_le(int16_t value) {
  return swap16(value);
}

uint16_t endian::write_le(uint16_t value) {
  return swap16(value);
}

#endif

#if __FLOAT_WORD_ORDER__ == __ORDER_BIG_ENDIAN__

float_t endian::read_le(float_t value) {
  conv_t v;
  v.f = value;
  v.i = swap32(v.i);
  return v.f;
}

float_t endian::write_le(float_t value) {
  conv_t v;
  v.f = value;
  v.i = swap32(v.i);
  return v.f;
}
#endif

/********
 * hash *
 ********/

uint32_t hash(const std::string &word, size_t table_size) {
  uint32_t seed = 31;
  uint64_t hvalue = 0;
  size_t len = word.size();

  /*  We add all the characters together with a seed. The seed ensures that
   * ab != ba.
   */
  for(register size_t i = 0; i < len; i++) {
    hvalue = (hvalue * seed) + word[i];
  }

  /*  Here we mod the hash value with the size of the hash table. This is
   * equivalent to inserting the new hash value into the table.
   */
  return (hvalue % table_size);
}

/**********
 * itohex *
 **********/

std::string itohex(unsigned int value, size_t width) {
  std::ostringstream convert;
  convert << std::setw(width) << std::setfill('0') << std::setbase(16);
  convert << value;
  return ("0x" + convert.str());
}

/******************************************************************************
 * utf8::Iterator Class
 */

utf8::Iterator::Iterator(const std::string &value)
  : _value(value), _index(0) {
  _length = 1;

  // Find the length of the character and make a copy for dereferencing.
  for (; (_value[_index + _length] & 0xc0) == 0x80; ++_length);
  _current = _value.substr(_index, _length);
}

utf8::Iterator::Iterator(const Iterator &other)
  : _value(other._value), _index(other._index), _length(other._length),
    _current(other._current) {
}

/*******************************
 * utf8::Iterator::operator ++ *
 *******************************/

utf8::Iterator &utf8::Iterator::operator ++() {
  // Increment the index.
  if (_index < _value.length()) _index += _length;

  // Get the length of the character and copy the character.
  if (_index < _value.length()) {
    for (; (_value[_index + _length] & 0xc0) == 0x80; ++_length);
    _current = _value.substr(_index, _length);
  } else {
    _current.clear();
  }

  return *this;
}

utf8::Iterator utf8::Iterator::operator ++(int) {
  Iterator tmp(*this);

  // Increment the index.
  if (_index < _value.length()) _index += _length;

  // Get the length of the character and copy it.
  if (_index < _value.length()) {
    for (; (_value[_index + _length] & 0xc0) == 0x80; ++_length);
    _current = _value.substr(_index, _length);
  } else {
    _current.clear();
  }

  return tmp;
}

/*******************************
 * utf8::Iterator::operator -- *
 *******************************/

utf8::Iterator &utf8::Iterator::operator --() {
  if (_index > 0) {
    size_t tmp = _index;

    // Find the first character.
    for (--_index; (_value[_index] & 0xc0) == 0x80 and _index > 0; --_index);

    // Get the length of the character and copy it.
    _length = tmp - _index;
    _current = _value.substr(_index, _length);
  } else {
    _current.clear();
  }

  return *this;
}

utf8::Iterator utf8::Iterator::operator --(int) {
  Iterator tmp(*this);

  if (_index > 0) {
    size_t tmp = _index;

    // Find the first character.
    for (--_index; (_value[_index] & 0xc0) == 0x80 and _index > 0; --_index);

    // Get the length of the character and copy it.
    _length = tmp - _index;
    _current = _value.substr(_index, _length);
  } else {
    _current.clear();
  }

  return tmp;
}

/*******************************
 * utf8::Iterator::operator == *
 *******************************/

bool utf8::Iterator::operator ==(const Iterator &other) const {
  return _index == other._index;
}

/*******************************
 * utf8::Iterator::operator != *
 *******************************/

bool utf8::Iterator::operator !=(const Iterator &other) const {
  return _index != other._index;
}

/******************************
 * utf8::Iterator::operator * *
 ******************************/

const std::string &utf8::Iterator::operator *() const {
  return _current;
}

utf8::Iterator utf8::Iterator::end() const {
  Iterator tmp(*this);
  tmp._index = _value.length();
  return tmp;
}

size_t utf8::Iterator::position() const {
  return _index;
}

size_t utf8::Iterator::length() const {
  return _length;
}

/****************
 * utf8::substr *
 ****************/

std::string utf8::substr(const Iterator &start, const Iterator &end) {
  const std::string &value = start._value;
  if (start != start.end() and end._index >= start._index) {
    return value.substr(start._index, end._index + end._length);
  }
  throw std::range_error("sub string iterators out of range");
}

/*****************
 * utf8::replace *
 *****************/

std::string utf8::replace(const Iterator &start, const Iterator &end,
                          const std::string &value) {
  std::string new_value = start._value;
  if (start != start.end() and end._index >= start._index) {
    return new_value.replace(start._index,
                             (end._index + end._length) - start._index,
                             value);
  }
  throw std::range_error("sub string iterators out of range");
}
