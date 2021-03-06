/*                                                                  -*- c++ -*-
 * Very basic testing frame work from the Meat Language.
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
/** @file */

#ifndef MEAT_TESTING
#define MEAT_TESTING

/** @ns */
namespace meat {

	/** @ns */
  namespace test {

    void test(const char *id, bool show = true);

    void passed(const char *id, bool show = true);
    void failed(const char *id, bool show = true);

    void summary();
  }
}

#endif /* MEAT_TESTING */
