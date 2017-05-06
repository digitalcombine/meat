/*                                                                  -*- c++ -*-
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

#ifndef _MEAT_BYTECODE_H
#define _MEAT_BYTECODE_H

/** @ns */
namespace meat {

  /** @ns */
  namespace bytecode {
    const uint8_t NOOP              = 0x00;

    const uint8_t MESSAGE           = 0x01; // obj mesg_id pcnt ...
    const uint8_t MESG_RESULT       = 0X02; // obj res mesg_id pcnt ...
    const uint8_t MESG_SUPER        = 0x03;
    const uint8_t MESG_SUPER_RESULT = 0x04;

    const uint8_t BLOCK             = 0x0a; // local locals block_size
    const uint8_t CONTEXT_END       = 0x0b;

    const uint8_t ASSIGN            = 0x10; // local local
    const uint8_t ASSIGN_OATTR      = 0x11; // local object_attr
    const uint8_t ASSIGN_CATTR      = 0x12; // local class_attr
    const uint8_t ASSIGN_CLASS      = 0x13; // local class_hash_id

    const uint8_t ASSIGN_CONST_INT  = 0x14; // local const
    const uint8_t ASSIGN_CONST_FLT  = 0x15;
    const uint8_t ASSIGN_CONST_STR  = 0x16;

    const uint8_t SET_OATTR         = 0x20; // attr local
    const uint8_t SET_CATTR         = 0x21; // attr const
  };
};

#endif /* _MEAT_BYTECODE_H */
