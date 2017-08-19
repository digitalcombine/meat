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
    const uint8_t ASSIGN_PROP       = 0x11; // local object_attr
    const uint8_t ASSIGN_CLASS_PROP = 0x12; // local class_attr
    const uint8_t ASSIGN_CLASS      = 0x13; // local class_hash_id

    const uint8_t ASSIGN_CONST_INT  = 0x14; // local const
    const uint8_t ASSIGN_CONST_NUM  = 0x15;
    const uint8_t ASSIGN_CONST_TXT  = 0x16;

    const uint8_t SET_PROP          = 0x20; // attr local
    const uint8_t SET_CLASS_PROP    = 0x21; // attr const

		typedef struct __attribute__((packed)) _message_s {
			uint8_t object;
			uint32_t message_id;
			uint8_t parameters;
			uint8_t parameter[];
		} message_t;

		typedef struct __attribute__((packed)) _message_result_s {
			uint8_t object;
			uint8_t result;
			uint32_t message_id;
			uint8_t parameters;
			uint8_t parameter[];
		} message_result_t;

		typedef struct __attribute__((packed)) _block_context_s {
			uint8_t result;
			uint8_t locals;
			uint16_t code_size;
		} block_context_t;

		typedef struct __attribute__((packed)) _assign_s {
			uint8_t destination;
			uint8_t source;
		} assign_t;

		typedef struct __attribute__((packed)) _assign_property_s {
			uint8_t destination;
			uint8_t property_id;
		} assign_property_t;

		typedef struct __attribute__((packed)) _class_s {
			uint8_t destination;
			uint32_t class_id;
		} class_t;

		typedef struct __attribute__((packed)) _const_integer_s {
			uint8_t destination;
			int32_t value;
		} const_integer_t;

		typedef struct __attribute__((packed)) _const_number_s {
			uint8_t destination;
			float_t value;
		} const_number_t;

		typedef struct __attribute__((packed)) _const_text_s {
			uint8_t destination;
			unsigned char value[]; // This is 0 terminated
		} const_text_t;

		typedef struct __attribute__((packed)) _set_property_s {
			uint8_t destination;
			uint8_t source;
		} set_property_t;

		typedef struct __attribute__((packed)) _bytecode_s {
			uint8_t code;
			union {
				message_t m;
				message_result_t mr;
				block_context_t bc;
				assign_t a;
				assign_property_t ap;
				class_t c;
				const_integer_t ci;
				const_number_t cn;
				const_text_t ct;
				set_property_t sp;
			} o;
		} bytecode_t;
  };
};

#endif /* _MEAT_BYTECODE_H */
