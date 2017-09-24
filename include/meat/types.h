/*                                                                  -*- c++ -*-
 * Fixed Size Base Types.
 *
 *  Unfortunately in C/C++ the sizes of various type change from platform to
 * platform. The size differences is for performance reasons, but it makes
 * defining data in a cross-platform way a real pain. So here we use some of
 * the features of gcc to create types of guaranteed sizes.
 *
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

#include <string>

#ifndef MEAT_TYPES_H
#define MEAT_TYPES_H

namespace meat {

  /* Macros for creating fixed size integer types. */
#undef __intN_t
#define __intN_t(N, MODE) \
  typedef int int##N##_t __attribute__ ((__mode__(MODE)))
#undef __uintN_t
#define __uintN_t(N, MODE) \
  typedef unsigned int uint##N##_t __attribute__ ((__mode__(MODE)))

  /* Signed Integer Types. */
  __intN_t(8, __QI__);  // int8_t
  __intN_t(16, __HI__); // int16_t
  __intN_t(32, __SI__); // int32_t
  __intN_t(64, __DI__); // int64_t

  /* Unsigned Integer Types. */
  __uintN_t(8, __QI__);  // uint8_t
  __uintN_t(16, __HI__); // uint16_t
  __uintN_t(32, __SI__); // uint32_t
  __uintN_t(64, __DI__); // uint64_t

  /* Fixed size floating point types. */
  typedef float float_t __attribute__((__mode__(__SF__)));   // 32 bits
  typedef double double_t __attribute__((__mode__(__DF__))); // 64 bits
}

#endif /* MEAT_TYPES_H */
