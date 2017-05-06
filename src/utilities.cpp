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
#ifdef DEBUG
#include <iostream>
#endif

using namespace meat;

typedef union {
  int32_t i;
  float_t f;
} conv_t;

static inline uint32_t swap32(uint32_t value) {
  return (value << 24) | ((value & 0xff00) << 8) |
    ((value & 0xff0000) >> 8) | (value >> 24);
}

static inline uint16_t swap16(uint16_t value) {
  return ((value & 0xff) << 8) | ((value & 0xff00) >> 8);
}

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

int32_t endian::read_be(int32_t value) {
  return swap32(value);
}

uint32_t endian::read_be(uint32_t value) {
  return swap32(value);
}

int16_t endian::read_be(int16_t value) {
  return swap16(value);
}

uint16_t endian::read_be(uint16_t value) {
  return swap16(value);
}

int32_t endian::write_be(int32_t value) {
  return swap32(value);
}

uint32_t endian::write_be(uint32_t value) {
  return swap32(value);
}

int16_t endian::write_be(int16_t value) {
  return swap16(value);
}

uint16_t endian::write_be(uint16_t value) {
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

#if __FLOAT_WORD_ORDER__ == __ORDER_LITTLE_ENDIAN__

float_t endian::read_be(float_t value) {
  conv_t v;
  v.f = value;
  v.i = swap32(v.i);
  return v.f;
}

float_t endian::write_be(float_t value) {
  conv_t v;
  v.f = value;
  v.i = swap32(v.i);
  return v.f;
}

#endif

uint32_t hash(const char *word, size_t table_size) {
  uint32_t seed = 31;
  uint64_t hvalue = 0;
  size_t len = std::strlen(word);

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
