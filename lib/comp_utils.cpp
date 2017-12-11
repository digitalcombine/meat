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

static unsigned int is_numeric(const char *value) {
  switch (value[0]) {
  case '\xe0':
    // Devavagari Digits U+0966 - U+096F
    if (std::strcmp(value, "\xe0\xa5\xa6") >= 0 and
        std::strcmp(value, "\xe0\xa5\xaf") <= 0)
      return 3;

    // Bengali Digits U+09E6 - U+09EF
    if (std::strcmp(value, "\xe0\xa7\xa6") >= 0 and
        std::strcmp(value, "\xe0\xa7\xaf") <= 0)
      return 3;

    // Gurmukhi Digits U+0A66 - U+0A6F
    if (std::strcmp(value, "\xe0\xa9\xa6") >= 0 and
        std::strcmp(value, "\xe0\xa9\xaf") <= 0)
      return 3;

    // Gujarati Digits U+0AE6 - U+0AEF
    if (std::strcmp(value, "\xe0\xab\xa6") >= 0 and
        std::strcmp(value, "\xe0\xab\xaf") <= 0)
      return 3;

    // Oriya Digits U+0B66 - U+0B6F
    if (std::strcmp(value, "\xe0\xad\xa6") >= 0 and
        std::strcmp(value, "\xe0\xad\xaf") <= 0)
      return 3;

    // Tamil Digits U+0BE6 - U+0BEF
    if (std::strcmp(value, "\xe0\xaf\xa6") >= 0 and
        std::strcmp(value, "\xe0\xaf\xaf") <= 0)
      return 3;

    // Telugu Digits U+0C66 - U+0C6F
    if (std::strcmp(value, "\xe0\xb1\xa6") >= 0 and
        std::strcmp(value, "\xe0\xb1\xaf") <= 0)
      return 3;

    // Kannada Digits U+0CE6 - U+0CEF
    if (std::strcmp(value, "\xe0\xb3\xa6") >= 0 and
        std::strcmp(value, "\xe0\xb3\xaf") <= 0)
      return 3;

    // Malayalam Digits U+0D66 - U+0D6F
    if (std::strcmp(value, "\xe0\xb5\xa6") >= 0 and
        std::strcmp(value, "\xe0\xb5\xaf") <= 0)
      return 3;

    // Sinhala Digits U+0DE6 - U+0DEF
    if (std::strcmp(value, "\xe0\xb7\xa6") >= 0 and
        std::strcmp(value, "\xe0\xb7\xaf") <= 0)
      return 3;

    // Thai Digits U+0E50 - U+0E59
    if (std::strcmp(value, "\xe0\xb9\x90") >= 0 and
        std::strcmp(value, "\xe0\xb9\x99") <= 0)
      return 3;

    // Lao Digits U+0ED0 - U+0ED9
    if (std::strcmp(value, "\xe0\xbb\x90") >= 0 and
        std::strcmp(value, "\xe0\xbb\x99") <= 0)
      return 3;

    // Tibetan Digits U+0F20 - U+0F29
    if (std::strcmp(value, "\xe0\xbc\xa0") >= 0 and
        std::strcmp(value, "\xe0\xbc\xa9") <= 0)
      return 3;

    break;

  case '\xe1':
    // Myanmar Digits U+1040 - U+1049
    if (std::strcmp(value, "\xe1\x81\x80") >= 0 and
        std::strcmp(value, "\xe1\x81\x89") <= 0)
      return 3;

    // Myanmar Shan Digits U+1090 - U+1099
    if (std::strcmp(value, "\xe1\x82\x90") >= 0 and
        std::strcmp(value, "\xe1\x82\x99") <= 0)
      return 3;

    // Khmer Digits U+17E0 - U+17E9
    if (std::strcmp(value, "\xe1\x9f\xa0") >= 0 and
        std::strcmp(value, "\xe1\x9f\xa9") <= 0)
      return 3;

    // Mongolian Digits U+1810 - U+1819
    if (std::strcmp(value, "\xe1\xa0\x90") >= 0 and
        std::strcmp(value, "\xe1\xa0\x99") <= 0)
      return 3;

    // Limbu Digits U+1946 - U+194F
    if (std::strcmp(value, "\xe1\xa5\x86") >= 0 and
        std::strcmp(value, "\xe1\xa5\x8f") <= 0)
      return 3;

    // New Tai Lue Digits U+19D0 - U+19D9
    if (std::strcmp(value, "\xe1\xa7\x90") >= 0 and
        std::strcmp(value, "\xe1\xa7\x99") <= 0)
      return 3;

    // Tai Tham Hora Digits U+1A80 - U+1A89
    if (std::strcmp(value, "\xe1\xaa\x80") >= 0 and
        std::strcmp(value, "\xe1\xaa\x89") <= 0)
      return 3;

    // Tai Tham Tham Digits U+1A90 - U+1A99
    if (std::strcmp(value, "\xe1\xaa\x90") >= 0 and
        std::strcmp(value, "\xe1\xaa\x99") <= 0)
      return 3;

    // Balinese Digits U+1B50 - U+1B59
    if (std::strcmp(value, "\xe1\xad\x90") >= 0 and
        std::strcmp(value, "\xe1\xad\x99") <= 0)
      return 3;

    // Sundanese Digits U+1BB0 - U+1BB9
    if (std::strcmp(value, "\xe1\xae\xb0") >= 0 and
        std::strcmp(value, "\xe1\xae\xb9") <= 0)
      return 3;

    // Lepcha Digits U+1C40 - U+1C49
    if (std::strcmp(value, "\xe1\xb1\x80") >= 0 and
        std::strcmp(value, "\xe1\xb1\x89") <= 0)
      return 3;

    // Ol Chiki Digits U+1C50 - U+1C59
    if (std::strcmp(value, "\xe1\xb1\x90") >= 0 and
        std::strcmp(value, "\xe1\xb1\x99") <= 0)
      return 3;

    break;

  case '\xea':
    // Vai Digits U+A620 - U+A629
    if (std::strcmp(value, "\xea\x98\xa0") >= 0 and
        std::strcmp(value, "\xea\x98\xa9") <= 0)
      return 3;

    // Saurashtra Digits U+A8D0 - U+A8D9
    if (std::strcmp(value, "\xea\xa3\x90") >= 0 and
        std::strcmp(value, "\xea\xa3\x99") <= 0)
      return 3;

    // Kayah Li Digits U+A900 - U+A909
    if (std::strcmp(value, "\xea\xa4\x80") >= 0 and
        std::strcmp(value, "\xea\xa4\x89") <= 0)
      return 3;

    // Javanese Digits U+A9D0 - U+A9D9
    if (std::strcmp(value, "\xea\xa7\x90") >= 0 and
        std::strcmp(value, "\xea\xa7\x99") <= 0)
      return 3;

    // Myanmar Tai Laing Digits U+A9F0 - U+A9F9
    if (std::strcmp(value, "\xea\xa7\xb0") >= 0 and
        std::strcmp(value, "\xea\xa7\xb9") <= 0)
      return 3;

    // Cham Digits U+AA50 - U+AA59
    if (std::strcmp(value, "\xea\xa9\x90") >= 0 and
        std::strcmp(value, "\xea\xa9\x99") <= 0)
      return 3;

    // Meetei Mayek Digits U+ABF0 - U+ABF9
    if (std::strcmp(value, "\xea\xaf\xb0") >= 0 and
        std::strcmp(value, "\xea\xaf\xb9") <= 0)
      return 3;

    break;

  case '\xf0':
    // Osmanya Digits U+104A0 - U+104A9
    if (std::strcmp(value, "\xf0\x90\x92\xa0") >= 0 and
        std::strcmp(value, "\xf0\x90\x92\xa9") <= 0)
      return 4;

    // Brahmi Digits U+11066 - U+1106F
    if (std::strcmp(value, "\xf0\x91\x81\xa6") >= 0 and
        std::strcmp(value, "\xf0\x91\x81\xaf") <= 0)
      return 4;

    // Sora Sompeng Digits U+110F0 - U+110F9
    if (std::strcmp(value, "\xf0\x91\x83\xb0") >= 0 and
        std::strcmp(value, "\xf0\x91\x83\xb9") <= 0)
      return 4;

    // Chakma Digits U+11136 - U+1113F
    if (std::strcmp(value, "\xf0\x91\x84\xb6") >= 0 and
        std::strcmp(value, "\xf0\x91\x84\xbf") <= 0)
      return 4;

    // Sharada Digits U+111D0 - U+111D9
    if (std::strcmp(value, "\xf0\x91\x87\x90") >= 0 and
        std::strcmp(value, "\xf0\x91\x87\x99") <= 0)
      return 4;

    // Khudawadi Digits U+112F0 - U+112F9
    if (std::strcmp(value, "\xf0\x91\x8b\xb0") >= 0 and
        std::strcmp(value, "\xf0\x91\x8b\xb9") <= 0)
      return 4;

    // Newa Digits U+11450 - U+11459
    if (std::strcmp(value, "\xf0\x91\x91\x90") >= 0 and
        std::strcmp(value, "\xf0\x91\x91\x99") <= 0)
      return 4;

    // Tirhuta Digits U+114D0 - U+114D9
    if (std::strcmp(value, "\xf0\x91\x93\x90") >= 0 and
        std::strcmp(value, "\xf0\x91\x93\x99") <= 0)
      return 4;

    // Modi Digits U+11650 - U+11659
    if (std::strcmp(value, "\xf0\x91\x99\x90") >= 0 and
        std::strcmp(value, "\xf0\x91\x99\x99") <= 0)
      return 4;

    // Takri Digits U+116C0 - U+116C9
    if (std::strcmp(value, "\xf0\x91\x9b\x80") >= 0 and
        std::strcmp(value, "\xf0\x91\x9b\x89") <= 0)
      return 4;

    // Ahom Digits U+11730 - U+11739
    if (std::strcmp(value, "\xf0\x91\x9c\xb0") >= 0 and
        std::strcmp(value, "\xf0\x91\x9c\xb9") <= 0)
      return 4;

    // Warang Citi Digits U+118E0 - U+118E9
    if (std::strcmp(value, "\xf0\x91\xa3\xa0") >= 0 and
        std::strcmp(value, "\xf0\x91\xa3\xa9") <= 0)
      return 4;

    // Bhaiksuki Digits U+11C50 - U+11C59
    if (std::strcmp(value, "\xf0\x91\xb1\x90") >= 0 and
        std::strcmp(value, "\xf0\x91\xb1\x99") <= 0)
      return 4;

    // Masaram Digits U+11D50 - U+11D59
    if (std::strcmp(value, "\xf0\x91\xb5\x90") >= 0 and
        std::strcmp(value, "\xf0\x91\xb5\x99") <= 0)
      return 4;

    // Mro Digits U+16A60 - U+16A69
    if (std::strcmp(value, "\xf0\x96\xa9\xa0") >= 0 and
        std::strcmp(value, "\xf0\x96\xa9\xa9") <= 0)
      return 4;

    // Pahawh Digits U+16B50 - U+16B59
    if (std::strcmp(value, "\xf0\x96\xad\x90") >= 0 and
        std::strcmp(value, "\xf0\x96\xad\x99") <= 0)
      return 4;

    // Mathematical Bold Digits U+1D7CE - U+1D7D7
    // Mathematical Double-Struck Digits U+1D7D8 - U+1D7E1
    // Mathematical Sans-Serif Digits U+1D7E2 - U+1D7EB
    // Mathematical Sans-Serif Bold Digits U+1D7EC - U+1D7F5
    // Mathematical Monospace Digits U+1D7F6 - U+1D7FF
    if (std::strcmp(value, "\xf0\x9d\x9f\x8e") >= 0 and
        std::strcmp(value, "\xf0\x9d\x9f\xbf") <= 0)
      return 4;

    // Adlam Digits U+1E950 - U+1E959
    if (std::strcmp(value, "\xf0\x9e\xa5\x90") >= 0 and
        std::strcmp(value, "\xf0\x9e\xa5\x99") <= 0)
      return 4;

    break;

  default:
    // Digits U+0030 - U+0039
    if (*value >= '\x30' and *value <= '\x39')
      return 1;

    // Arabic-Indic Digits U+0660 - U+0669
    if (std::strcmp(value, "\xd9\xa0") >= 0 and
        std::strcmp(value, "\xd9\xa9") <= 0)
      return 2;

    // Extended Arabic-Indic Digits U+06F0-U+06F9
    if (std::strcmp(value, "\xdb\xb0") >= 0 and
        std::strcmp(value, "\xdb\xb9") <= 0)
      return 2;

    // NKo Digits U+07C0 - U+07C9
    if (std::strcmp(value, "\xdf\x80") >= 0 and
        std::strcmp(value, "\xdf\x89") <= 0)
      return 2;

    // Fullwidth Digits U+FF10 - U+FF19
    if (std::strcmp(value, "\xef\xbc\x90") >= 0 and
        std::strcmp(value, "\xef\xbc\x99") <= 0)
      return 3;

    break;
  }

  return 0;
}

static unsigned int to_numeric(const char *value) {
  switch (value[0]) {
  case '\xe0':
    // Devavagari Digits U+0966 - U+096F
    if (std::strcmp(value, "\xe0\xa5\xa6") >= 0 and
        std::strcmp(value, "\xe0\xa5\xaf") <= 0)
      return value[2] - '\xa6';

    // Bengali Digits U+09E6 - U+09EF
    if (std::strcmp(value, "\xe0\xa7\xa6") >= 0 and
        std::strcmp(value, "\xe0\xa7\xaf") <= 0)
      return value[2] - '\xa6';

    // Gurmukhi Digits U+0A66 - U+0A6F
    if (std::strcmp(value, "\xe0\xa9\xa6") >= 0 and
        std::strcmp(value, "\xe0\xa9\xaf") <= 0)
      return value[2] - '\xa6';

    // Gujarati Digits U+0AE6 - U+0AEF
    if (std::strcmp(value, "\xe0\xab\xa6") >= 0 and
        std::strcmp(value, "\xe0\xab\xaf") <= 0)
      return value[2] - '\xa6';

    // Oriya Digits U+0B66 - U+0B6F
    if (std::strcmp(value, "\xe0\xad\xa6") >= 0 and
        std::strcmp(value, "\xe0\xad\xaf") <= 0)
      return value[2] - '\xa6';

    // Tamil Digits U+0BE6 - U+0BEF
    if (std::strcmp(value, "\xe0\xaf\xa6") >= 0 and
        std::strcmp(value, "\xe0\xaf\xaf") <= 0)
      return value[2] - '\xa6';

    // Telugu Digits U+0C66 - U+0C6F
    if (std::strcmp(value, "\xe0\xb1\xa6") >= 0 and
        std::strcmp(value, "\xe0\xb1\xaf") <= 0)
      return value[2] - '\xa6';

    // Kannada Digits U+0CE6 - U+0CEF
    if (std::strcmp(value, "\xe0\xb3\xa6") >= 0 and
        std::strcmp(value, "\xe0\xb3\xaf") <= 0)
      return value[2] - '\xa6';

    // Malayalam Digits U+0D66 - U+0D6F
    if (std::strcmp(value, "\xe0\xb5\xa6") >= 0 and
        std::strcmp(value, "\xe0\xb5\xaf") <= 0)
      return value[2] - '\xa6';

    // Sinhala Digits U+0DE6 - U+0DEF
    if (std::strcmp(value, "\xe0\xb7\xa6") >= 0 and
        std::strcmp(value, "\xe0\xb7\xaf") <= 0)
      return value[2] - '\xa6';

    // Thai Digits U+0E50 - U+0E59
    if (std::strcmp(value, "\xe0\xb9\x90") >= 0 and
        std::strcmp(value, "\xe0\xb9\x99") <= 0)
      return value[2] - '\x90';

    // Lao Digits U+0ED0 - U+0ED9
    if (std::strcmp(value, "\xe0\xbb\x90") >= 0 and
        std::strcmp(value, "\xe0\xbb\x99") <= 0)
      return value[2] - '\x90';

    // Tibetan Digits U+0F20 - U+0F29
    if (std::strcmp(value, "\xe0\xbc\xa0") >= 0 and
        std::strcmp(value, "\xe0\xbc\xa9") <= 0)
      return value[2] - '\xa0';

    break;

  case '\xe1':
    // Myanmar Digits U+1040 - U+1049
    if (std::strcmp(value, "\xe1\x81\x80") >= 0 and
        std::strcmp(value, "\xe1\x81\x89") <= 0)
      return value[2] - '\x80';

    // Myanmar Shan Digits U+1090 - U+1099
    if (std::strcmp(value, "\xe1\x82\x90") >= 0 and
        std::strcmp(value, "\xe1\x82\x99") <= 0)
      return value[2] - '\x90';

    // Khmer Digits U+17E0 - U+17E9
    if (std::strcmp(value, "\xe1\x9f\xa0") >= 0 and
        std::strcmp(value, "\xe1\x9f\xa9") <= 0)
      return value[2] - '\xa0';

    // Mongolian Digits U+1810 - U+1819
    if (std::strcmp(value, "\xe1\xa0\x90") >= 0 and
        std::strcmp(value, "\xe1\xa0\x99") <= 0)
      return value[2] - '\x90';

    // Limbu Digits U+1946 - U+194F
    if (std::strcmp(value, "\xe1\xa5\x86") >= 0 and
        std::strcmp(value, "\xe1\xa5\x8f") <= 0)
      return value[2] - '\x86';

    // New Tai Lue Digits U+19D0 - U+19D9
    if (std::strcmp(value, "\xe1\xa7\x90") >= 0 and
        std::strcmp(value, "\xe1\xa7\x99") <= 0)
      return value[2] - '\x90';

    // Tai Tham Hora Digits U+1A80 - U+1A89
    if (std::strcmp(value, "\xe1\xaa\x80") >= 0 and
        std::strcmp(value, "\xe1\xaa\x89") <= 0)
      return value[2] - '\x80';

    // Tai Tham Tham Digits U+1A90 - U+1A99
    if (std::strcmp(value, "\xe1\xaa\x90") >= 0 and
        std::strcmp(value, "\xe1\xaa\x99") <= 0)
      return value[2] - '\x90';

    // Balinese Digits U+1B50 - U+1B59
    if (std::strcmp(value, "\xe1\xad\x90") >= 0 and
        std::strcmp(value, "\xe1\xad\x99") <= 0)
      return value[2] - '\x90';

    // Sundanese Digits U+1BB0 - U+1BB9
    if (std::strcmp(value, "\xe1\xae\xb0") >= 0 and
        std::strcmp(value, "\xe1\xae\xb9") <= 0)
      return value[2] - '\xb0';

    // Lepcha Digits U+1C40 - U+1C49
    if (std::strcmp(value, "\xe1\xb1\x80") >= 0 and
        std::strcmp(value, "\xe1\xb1\x89") <= 0)
      return value[2] - '\x80';

    // Ol Chiki Digits U+1C50 - U+1C59
    if (std::strcmp(value, "\xe1\xb1\x90") >= 0 and
        std::strcmp(value, "\xe1\xb1\x99") <= 0)
      return value[2] - '\x90';

    break;

  case '\xea':
    // Vai Digits U+A620 - U+A629
    if (std::strcmp(value, "\xea\x98\xa0") >= 0 and
        std::strcmp(value, "\xea\x98\xa9") <= 0)
      return value[2] - '\xa0';

    // Saurashtra Digits U+A8D0 - U+A8D9
    if (std::strcmp(value, "\xea\xa3\x90") >= 0 and
        std::strcmp(value, "\xea\xa3\x99") <= 0)
      return value[2] - '\x90';

    // Kayah Li Digits U+A900 - U+A909
    if (std::strcmp(value, "\xea\xa4\x80") >= 0 and
        std::strcmp(value, "\xea\xa4\x89") <= 0)
      return value[2] - '\x80';

    // Javanese Digits U+A9D0 - U+A9D9
    if (std::strcmp(value, "\xea\xa7\x90") >= 0 and
        std::strcmp(value, "\xea\xa7\x99") <= 0)
      return value[2] - '\x90';

    // Myanmar Tai Laing Digits U+A9F0 - U+A9F9
    if (std::strcmp(value, "\xea\xa7\xb0") >= 0 and
        std::strcmp(value, "\xea\xa7\xb9") <= 0)
      return value[2] - '\xb0';

    // Cham Digits U+AA50 - U+AA59
    if (std::strcmp(value, "\xea\xa9\x90") >= 0 and
        std::strcmp(value, "\xea\xa9\x99") <= 0)
      return value[2] - '\x90';

    // Meetei Mayek Digits U+ABF0 - U+ABF9
    if (std::strcmp(value, "\xea\xaf\xb0") >= 0 and
        std::strcmp(value, "\xea\xaf\xb9") <= 0)
      return value[2] - '\xb0';

    break;

  case '\xf0':
    // Osmanya Digits U+104A0 - U+104A9
    if (std::strcmp(value, "\xf0\x90\x92\xa0") >= 0 and
        std::strcmp(value, "\xf0\x90\x92\xa9") <= 0)
      return value[3] - '\xa0';

    // Brahmi Digits U+11066 - U+1106F
    if (std::strcmp(value, "\xf0\x91\x81\xa6") >= 0 and
        std::strcmp(value, "\xf0\x91\x81\xaf") <= 0)
      return value[3] - '\xa6';

    // Sora Sompeng Digits U+110F0 - U+110F9
    if (std::strcmp(value, "\xf0\x91\x83\xb0") >= 0 and
        std::strcmp(value, "\xf0\x91\x83\xb9") <= 0)
      return value[3] - '\xb0';

    // Chakma Digits U+11136 - U+1113F
    if (std::strcmp(value, "\xf0\x91\x84\xb6") >= 0 and
        std::strcmp(value, "\xf0\x91\x84\xbf") <= 0)
      return value[3] - '\xb6';

    // Sharada Digits U+111D0 - U+111D9
    if (std::strcmp(value, "\xf0\x91\x87\x90") >= 0 and
        std::strcmp(value, "\xf0\x91\x87\x99") <= 0)
      return value[3] - '\x90';

    // Khudawadi Digits U+112F0 - U+112F9
    if (std::strcmp(value, "\xf0\x91\x8b\xb0") >= 0 and
        std::strcmp(value, "\xf0\x91\x8b\xb9") <= 0)
      return value[3] - '\xb0';

    // Newa Digits U+11450 - U+11459
    if (std::strcmp(value, "\xf0\x91\x91\x90") >= 0 and
        std::strcmp(value, "\xf0\x91\x91\x99") <= 0)
      return value[3] - '\x90';

    // Tirhuta Digits U+114D0 - U+114D9
    if (std::strcmp(value, "\xf0\x91\x93\x90") >= 0 and
        std::strcmp(value, "\xf0\x91\x93\x99") <= 0)
      return value[3] - '\x90';

    // Modi Digits U+11650 - U+11659
    if (std::strcmp(value, "\xf0\x91\x99\x90") >= 0 and
        std::strcmp(value, "\xf0\x91\x99\x99") <= 0)
      return value[3] - '\x90';

    // Takri Digits U+116C0 - U+116C9
    if (std::strcmp(value, "\xf0\x91\x9b\x80") >= 0 and
        std::strcmp(value, "\xf0\x91\x9b\x89") <= 0)
      return value[3] - '\x80';

    // Ahom Digits U+11730 - U+11739
    if (std::strcmp(value, "\xf0\x91\x9c\xb0") >= 0 and
        std::strcmp(value, "\xf0\x91\x9c\xb9") <= 0)
      return value[3] - '\xb0';

    // Warang Citi Digits U+118E0 - U+118E9
    if (std::strcmp(value, "\xf0\x91\xa3\xa0") >= 0 and
        std::strcmp(value, "\xf0\x91\xa3\xa9") <= 0)
      return value[3] - '\xa0';

    // Bhaiksuki Digits U+11C50 - U+11C59
    if (std::strcmp(value, "\xf0\x91\xb1\x90") >= 0 and
        std::strcmp(value, "\xf0\x91\xb1\x99") <= 0)
      return value[3] - '\x90';

    // Masaram Digits U+11D50 - U+11D59
    if (std::strcmp(value, "\xf0\x91\xb5\x90") >= 0 and
        std::strcmp(value, "\xf0\x91\xb5\x99") <= 0)
      return value[3] - '\x90';

    // Mro Digits U+16A60 - U+16A69
    if (std::strcmp(value, "\xf0\x96\xa9\xa0") >= 0 and
        std::strcmp(value, "\xf0\x96\xa9\xa9") <= 0)
      return value[3] - '\xa0';

    // Pahawh Digits U+16B50 - U+16B59
    if (std::strcmp(value, "\xf0\x96\xad\x90") >= 0 and
        std::strcmp(value, "\xf0\x96\xad\x99") <= 0)
      return value[3] - '\x90';

    // Mathematical Bold Digits U+1D7CE - U+1D7D7
    // Mathematical Double-Struck Digits U+1D7D8 - U+1D7E1
    // Mathematical Sans-Serif Digits U+1D7E2 - U+1D7EB
    // Mathematical Sans-Serif Bold Digits U+1D7EC - U+1D7F5
    // Mathematical Monospace Digits U+1D7F6 - U+1D7FF
    if (std::strcmp(value, "\xf0\x9d\x9f\x8e") >= 0 and
        std::strcmp(value, "\xf0\x9d\x9f\xbf") <= 0)
      return (value[3] - '\x8e') % 10;

    // Adlam Digits U+1E950 - U+1E959
    if (std::strcmp(value, "\xf0\x9e\xa5\x90") >= 0 and
        std::strcmp(value, "\xf0\x9e\xa5\x99") <= 0)
      return value[3] - '\x90';

    break;

  default:
    // Digits U+0030 - U+0039
    if (value[0] >= '\x30' and value[0] <= '\x39')
      return value[0] - '\x30';

    // Arabic-Indic Digits U+0660 - U+0669
    if (std::strcmp(value, "\xd9\xa0") >= 0 and
        std::strcmp(value, "\xd9\xa9") <= 0)
      return value[1] - '\xa0';

    // Extended Arabic-Indic Digits U+06F0-U+06F9
    if (std::strcmp(value, "\xdb\xb0") >= 0 and
        std::strcmp(value, "\xdb\xb9") <= 0)
      return value[1] - '\xb0';

    // NKo Digits U+07C0 - U+07C9
    if (std::strcmp(value, "\xdf\x80") >= 0 and
        std::strcmp(value, "\xdf\x89") <= 0)
      return value[1] - '\x80';

    // Fullwidth Digits U+FF10 - U+FF19
    if (std::strcmp(value, "\xef\xbc\x90") >= 0 and
        std::strcmp(value, "\xef\xbc\x99") <= 0)
      return value[2] - '\x90';

    break;
  }

  return 0;
}

bool meat::grinder::Utils::is_integer(const std::string &value,
                                      int32_t &res) {
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
  res = 0;
  unsigned int inc = 1;
  for (; c < value.length(); c += inc) {
    if ((inc = is_numeric(&value[c]))) {
      res *= 10;
      res += to_numeric(&value[c]);
    } else {
      return false;
    }
  }

  // Set the sign if we need to.
  if (neg)
    res = -(res);

  return true;
}

bool meat::grinder::Utils::is_float(const std::string &value, double &res) {
  const char *s_value = value.c_str();
  char *end_ptr;

  res = strtod(s_value, &end_ptr);
  if (end_ptr < s_value + strlen(s_value)) {
    return false;
  }
  return true;
}
