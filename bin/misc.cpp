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

#include <testing.h>
#include <gitgo/utilities.h>

using namespace Gitgo;

static const uint32_t u32_value = 0x12345678;
static const uint16_t u16_value = 0x1234;

/** Test the endian functions.
 */
static void endian_tests() {

#ifdef IS_LITTLE_ENDIAN
  Gitgo::Test::test("Endian unsigned 32bit write");
  uint32_t u32_result = Endian::write_be(u32_value);
  if (u32_result != 0x78563412) {
    Gitgo::Test::failed("Endian unsigned 32bit write");
  } else {
    Gitgo::Test::passed("Endian unsigned 32bit write");
  }

  Gitgo::Test::test("Endian unsigned 32bit read");
  u32_result = Endian::read_be(u32_result);
  if (u32_result != u32_value) {
    Gitgo::Test::failed("Endian unsigned 32bit read");
  } else {
    Gitgo::Test::passed("Endian unsigned 32bit read");
  }

  Gitgo::Test::test("Endian signed 32bit write");
  int32_t s32_result = Endian::write_be((int32_t)u32_value);
  if ((uint32_t)s32_result != 0x78563412) {
    Gitgo::Test::failed("Endian signed 32bit write");
  } else {
    Gitgo::Test::passed("Endian signed 32bit write");
  }

  Gitgo::Test::test("Endian signed 32bit read");
  s32_result = Endian::read_be(s32_result);
  if ((uint32_t)s32_result != u32_value) {
    Gitgo::Test::failed("Endian signed 32bit read");
  } else {
    Gitgo::Test::passed("Endian signed 32bit read");
  }

  Gitgo::Test::test("Endian unsigned 16bit write");
  uint16_t u16_result = Endian::write_be(u16_value);
  if (u16_result != 0x3412) {
    Gitgo::Test::failed("Endian unsigned 16bit write");
  } else {
    Gitgo::Test::passed("Endian unsigned 16bit write");
  }

  Gitgo::Test::test("Endian unsigned 16bit read");
  u16_result = Endian::read_be(u16_result);
  if (u16_result != u16_value) {
    Gitgo::Test::failed("Endian unsigned 16bit read");
  } else {
    Gitgo::Test::passed("Endian unsigned 16bit read");
  }

  Gitgo::Test::test("Endian signed 16bit write");
  int16_t s16_result = Endian::write_be((int16_t)u16_value);
  if ((int16_t)s16_result != 0x3412) {
    Gitgo::Test::failed("Endian signed 16bit write");
  } else {
    Gitgo::Test::passed("Endian signed 16bit write");
  }

  Gitgo::Test::test("Endian signed 16bit read");
  s16_result = Endian::read_be(s16_result);
  if ((uint16_t)s16_result != u16_value) {
    Gitgo::Test::failed("Endian signed 16bit read");
  } else {
    Gitgo::Test::passed("Endian signed 16bit read");
  }

  // XXX Test float point endian read/write here!

#else /* IS_BIG_ENDIAN */
#endif /* IS_LITTLE_ENDIAN */
}

void misc_tests() {
  endian_tests();
}
