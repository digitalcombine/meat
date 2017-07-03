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
#include "compiler.h"

#include <algorithm>
#include <cstring>

bool meat::grinder::Utils::is_integer(const std::string &value,
					int32_t *res) {
  bool neg = false;
  unsigned int c = 0;

  // Check for numeric sign.
  if (value[c] == '-') {
    neg = true;
    c++;
  } else if (value[c] == '+') {
    c++;
  }

  // Get the numeric value.
  *res = 0;
  for (; c < value.length(); c++) {
    if (value[c] >= '0' and value[c] <= '9') {
      *res *= 10;
      *res += value[c] - '0';
    } else {
      return false;
    }
  }

  // Set the sign if we need to.
  if (neg)
    *res = -(*res);

  return true;
}

bool meat::grinder::Utils::is_float(const std::string &value, float_t *res) {
  const char *s_value = value.c_str();
  char *end_ptr;

  *res = strtod(s_value, &end_ptr);
  if (end_ptr < s_value + strlen(s_value)) {
    return false;
  }
  return true;
}
