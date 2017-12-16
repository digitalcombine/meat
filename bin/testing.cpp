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
#include <meat/utilities.h>

#include <map>
#include <iostream>

std::map<std::string, unsigned int> test_results;

// misc.cpp
void misc_tests();

void meat::test::test(const char *id, bool show) {
  if (test_results.find(id) == test_results.end())
    test_results[id] = 0;
  /*if (show) {
    std::cout << "TEST: " << id << "...";
    }*/
}

void meat::test::passed(const char *id, bool show) {
  if (test_results.find(id) != test_results.end()) {
    /*if (show) {
      std::cout << " passed" << std::endl;
      }*/
  } else {

  }
}

void meat::test::failed(const char *id, bool show) {
  if (test_results.find(id) != test_results.end()) {
    test_results[id] = 1;
    /*if (show) {
      std::cout << " failed" << std::endl;
      } else {*/
    std::cout << "TEST: " << id << "... FAILED" << std::endl;

  } else {
    std::cerr << "TESTING: Unknown test " << id << " failed" << std::endl;
  }
}

void meat::test::summary() {
  unsigned int failures = 0;

  if (failures == 0) {
    /*  If there was not test failures, then we don't need to display the
     * summary unless debugging was enabled.
     */
#ifdef DEBUG
    std::cout << "TEST SUMMARY:" << std::endl;
    std::cout << "  100% of the tests passed." << std::endl;
#endif
  } else {
    /*  If any tests failed, then we always display the summary.
     */

    std::cout << "TEST SUMMARY:" << std::endl;

    std::map<std::string, unsigned int>::iterator it;
    for (it = test_results.begin(); it != test_results.end(); it++) {
      if (it->second > 0) {
        std::cout << "  " << it->first << "... FAILED" << std::endl;
        failures++;
      }
    }

    std::cout << "  " << std::dec << failures << " of the tests failed."
              << std::endl;
  }
}

void meat::test::run_tests() {
  misc_tests();
}
