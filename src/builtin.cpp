/*                                                                  -*- c++ -*-
 * The Meat language builtin classes.
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

#include <meat.h>
#include <meat/datastore.h>
#include <meat/utilities.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <cmath>

#ifdef TESTING
#include <testing.h>
#endif /* TESTING */

using namespace meat;

#define null (meat::Null())

/******************************************************************************
 * Object Class
 */

static meat::Reference Object_constructor(meat::Reference &klass,
																					meat::uint8_t properties) {
  return new meat::Object(klass, properties);
}

// method is:
static meat::Reference Object_om_is_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	return meat::Boolean(self == other);
}

// method isNot:
static meat::Reference Object_om_isNot_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	return meat::Boolean(!(self == other));
}

// method isType:
static meat::Reference Object_om_isType_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference type = meat::cast<meat::Context>(context).parameter(0);

  return meat::Boolean(self->is_type(type));
}

// method type
static meat::Reference Object_om_type(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();

  return self->type();
}

// method weak
static meat::Reference Object_om_weak(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();

  return self.weak();
}

static meat::vtable_entry_t ObjectMethods[] = {
  {0x00000782, 0x0c658f60, VTM_BYTECODE, 6, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x0c658f60, VTM_BYTECODE, 6, {(meat::method_ptr_t)18}},
  {0x00019850, 0x0c658f60, VTM_NATIVE  , 1, Object_om_is_},
  {0x00368f3a, 0x0c658f60, VTM_NATIVE  , 0, Object_om_type},
  {0x00379f78, 0x0c658f60, VTM_NATIVE  , 0, Object_om_weak},
  {0x331152ee, 0x0c658f60, VTM_BYTECODE, 4, {(meat::method_ptr_t)36}},
  {0x34003578, 0x0c658f60, VTM_BYTECODE, 4, {(meat::method_ptr_t)37}},
  {0x39a68c12, 0x0c658f60, VTM_NATIVE  , 1, Object_om_isNot_},
  {0x39a6a1d2, 0x0c658f60, VTM_BYTECODE, 6, {(meat::method_ptr_t)61}},
  {0x6b2d9a7a, 0x0c658f60, VTM_BYTECODE, 6, {(meat::method_ptr_t)84}},
  {0x7a8e569a, 0x0c658f60, VTM_BYTECODE, 6, {(meat::method_ptr_t)38}},
  {0x7b840562, 0x0c658f60, VTM_NATIVE  , 1, Object_om_isType_}
};

// class method is:
static meat::Reference Object_cm_is_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	return meat::Boolean(self == other);

}

// class method isNot:
static meat::Reference Object_cm_isNot_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	return meat::Boolean(!(self == other));
}

static meat::vtable_entry_t ObjectCMethods[] = {
  {0x00000782, 0x0c658f60, VTM_BYTECODE, 6, {(meat::method_ptr_t)107}},
  {0x000007a0, 0x0c658f60, VTM_BYTECODE, 6, {(meat::method_ptr_t)125}},
  {0x00019850, 0x0c658f60, VTM_NATIVE  , 1, Object_cm_is_},
  {0x00368f3a, 0x0c658f60, VTM_BYTECODE, 5, {(meat::method_ptr_t)212}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x0c658f60, VTM_NATIVE  , 1, Object_cm_isNot_},
  {0x39a6a1d2, 0x0c658f60, VTM_BYTECODE, 6, {(meat::method_ptr_t)166}},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x0c658f60, VTM_BYTECODE, 6, {(meat::method_ptr_t)189}},
  {0x7a8e569a, 0x0c658f60, VTM_BYTECODE, 6, {(meat::method_ptr_t)143}}
};

static meat::uint8_t ObjectBytecode[] = {
  0x02, 0x00, 0x05, 0x39, 0xa6, 0x8c, 0x12, 0x01, 0x04, 0x01, 0x02, 0x41, 0x79,
  0x69, 0x3a, 0x01, 0x05, 0x0b, 0x02, 0x00, 0x05, 0x00, 0x01, 0x98, 0x50, 0x01,
  0x04, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x05, 0x0b, 0x0b, 0x0b, 0x13,
  0x04, 0x67, 0x14, 0x04, 0x24, 0x02, 0x04, 0x05, 0x05, 0xcb, 0x19, 0x23, 0x00,
  0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x05, 0x0b, 0x13, 0x04, 0x67, 0x14,
  0x04, 0x24, 0x02, 0x04, 0x05, 0x05, 0xcb, 0x19, 0x23, 0x00, 0x01, 0x02, 0x41,
  0x79, 0x69, 0x3a, 0x01, 0x05, 0x0b, 0x13, 0x04, 0x67, 0x14, 0x04, 0x24, 0x02,
  0x04, 0x05, 0x00, 0x36, 0x75, 0x8e, 0x00, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a,
  0x01, 0x05, 0x0b, 0x02, 0x00, 0x05, 0x39, 0xa6, 0x8c, 0x12, 0x01, 0x04, 0x01,
  0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x05, 0x0b, 0x02, 0x00, 0x05, 0x00, 0x01,
  0x98, 0x50, 0x01, 0x04, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x05, 0x0b,
  0x13, 0x04, 0x67, 0x14, 0x04, 0x24, 0x02, 0x04, 0x05, 0x00, 0x36, 0x75, 0x8e,
  0x00, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x05, 0x0b, 0x13, 0x04, 0x67,
  0x14, 0x04, 0x24, 0x02, 0x04, 0x05, 0x05, 0xcb, 0x19, 0x23, 0x00, 0x01, 0x02,
  0x41, 0x79, 0x69, 0x3a, 0x01, 0x05, 0x0b, 0x13, 0x04, 0x67, 0x14, 0x04, 0x24,
  0x02, 0x04, 0x05, 0x05, 0xcb, 0x19, 0x23, 0x00, 0x01, 0x02, 0x41, 0x79, 0x69,
  0x3a, 0x01, 0x05, 0x0b, 0x13, 0x04, 0x03, 0xe2, 0xb9, 0x58, 0x01, 0x02, 0x41,
  0x79, 0x69, 0x3a, 0x01, 0x04, 0x0b
};

/******************************************************************************
 * Class Class
 *
 */

static meat::vtable_entry_t ClassMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

// class method getName
static meat::Reference Class_cm_getName(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  //meat::Reference klass = meat::cast<meat::Context>(context).klass();

	return new meat::Text(meat::cast<meat::Class>(self).name());
}

// class method newObject
static meat::Reference Class_cm_newObject(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
	meat::Reference messanger =
		meat::cast<meat::Context>(context).messenger();

	// Create the new object and call the initialize method.
	meat::Reference new_object = meat::cast<meat::Class>(self).new_object();
	meat::Reference init_ctx = message(new_object, "initialize", context);
	execute(init_ctx);

	// We replace the self object to the new object.
	meat::cast<meat::Context>(messanger).self(new_object);

	return null;
}

// class method subClass:body:
static meat::Reference Class_cm_subClass_as_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference name = meat::cast<meat::Context>(context).parameter(0);
  meat::Reference block = meat::cast<meat::Context>(context).parameter(1);

	if (meat::grinder_impl() != NULL) {
		//class_compiler()(self, meat::cast<meat::Text>(name), meat::cast<meat::Text>(block), context);
		meat::grinder_impl()->create_class(self, meat::cast<meat::Text>(name),
																			 meat::cast<meat::Text>(block), context);
	} else {
		throw meat::Exception("No compiler implementation is loaded.");
	}

	return null;
}

// class method super
static meat::Reference Class_cm_superClass(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();

	return meat::cast<meat::Class>(self).super();
}

static meat::vtable_entry_t ClassCMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x181f14c4, 0x03e2b958, VTM_NATIVE  , 0, Class_cm_superClass},
  {0x34003578, 0x03e2b958, VTM_BYTECODE, 4, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x50188993, 0x03e2b958, VTM_NATIVE  , 2, Class_cm_subClass_as_},
  {0x54aa30e6, 0x03e2b958, VTM_NATIVE  , 0, Class_cm_newObject},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b82e32c, 0x03e2b958, VTM_NATIVE  , 0, Class_cm_getName}
};

static meat::uint8_t ClassBytecode[] = {
  0x0b
};

/******************************************************************************
 * Context Class
 */

static meat::Reference Context_constructor(meat::Reference &cls,
																					 meat::uint8_t properties) {
  return new meat::Context(properties);
}

// method getLocal:
static meat::Reference Context_om_getLocal_(meat::Reference context) {
	meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference index = meat::cast<meat::Context>(context).parameter(0);

	return meat::cast<meat::Context>(self).local(INTEGER(index));
}

// method localVariables
static meat::Reference Context_om_localVariables(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();

	return new meat::Value(meat::cast<meat::Context>(self).locals() - 4);
}

// method messenger
static meat::Reference Context_om_messenger(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();

	return meat::cast<meat::Context>(self).messenger();
}

// method repeat:
static meat::Reference Context_om_repeat_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference block = meat::cast<meat::Context>(context).parameter(0);

	meat::cast<meat::Context>(block).messenger(context);

	for (;;) {
		meat::cast<meat::BlockContext>(block).set_break_trap();
		meat::cast<meat::BlockContext>(block).set_continue_trap();
		execute(block);

		if (meat::cast<meat::BlockContext>(block).break_called() or
				meat::cast<meat::Context>(block).is_done())
			break;

		meat::cast<meat::BlockContext>(block).reset();
	}
	meat::cast<meat::BlockContext>(block).reset();

	return null;
}

// method return
static meat::Reference Context_om_return(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();

	meat::cast<meat::Context>(self).finish();
	return null;
}

// method return:
static meat::Reference Context_om_return_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference value = meat::cast<meat::Context>(context).parameter(0);

	meat::cast<meat::Context>(self).result(value); // Set the return value
	meat::cast<meat::Context>(self).finish(); // Tell the context it's done.
	return null;
}

// method setLocal:to:
static meat::Reference Context_om_setLocal_to_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference index = meat::cast<meat::Context>(context).parameter(0);
  meat::Reference value = meat::cast<meat::Context>(context).parameter(1);

  meat::cast<meat::Context>(self).local(INTEGER(index)) = value;
  return null;
}

static meat::vtable_entry_t ContextMethods[] = {
  {0x0000043c, 0x00000000, VTM_SUPER,    0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER,    0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER,    0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER,    0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER,    0, {(meat::method_ptr_t)0}},
  {0x059a58ff, 0x1befcdac, VTM_BYTECODE, 4, {(meat::method_ptr_t)0}},
  {0x23add62f, 0x1befcdac, VTM_NATIVE,   1, Context_om_getLocal_},
  {0x2a67696c, 0x1befcdac, VTM_NATIVE,   0, Context_om_messenger},
  {0x34003578, 0x00000000, VTM_SUPER,    0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER,    0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER,    0, {(meat::method_ptr_t)0}},
	{0x4139862f, 0x1befcdac, VTM_NATIVE,   1, Context_om_repeat_},
  {0x4179693a, 0x1befcdac, VTM_NATIVE,   1, Context_om_return_},
  {0x47206ce4, 0x1befcdac, VTM_NATIVE,   0, Context_om_localVariables},
  {0x484e3d31, 0x1befcdac, VTM_NATIVE,   0, Context_om_return},
  {0x5e3131ca, 0x1befcdac, VTM_BYTECODE, 4, {(meat::method_ptr_t)1}},
  {0x675bde74, 0x1befcdac, VTM_NATIVE,   2, Context_om_setLocal_to_},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER,    0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER,    0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER,    0, {(meat::method_ptr_t)0}}
};

static meat::vtable_entry_t ContextCMethods[] = {
  {0x0000043c, 0x00000000, VTM_SUPER,    0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER,    0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER,    0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER,    0, {(meat::method_ptr_t)0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER,    0, {(meat::method_ptr_t)0}},
  {0x2c296348, 0x1befcdac, VTM_BYTECODE, 8, {(meat::method_ptr_t)72}},
  {0x39a68c12, 0x00000000, VTM_SUPER,    0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER,    0, {(meat::method_ptr_t)0}},
  {0x54aa30e6, 0x1befcdac, VTM_BYTECODE, 7, {(meat::method_ptr_t)2}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER,    0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER,    0, {(meat::method_ptr_t)0}}
};

static meat::uint8_t ContextBytecode[] = {
  0x0b, 0x0b, 0x13, 0x04, 0x4f, 0xc2, 0x61, 0x66, 0x16, 0x05, 0x43, 0x61, 0x6e,
  0x6e, 0x6f, 0x74, 0x20, 0x63, 0x72, 0x65, 0x61, 0x74, 0x65, 0x20, 0x61, 0x20,
  0x6e, 0x65, 0x77, 0x20, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x6e, 0x61, 0x6c, 0x20,
  0x43, 0x6f, 0x6e, 0x74, 0x65, 0x78, 0x74, 0x20, 0x6f, 0x62, 0x6a, 0x65, 0x63,
  0x74, 0x00, 0x02, 0x02, 0x06, 0x72, 0x79, 0x75, 0xfa, 0x00, 0x01, 0x04, 0x6d,
  0xb6, 0x8a, 0xb6, 0x02, 0x05, 0x06, 0x0b, 0x13, 0x06, 0x4f, 0xc2, 0x61, 0x66,
  0x16, 0x07, 0x43, 0x61, 0x6e, 0x6e, 0x6f, 0x74, 0x20, 0x73, 0x75, 0x62, 0x63,
  0x6c, 0x61, 0x73, 0x73, 0x20, 0x61, 0x6e, 0x20, 0x69, 0x6e, 0x74, 0x65, 0x72,
  0x6e, 0x61, 0x6c, 0x20, 0x43, 0x6f, 0x6e, 0x74, 0x65, 0x78, 0x74, 0x20, 0x63,
  0x6c, 0x61, 0x73, 0x73, 0x00, 0x01, 0x06, 0x4b, 0xe1, 0x36, 0x15, 0x01, 0x07,
  0x0b
};

/******************************************************************************
 * BlockContext Class
 */

// method break
static meat::Reference BlockContext_om_break(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();

  meat::cast<meat::BlockContext>(self).c_break();
	if (not meat::cast<meat::BlockContext>(self).break_trap_set()) {
		meat::Reference init_ctx =
			meat::cast<meat::BlockContext>(self).origin();
		meat::Reference up_context = message(init_ctx, "break", context);
    execute(up_context);
	}
  return null;
}

// method continue
static meat::Reference BlockContext_om_continue(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();

  meat::cast<meat::BlockContext>(self).c_continue();
	if (not meat::cast<meat::BlockContext>(self).continue_trap_set()) {
		meat::Reference init_ctx =
			meat::cast<meat::BlockContext>(self).origin();
		meat::Reference up_context = message(init_ctx, "continue", context);
    execute(up_context);
	}
  return null;
}

// method execute
static meat::Reference BlockContext_om_execute(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();

	meat::cast<meat::BlockContext>(self).messenger(context);
	execute(self);
	meat::cast<meat::BlockContext>(self).reset();

	return null;
}

// method executeOnBreak:
static meat::Reference
BlockContext_om_executeOnBreak_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference breakBlock = meat::cast<meat::Context>(context).parameter(0);

	meat::cast<meat::BlockContext>(self).set_break_trap();
	meat::cast<meat::BlockContext>(self).messenger(context);
	execute(self);

	if (meat::cast<meat::BlockContext>(self).break_called()) {
		meat::Reference up_context = message(breakBlock, "execute", context);
		execute(up_context);
	}

	meat::cast<meat::BlockContext>(self).reset();

	return null;
}

// method executeOnBreak:onContinue:
static meat::Reference
BlockContext_om_executeOnBreak_onContinue_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference breakBlock = meat::cast<meat::Context>(context).parameter(0);
  meat::Reference continueBlock =
		meat::cast<meat::Context>(context).parameter(1);

	meat::cast<meat::BlockContext>(self).set_break_trap();
	meat::cast<meat::BlockContext>(self).set_continue_trap();
	meat::cast<meat::BlockContext>(self).messenger(context);
	execute(self);

	if (meat::cast<meat::BlockContext>(self).break_called()) {
		meat::Reference up_context = message(breakBlock, "execute", context);
		execute(up_context);
	} else if (meat::cast<meat::BlockContext>(self).continue_called()) {
		meat::Reference up_context = message(continueBlock, "execute", context);
		execute(up_context);
	}

	meat::cast<meat::BlockContext>(self).reset();

	return null;
}

// method executeOnContinue:
static meat::Reference
BlockContext_om_executeOnContinue_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference continueBlock =
		meat::cast<meat::Context>(context).parameter(0);

	meat::cast<meat::BlockContext>(self).set_continue_trap();
	meat::cast<meat::BlockContext>(self).messenger(context);
	execute(self);

	if (meat::cast<meat::BlockContext>(self).continue_called()) {
		meat::Reference up_context = message(continueBlock, "execute", context);
		execute(up_context);
	}

	meat::cast<meat::BlockContext>(self).reset();

	return null;
}

// method return
static meat::Reference BlockContext_om_return(meat::Reference context) {
	meat::Reference self = meat::cast<meat::Context>(context).self();

	// We need to get the actual context to message the return method from.
	meat::Reference origin = meat::cast<meat::BlockContext>(self).origin();
	meat::Reference up_context = message(origin, "return", context);
	execute(up_context);

	meat::cast<meat::Context>(self).finish(); // Tell the context it's done.

	return null;
}

// method return:
static meat::Reference BlockContext_om_return_(meat::Reference context) {
	meat::Reference self = meat::cast<meat::Context>(context).self();
	meat::Reference value = meat::cast<meat::Context>(context).parameter(0);

	meat::Reference origin = meat::cast<meat::BlockContext>(self).origin();
	meat::Reference up_context = message(origin, "return:", context);
	meat::cast<meat::Context>(up_context).parameter(0, value);
	execute(up_context);

	meat::cast<meat::Context>(self).finish(); // Tell the context it's done.

	return null;
}

static meat::vtable_entry_t BlockContextMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x059a58ff, 0x46ba8a20, VTM_NATIVE  , 0, BlockContext_om_break},
  {0x290a0d52, 0x46ba8a20, VTM_NATIVE  , 1, BlockContext_om_executeOnContinue_},
  {0x3158f7a0, 0x46ba8a20, VTM_NATIVE  , 0, BlockContext_om_execute},
  {0x331152ee, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x4179693a, 0x46ba8a20, VTM_NATIVE  , 1, BlockContext_om_return_},
  {0x484e3d31, 0x46ba8a20, VTM_NATIVE  , 0, BlockContext_om_return},
  {0x5a7f66e9, 0x46ba8a20, VTM_NATIVE  , 1, BlockContext_om_executeOnBreak_},
  {0x5e3131ca, 0x46ba8a20, VTM_NATIVE  , 0, BlockContext_om_continue},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x71828805, 0x46ba8a20, VTM_NATIVE  , 2, BlockContext_om_executeOnBreak_onContinue_},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static meat::vtable_entry_t BlockContextCMethods[] = {
  {0x0000043c, 0x00000000, VTM_SUPER, 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER, 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER, 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER, 0, {(meat::method_ptr_t)0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER, 0, {(meat::method_ptr_t)0}},
  {0x2c296348, 0x00000000, VTM_SUPER, 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER, 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER, 0, {(meat::method_ptr_t)0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER, 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER, 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER, 0, {(meat::method_ptr_t)0}}
};

/******************************************************************************
 * Null Class
 */

#define nullObject (klass->property(0))

static meat::vtable_entry_t NullMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x002539a7, VTM_BYTECODE, 6, {(meat::method_ptr_t)0}},
  {0x4300a340, 0x002539a7, VTM_BYTECODE, 5, {(meat::method_ptr_t)23}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static meat::vtable_entry_t NullCMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x2c296348, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x50188993, 0x002539a7, VTM_BYTECODE, 9, {(meat::method_ptr_t)102}},
  {0x54aa30e6, 0x002539a7, VTM_BYTECODE, 7, {(meat::method_ptr_t)35}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

#undef nullObject

static meat::uint8_t NullBytecode[] = {
  0x13, 0x04, 0x67, 0x14, 0x04, 0x24, 0x02, 0x04, 0x05, 0x00, 0x36, 0x75, 0x8e,
  0x00, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x05, 0x0b, 0x12, 0x04, 0x00,
  0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x04, 0x0b, 0x13, 0x04, 0x4f, 0xc2,
  0x61, 0x66, 0x16, 0x05, 0x43, 0x61, 0x6e, 0x6e, 0x6f, 0x74, 0x20, 0x63, 0x72,
  0x65, 0x61, 0x74, 0x65, 0x20, 0x61, 0x20, 0x6e, 0x65, 0x77, 0x20, 0x69, 0x6e,
  0x74, 0x65, 0x72, 0x6e, 0x61, 0x6c, 0x20, 0x4e, 0x75, 0x6c, 0x6c, 0x20, 0x6f,
  0x62, 0x6a, 0x65, 0x63, 0x74, 0x00, 0x02, 0x02, 0x06, 0x2a, 0x67, 0x69, 0x6c,
  0x00, 0x01, 0x04, 0x6d, 0xb6, 0x8a, 0xb6, 0x02, 0x05, 0x06, 0x0b, 0x13, 0x06,
  0x4f, 0xc2, 0x61, 0x66, 0x16, 0x07, 0x43, 0x61, 0x6e, 0x6e, 0x6f, 0x74, 0x20,
  0x73, 0x75, 0x62, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x20, 0x61, 0x6e, 0x20, 0x69,
  0x6e, 0x74, 0x65, 0x72, 0x6e, 0x61, 0x6c, 0x20, 0x4e, 0x75, 0x6c, 0x6c, 0x20,
  0x63, 0x6c, 0x61, 0x73, 0x73, 0x00, 0x02, 0x02, 0x08, 0x2a, 0x67, 0x69, 0x6c,
  0x00, 0x01, 0x06, 0x6d, 0xb6, 0x8a, 0xb6, 0x02, 0x07, 0x08, 0x0b
};

/******************************************************************************
 * Exception Class
 */

static meat::Reference Exception_constructor(meat::Reference &klass,
																						 meat::uint8_t properties) {
  return new meat::Exception(klass, properties);
}

#define mesg (self->property(0))
#define cntxt (self->property(1))

static meat::vtable_entry_t ExceptionMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x331152ee, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x38b735d9, 0x4fc26166, VTM_BYTECODE, 5, {(meat::method_ptr_t)0}},
  {0x38eb0035, 0x4fc26166, VTM_BYTECODE, 5, {(meat::method_ptr_t)12}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

// class method throw
static meat::Reference Exception_cm_throw(meat::Reference context) {
	meat::Reference ctx = meat::cast<meat::Context>(context).messenger();
	throw meat::Exception(null, ctx);
}

// class method throw:
static meat::Reference Exception_cm_throw_(meat::Reference context) {
  meat::Reference message = meat::cast<meat::Context>(context).parameter(0);
	meat::Reference ctx = meat::cast<meat::Context>(context).messenger();
	throw meat::Exception(message, ctx);
}

// class method throw:for:
static meat::Reference Exception_cm_throw_for_(meat::Reference context) {
  meat::Reference message = meat::cast<meat::Context>(context).parameter(0);
  meat::Reference ctx = meat::cast<meat::Context>(context).parameter(1);
	throw meat::Exception(message, ctx);
}

// class method try:
static meat::Reference Exception_cm_try_(meat::Reference context) {
  meat::Reference block = meat::cast<meat::Context>(context).parameter(0);

	meat::cast<meat::Context>(block).messenger(context);
	try {
		execute(block);
	} catch (meat::Exception &err) {
	} catch (...) {
	}
	meat::cast<meat::BlockContext>(block).reset();

	return null;
}

// class method try:catch:
static meat::Reference Exception_cm_try_catch_(meat::Reference context) {
  meat::Reference try_block = meat::cast<meat::Context>(context).parameter(0);
  meat::Reference catch_block = meat::cast<meat::Context>(context).parameter(1);

	meat::cast<meat::Context>(try_block).messenger(context);
	meat::cast<meat::Context>(catch_block).messenger(context);
	try {
		execute(try_block);
	} catch (std::exception &err) {
		execute(catch_block);
	}

	meat::cast<meat::BlockContext>(try_block).reset();
	meat::cast<meat::BlockContext>(catch_block).reset();

	return null;
}

// class method try:catch:do
static meat::Reference Exception_cm_try_catch_do_(meat::Reference context) {
  meat::Reference try_block = meat::cast<meat::Context>(context).parameter(0);
  meat::Reference error = meat::cast<meat::Context>(context).parameter(1);
  meat::Reference catch_block = meat::cast<meat::Context>(context).parameter(2);

	meat::cast<meat::Context>(try_block).messenger(context);
	meat::cast<meat::Context>(catch_block).messenger(context);
	try {
		execute(try_block);
	} catch (meat::Exception &err) {
		meat::Reference excp = new meat::Exception(err);
		meat::cast<meat::Context>(catch_block).local(INTEGER(error)) = excp;
		execute(catch_block);
	}

	meat::cast<meat::BlockContext>(try_block).reset();
	meat::cast<meat::BlockContext>(catch_block).reset();

	return null;
}

static meat::vtable_entry_t ExceptionCMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x003675df, 0x4fc26166, VTM_NATIVE  , 1, Exception_cm_try_},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x0693a6e6, 0x4fc26166, VTM_NATIVE  , 0, Exception_cm_throw},
  {0x2c296348, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x4be13615, 0x4fc26166, VTM_NATIVE  , 1, Exception_cm_throw_},
  {0x4e48308e, 0x4fc26166, VTM_NATIVE  , 3, Exception_cm_try_catch_do_},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6db68ab6, 0x4fc26166, VTM_NATIVE  , 2, Exception_cm_throw_for_},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7bebbd56, 0x4fc26166, VTM_NATIVE  , 2, Exception_cm_try_catch_}
};

static meat::uint8_t ExceptionBytecode[] = {
  0x11, 0x04, 0x01, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x04, 0x0b, 0x11,
  0x04, 0x00, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x04, 0x0b
};

/******************************************************************************
 * Boolean Class
 */

#define trueObject (klass->property(0))
#define falseObject (klass->property(1))

// method and:
static meat::Reference Boolean_om_and_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference klass = meat::cast<meat::Context>(context).klass();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	return ((self == trueObject and other == trueObject) ?
					trueObject : falseObject);
}

// method isFalse:
static meat::Reference Boolean_om_isFalse_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference klass = meat::cast<meat::Context>(context).klass();
  meat::Reference block = meat::cast<meat::Context>(context).parameter(0);

	if (self == falseObject) {
		meat::cast<meat::BlockContext>(block).messenger(context);
		execute(block);
		meat::cast<meat::BlockContext>(block).reset();
	}
	return null;
}

// method isFalse:else:
static meat::Reference Boolean_om_isFalse_else_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference klass = meat::cast<meat::Context>(context).klass();
  meat::Reference falseBlock = meat::cast<meat::Context>(context).parameter(0);
  meat::Reference elseBlock = meat::cast<meat::Context>(context).parameter(1);

	if (self == falseObject) {
		meat::cast<meat::BlockContext>(falseBlock).messenger(context);
		execute(falseBlock);
		meat::cast<meat::BlockContext>(falseBlock).reset();
	} else {
		meat::cast<meat::BlockContext>(elseBlock).messenger(context);
		execute(elseBlock);
		meat::cast<meat::BlockContext>(elseBlock).reset();
	}
	return null;
}

// method isTrue:
static meat::Reference Boolean_om_isTrue_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference klass = meat::cast<meat::Context>(context).klass();
  meat::Reference block = meat::cast<meat::Context>(context).parameter(0);

	if (self == trueObject)  {
		meat::cast<meat::BlockContext>(block).messenger(context);
		execute(block);
		meat::cast<meat::BlockContext>(block).reset();
	}
	return null;
}

// method isTrue:else:
static meat::Reference Boolean_om_isTrue_else_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference klass = meat::cast<meat::Context>(context).klass();
  meat::Reference trueBlock = meat::cast<meat::Context>(context).parameter(0);
  meat::Reference elseBlock = meat::cast<meat::Context>(context).parameter(1);

	if (self == trueObject) {
		meat::cast<meat::BlockContext>(trueBlock).messenger(context);
		execute(trueBlock);
		meat::cast<meat::BlockContext>(trueBlock).reset();
	} else {
		meat::cast<meat::BlockContext>(elseBlock).messenger(context);
		execute(elseBlock);
		meat::cast<meat::BlockContext>(elseBlock).reset();
	}
	return null;
}

// method or:
static meat::Reference Boolean_om_or_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference klass = meat::cast<meat::Context>(context).klass();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	return ((self == trueObject or other == trueObject) ?
					trueObject : falseObject);
}

// method xor:
static meat::Reference Boolean_om_xor_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference klass = meat::cast<meat::Context>(context).klass();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	return (((self == trueObject) xor (other == trueObject)) ?
					trueObject : falseObject);
}

static meat::vtable_entry_t BooleanMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x0001aad3, 0x67140424, VTM_BYTECODE, 8, {(meat::method_ptr_t)65}},
  {0x0001aeb7, 0x67140424, VTM_NATIVE  , 1, Boolean_om_or_},
  {0x002dc143, 0x67140424, VTM_NATIVE  , 1, Boolean_om_and_},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00383b3f, 0x67140424, VTM_NATIVE  , 1, Boolean_om_xor_},
  {0x2c025c80, 0x67140424, VTM_BYTECODE, 8, {(meat::method_ptr_t)0}},
  {0x331152ee, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x47607971, 0x67140424, VTM_NATIVE  , 2, Boolean_om_isTrue_else_},
  {0x57c628fe, 0x67140424, VTM_NATIVE  , 2, Boolean_om_isFalse_else_},
  {0x5bc502b2, 0x67140424, VTM_NATIVE  , 1, Boolean_om_isFalse_},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b80e98e, 0x67140424, VTM_NATIVE  , 1, Boolean_om_isTrue_},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static meat::vtable_entry_t BooleanCMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x0036758e, 0x67140424, VTM_BYTECODE, 5, {(meat::method_ptr_t)133}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x05cb1923, 0x67140424, VTM_BYTECODE, 5, {(meat::method_ptr_t)121}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x2c296348, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

#undef trueObject
#undef falseObject

static meat::uint8_t BooleanBytecode[] = {
  0x12, 0x04, 0x00, 0x02, 0x00, 0x05, 0x00, 0x00, 0x07, 0xa0, 0x01, 0x04, 0x0a,
  0x06, 0x01, 0x00, 0x10, 0x16, 0x08, 0x74, 0x72, 0x75, 0x65, 0x00, 0x01, 0x02,
  0x41, 0x79, 0x69, 0x3a, 0x01, 0x08, 0x0b, 0x0a, 0x07, 0x01, 0x00, 0x11, 0x16,
  0x08, 0x66, 0x61, 0x6c, 0x73, 0x65, 0x00, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a,
  0x01, 0x08, 0x0b, 0x01, 0x05, 0x47, 0x60, 0x79, 0x71, 0x02, 0x06, 0x07, 0x0b,
  0x12, 0x04, 0x00, 0x02, 0x00, 0x05, 0x00, 0x00, 0x07, 0xa0, 0x01, 0x04, 0x0a,
  0x06, 0x01, 0x00, 0x0c, 0x12, 0x08, 0x01, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a,
  0x01, 0x08, 0x0b, 0x0a, 0x07, 0x01, 0x00, 0x0c, 0x12, 0x08, 0x00, 0x01, 0x02,
  0x41, 0x79, 0x69, 0x3a, 0x01, 0x08, 0x0b, 0x01, 0x05, 0x47, 0x60, 0x79, 0x71,
  0x02, 0x06, 0x07, 0x0b, 0x12, 0x04, 0x01, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a,
  0x01, 0x04, 0x0b, 0x12, 0x04, 0x00, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01,
  0x04, 0x0b
};

/******************************************************************************
 * Numeric Class
 */

static meat::vtable_entry_t NumericMethods[] = {
  {0x00000025, 0x6bfcb30e, VTM_BYTECODE, 7, {(meat::method_ptr_t)0}},
  {0x0000002a, 0x6bfcb30e, VTM_BYTECODE, 7, {(meat::method_ptr_t)36}},
  {0x0000002b, 0x6bfcb30e, VTM_BYTECODE, 7, {(meat::method_ptr_t)72}},
  {0x0000002d, 0x6bfcb30e, VTM_BYTECODE, 7, {(meat::method_ptr_t)108}},
  {0x0000002f, 0x6bfcb30e, VTM_BYTECODE, 7, {(meat::method_ptr_t)144}},
  {0x0000003c, 0x6bfcb30e, VTM_BYTECODE, 7, {(meat::method_ptr_t)180}},
  {0x0000003e, 0x6bfcb30e, VTM_BYTECODE, 7, {(meat::method_ptr_t)327}},
  {0x0000005e, 0x6bfcb30e, VTM_BYTECODE, 7, {(meat::method_ptr_t)400}},
  {0x00000781, 0x6bfcb30e, VTM_BYTECODE, 7, {(meat::method_ptr_t)216}},
  {0x00000782, 0x6bfcb30e, VTM_BYTECODE, 7, {(meat::method_ptr_t)253}},
  {0x000007a0, 0x6bfcb30e, VTM_BYTECODE, 7, {(meat::method_ptr_t)290}},
  {0x000007bf, 0x6bfcb30e, VTM_BYTECODE, 7, {(meat::method_ptr_t)363}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x2c025c80, 0x6bfcb30e, VTM_BYTECODE, 6, {(meat::method_ptr_t)479}},
  {0x331152ee, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x36e711e1, 0x6bfcb30e, VTM_BYTECODE, 6, {(meat::method_ptr_t)536}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6701127b, 0x6bfcb30e, VTM_BYTECODE, 6, {(meat::method_ptr_t)436}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static meat::vtable_entry_t NumericCMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x17cbc00b, 0x6bfcb30e, VTM_BYTECODE, 6, {(meat::method_ptr_t)579}},
  {0x2c296348, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x2ded23e0, 0x6bfcb30e, VTM_BYTECODE, 6, {(meat::method_ptr_t)631}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static meat::uint8_t NumericBytecode[] = {
  0x13, 0x05, 0x4f, 0xc2, 0x61, 0x66, 0x16, 0x06, 0x4d, 0x65, 0x74, 0x68, 0x6f,
  0x64, 0x20, 0x25, 0x20, 0x75, 0x6e, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x64,
  0x00, 0x01, 0x05, 0x4b, 0xe1, 0x36, 0x15, 0x01, 0x06, 0x0b, 0x13, 0x05, 0x4f,
  0xc2, 0x61, 0x66, 0x16, 0x06, 0x4d, 0x65, 0x74, 0x68, 0x6f, 0x64, 0x20, 0x2a,
  0x20, 0x75, 0x6e, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x64, 0x00, 0x01, 0x05,
  0x4b, 0xe1, 0x36, 0x15, 0x01, 0x06, 0x0b, 0x13, 0x05, 0x4f, 0xc2, 0x61, 0x66,
  0x16, 0x06, 0x4d, 0x65, 0x74, 0x68, 0x6f, 0x64, 0x20, 0x2b, 0x20, 0x75, 0x6e,
  0x64, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x64, 0x00, 0x01, 0x05, 0x4b, 0xe1, 0x36,
  0x15, 0x01, 0x06, 0x0b, 0x13, 0x05, 0x4f, 0xc2, 0x61, 0x66, 0x16, 0x06, 0x4d,
  0x65, 0x74, 0x68, 0x6f, 0x64, 0x20, 0x2d, 0x20, 0x75, 0x6e, 0x64, 0x65, 0x66,
  0x69, 0x6e, 0x65, 0x64, 0x00, 0x01, 0x05, 0x4b, 0xe1, 0x36, 0x15, 0x01, 0x06,
  0x0b, 0x13, 0x05, 0x4f, 0xc2, 0x61, 0x66, 0x16, 0x06, 0x4d, 0x65, 0x74, 0x68,
  0x6f, 0x64, 0x20, 0x2f, 0x20, 0x75, 0x6e, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65,
  0x64, 0x00, 0x01, 0x05, 0x4b, 0xe1, 0x36, 0x15, 0x01, 0x06, 0x0b, 0x13, 0x05,
  0x4f, 0xc2, 0x61, 0x66, 0x16, 0x06, 0x4d, 0x65, 0x74, 0x68, 0x6f, 0x64, 0x20,
  0x3c, 0x20, 0x75, 0x6e, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x64, 0x00, 0x01,
  0x05, 0x4b, 0xe1, 0x36, 0x15, 0x01, 0x06, 0x0b, 0x13, 0x05, 0x4f, 0xc2, 0x61,
  0x66, 0x16, 0x06, 0x4d, 0x65, 0x74, 0x68, 0x6f, 0x64, 0x20, 0x3c, 0x3d, 0x20,
  0x75, 0x6e, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x64, 0x00, 0x01, 0x05, 0x4b,
  0xe1, 0x36, 0x15, 0x01, 0x06, 0x0b, 0x13, 0x05, 0x4f, 0xc2, 0x61, 0x66, 0x16,
  0x06, 0x4d, 0x65, 0x74, 0x68, 0x6f, 0x64, 0x20, 0x3c, 0x3e, 0x20, 0x75, 0x6e,
  0x64, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x64, 0x00, 0x01, 0x05, 0x4b, 0xe1, 0x36,
  0x15, 0x01, 0x06, 0x0b, 0x13, 0x05, 0x4f, 0xc2, 0x61, 0x66, 0x16, 0x06, 0x4d,
  0x65, 0x74, 0x68, 0x6f, 0x64, 0x20, 0x3d, 0x3d, 0x20, 0x75, 0x6e, 0x64, 0x65,
  0x66, 0x69, 0x6e, 0x65, 0x64, 0x00, 0x01, 0x05, 0x4b, 0xe1, 0x36, 0x15, 0x01,
  0x06, 0x0b, 0x13, 0x05, 0x4f, 0xc2, 0x61, 0x66, 0x16, 0x06, 0x4d, 0x65, 0x74,
  0x68, 0x6f, 0x64, 0x20, 0x3e, 0x20, 0x75, 0x6e, 0x64, 0x65, 0x66, 0x69, 0x6e,
  0x65, 0x64, 0x00, 0x01, 0x05, 0x4b, 0xe1, 0x36, 0x15, 0x01, 0x06, 0x0b, 0x13,
  0x05, 0x4f, 0xc2, 0x61, 0x66, 0x16, 0x06, 0x4d, 0x65, 0x74, 0x68, 0x6f, 0x64,
  0x20, 0x3e, 0x3d, 0x20, 0x75, 0x6e, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x64,
  0x00, 0x01, 0x05, 0x4b, 0xe1, 0x36, 0x15, 0x01, 0x06, 0x0b, 0x13, 0x05, 0x4f,
  0xc2, 0x61, 0x66, 0x16, 0x06, 0x4d, 0x65, 0x74, 0x68, 0x6f, 0x64, 0x20, 0x5e,
  0x20, 0x75, 0x6e, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x64, 0x00, 0x01, 0x05,
  0x4b, 0xe1, 0x36, 0x15, 0x01, 0x06, 0x0b, 0x13, 0x04, 0x4f, 0xc2, 0x61, 0x66,
  0x16, 0x05, 0x4d, 0x65, 0x74, 0x68, 0x6f, 0x64, 0x20, 0x61, 0x62, 0x73, 0x6f,
  0x6c, 0x75, 0x74, 0x65, 0x20, 0x75, 0x6e, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65,
  0x64, 0x00, 0x01, 0x04, 0x4b, 0xe1, 0x36, 0x15, 0x01, 0x05, 0x0b, 0x13, 0x04,
  0x4f, 0xc2, 0x61, 0x66, 0x16, 0x05, 0x55, 0x6e, 0x61, 0x62, 0x6c, 0x65, 0x20,
  0x74, 0x6f, 0x20, 0x63, 0x6f, 0x6e, 0x76, 0x65, 0x72, 0x74, 0x20, 0x6e, 0x75,
  0x6d, 0x65, 0x72, 0x69, 0x63, 0x20, 0x76, 0x61, 0x6c, 0x75, 0x65, 0x20, 0x74,
  0x6f, 0x20, 0x54, 0x65, 0x78, 0x74, 0x00, 0x01, 0x04, 0x4b, 0xe1, 0x36, 0x15,
  0x01, 0x05, 0x0b, 0x13, 0x04, 0x4f, 0xc2, 0x61, 0x66, 0x16, 0x05, 0x4d, 0x65,
  0x74, 0x68, 0x6f, 0x64, 0x20, 0x6e, 0x65, 0x67, 0x61, 0x74, 0x69, 0x76, 0x65,
  0x20, 0x75, 0x6e, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x64, 0x00, 0x01, 0x04,
  0x4b, 0xe1, 0x36, 0x15, 0x01, 0x05, 0x0b, 0x13, 0x04, 0x4f, 0xc2, 0x61, 0x66,
  0x16, 0x05, 0x4e, 0x75, 0x6d, 0x65, 0x72, 0x69, 0x63, 0x20, 0x76, 0x61, 0x6c,
  0x75, 0x65, 0x20, 0x68, 0x61, 0x73, 0x20, 0x6e, 0x6f, 0x20, 0x6d, 0x69, 0x6e,
  0x69, 0x6d, 0x75, 0x6d, 0x20, 0x76, 0x61, 0x6c, 0x75, 0x65, 0x00, 0x01, 0x04,
  0x4b, 0xe1, 0x36, 0x15, 0x01, 0x05, 0x0b, 0x13, 0x04, 0x4f, 0xc2, 0x61, 0x66,
  0x16, 0x05, 0x4e, 0x75, 0x6d, 0x65, 0x72, 0x69, 0x63, 0x20, 0x76, 0x61, 0x6c,
  0x75, 0x65, 0x20, 0x68, 0x61, 0x73, 0x20, 0x6e, 0x6f, 0x20, 0x6d, 0x69, 0x6e,
  0x69, 0x6d, 0x75, 0x6d, 0x20, 0x76, 0x61, 0x6c, 0x75, 0x65, 0x00, 0x01, 0x04,
  0x4b, 0xe1, 0x36, 0x15, 0x01, 0x05, 0x0b
};

/******************************************************************************
 * Integer Class
 */

static meat::Reference Integer_constructor(meat::Reference &klass,
																					 meat::uint8_t properties) {
  return new meat::Value(klass, properties);
}

// method %
static meat::Reference Integer_om_mod(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

  return new meat::Value(INTEGER(self) % INTEGER(other));
}

// method *
static meat::Reference Integer_om_mult(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	int32_t res;
	if (__builtin_mul_overflow(INTEGER(self), INTEGER(other), &res))
		throw meat::Exception("Integer multiplication overflow");
  return new meat::Value(res);
}

// method +
static meat::Reference Integer_om_add(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	int32_t res;
	if (__builtin_add_overflow(INTEGER(self), INTEGER(other), &res))
		throw meat::Exception("Integer addition overflow");
  return new meat::Value(res);
}

// method -
static meat::Reference Integer_om_sub(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	int32_t res;
	if (__builtin_sub_overflow(INTEGER(self), INTEGER(other), &res))
		throw meat::Exception("Integer subtraction underflow");
  return new meat::Value(res);
}

// method /
static meat::Reference Integer_om_div(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

  return new meat::Value(INTEGER(self) / INTEGER(other));
}

// method <
static meat::Reference Integer_om_less(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

  try {
    if (INTEGER(self) < INTEGER(other))
      return meat::Boolean(true);
  } catch (...) {}
  return meat::Boolean(false);
}

// method <=
static meat::Reference Integer_om_less_equal(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

  try {
    if (INTEGER(self) <= INTEGER(other))
      return meat::Boolean(true);
  } catch (...) {}
  return meat::Boolean(false);
}

// method <>
static meat::Reference Integer_om_nequals(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

  try {
    if (INTEGER(self) != INTEGER(other))
      return meat::Boolean(true);
  } catch (...) {}
  return meat::Boolean(false);
}

// method ==
static meat::Reference Integer_om_equals(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

  try {
    if (INTEGER(self) == INTEGER(other))
      return meat::Boolean(true);
  } catch (...) {}
  return meat::Boolean(false);
}

// method >
static meat::Reference Integer_om_greater(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

  try {
    if (INTEGER(self) > INTEGER(other))
      return meat::Boolean(true);
  } catch (...) {}
  return meat::Boolean(false);
}

// method >=
static meat::Reference Integer_om_greater_equal(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

  try {
    if (INTEGER(self) >= INTEGER(other))
      return meat::Boolean(true);
  } catch (...) {}
  return meat::Boolean(false);
}

// method ^
static meat::Reference Integer_om_pow(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

  return new meat::Value((meat::float_t)std::pow(FLOAT(self),
																								 FLOAT(other)));
}

// method and:
static meat::Reference Integer_om_and_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

  return new meat::Value(INTEGER(self) & INTEGER(other));
}

// method asText
static meat::Reference Integer_om_asText(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();

  std::stringstream result;
  result << INTEGER(self);
  return new meat::Text(result.str());
}

// method lshift
static meat::Reference Integer_om_lshift(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();

  return new meat::Value(INTEGER(self) << 1);
}

// method lshift:
static meat::Reference Integer_om_lshift_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference amount = meat::cast<meat::Context>(context).parameter(0);

  return new meat::Value(INTEGER(self) << INTEGER(amount));
}

// method neg
static meat::Reference Integer_om_negative(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();

  return new meat::Value(-INTEGER(self));
}

// method abs
static meat::Reference Integer_om_absolute(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();

  return new meat::Value(std::abs(INTEGER(self)));
}

// method or:
static meat::Reference Integer_om_or_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

  return new meat::Value(INTEGER(self) | INTEGER(other));
}

// method rshift
static meat::Reference Integer_om_rshift(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();

  return new meat::Value(INTEGER(self) >> 1);
}

// method rshift:
static meat::Reference Integer_om_rshift_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference amount = meat::cast<meat::Context>(context).parameter(0);

  return new meat::Value(INTEGER(self) >> INTEGER(amount));
}

// method timesDo:
static meat::Reference Integer_om_timesDo_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference block = meat::cast<meat::Context>(context).parameter(0);

  meat::int32_t limit = INTEGER(self);
  for (meat::int32_t c = 0; c < limit; c++) {
    meat::execute(block);
    meat::cast<meat::BlockContext>(block).reset();
  }
  return null;
}

// method xor:
static meat::Reference Integer_om_xor_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

  return new meat::Value(INTEGER(self) ^ INTEGER(other));
}

static meat::vtable_entry_t IntegerMethods[] = {
  {0x00000025, 0x57ee193d, VTM_NATIVE  , 1, Integer_om_mod},
  {0x0000002a, 0x57ee193d, VTM_NATIVE  , 1, Integer_om_mult},
  {0x0000002b, 0x57ee193d, VTM_NATIVE  , 1, Integer_om_add},
  {0x0000002d, 0x57ee193d, VTM_NATIVE  , 1, Integer_om_sub},
  {0x0000002f, 0x57ee193d, VTM_NATIVE  , 1, Integer_om_div},
  {0x0000003c, 0x57ee193d, VTM_NATIVE  , 1, Integer_om_less},
  {0x0000003e, 0x57ee193d, VTM_NATIVE  , 1, Integer_om_greater},
  {0x0000005e, 0x57ee193d, VTM_NATIVE  , 1, Integer_om_pow},
  {0x00000781, 0x57ee193d, VTM_NATIVE  , 1, Integer_om_less_equal},
  {0x00000782, 0x57ee193d, VTM_NATIVE  , 1, Integer_om_nequals},
  {0x000007a0, 0x57ee193d, VTM_NATIVE  , 1, Integer_om_equals},
  {0x000007bf, 0x57ee193d, VTM_NATIVE  , 1, Integer_om_greater_equal},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x0001aeb7, 0x57ee193d, VTM_NATIVE  , 1, Integer_om_or_},
  {0x002dc143, 0x57ee193d, VTM_NATIVE  , 1, Integer_om_and_},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00383b3f, 0x57ee193d, VTM_NATIVE  , 1, Integer_om_xor_},
  {0x043b7de4, 0x57ee193d, VTM_NATIVE  , 1, Integer_om_timesDo_},
  {0x1b492392, 0x57ee193d, VTM_NATIVE  , 1, Integer_om_lshift_},
  {0x2c025c80, 0x57ee193d, VTM_NATIVE  , 0, Integer_om_asText},
  {0x331152ee, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x36e711e1, 0x57ee193d, VTM_NATIVE  , 0, Integer_om_negative},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x3ed0cf97, 0x57ee193d, VTM_NATIVE  , 0, Integer_om_lshift},
  {0x490de351, 0x57ee193d, VTM_NATIVE  , 0, Integer_om_rshift},
  {0x58ae871a, 0x57ee193d, VTM_NATIVE  , 1, Integer_om_rshift_},
  {0x6701127b, 0x57ee193d, VTM_NATIVE  , 0, Integer_om_absolute},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

// class method maxValue
static meat::Reference Integer_cm_maxValue(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference klass = meat::cast<meat::Context>(context).klass();

	return new meat::Value(2147483647);
}

// class method minValue
static meat::Reference Integer_cm_minValue(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference klass = meat::cast<meat::Context>(context).klass();

	return new meat::Value(-2147483647-1);
}

static meat::vtable_entry_t IntegerCMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x17cbc00b, 0x57ee193d, VTM_NATIVE  , 0, Integer_cm_maxValue},
  {0x2c296348, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x2ded23e0, 0x57ee193d, VTM_NATIVE  , 0, Integer_cm_minValue},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

/******************************************************************************
 * Number Class
 */

static meat::Reference Number_constructor(meat::Reference &klass,
																					meat::uint8_t properties) {
  return new meat::Value(klass, properties);
}

// method %
static meat::Reference Number_om_mod(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	return new meat::Value(std::fmod(FLOAT(self), FLOAT(other)));
}

// method *
static meat::Reference Number_om_mult(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	return new meat::Value(FLOAT(self) * FLOAT(other));
}

// method +
static meat::Reference Number_om_add(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	return new meat::Value(FLOAT(self) + FLOAT(other));
}

// method -
static meat::Reference Number_om_sub(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	return new meat::Value(FLOAT(self) - FLOAT(other));
}

// method /
static meat::Reference Number_om_div(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

  if (FLOAT(other) == 0.0)
    throw meat::Exception("Divide by zero");

	return new meat::Value(FLOAT(self) / FLOAT(other));
}

// method <
static meat::Reference Number_om_less(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	try {
		if (FLOAT(self) < FLOAT(other))
			return meat::Boolean(true);
	} catch (...) {}
	return meat::Boolean(false);
}

// method <=
static meat::Reference Number_om_less_equal(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	try {
		if (FLOAT(self) <= FLOAT(other))
			return meat::Boolean(true);
	} catch (...) {}
	return meat::Boolean(false);
}

// method <>
static meat::Reference Number_om_nequals(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference klass = meat::cast<meat::Context>(context).klass();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	try {
		if (FLOAT(self) != FLOAT(other))
			return meat::Boolean(true);
	} catch (...) {}
	return meat::Boolean(false);
}

// method ==
static meat::Reference Number_om_equals(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
	meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	try {
		if (FLOAT(self) == FLOAT(other))
			return meat::Boolean(true);
	} catch (...) {}
	return meat::Boolean(false);
}

// method >
static meat::Reference Number_om_greater(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	try {
		if (FLOAT(self) > FLOAT(other))
			return meat::Boolean(true);
	} catch (...) {}
	return meat::Boolean(false);
}

// method >=
static meat::Reference Number_om_greater_equal(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	try {
		if (FLOAT(self) >= FLOAT(other))
			return meat::Boolean(true);
	} catch (...) {}
	return meat::Boolean(false);
}

// method ^
static meat::Reference Number_om_pow(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	return new meat::Value((meat::float_t)std::pow(FLOAT(self),
																								 FLOAT(other)));
}

// method absolute
static meat::Reference Number_om_absolute(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();

	return new meat::Value(std::abs(FLOAT(self)));
}

// method asText
static meat::Reference Number_om_asText(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();

	std::stringstream result;
	result << (double)FLOAT(self);
	return new meat::Text(result.str());
}

// method negative
static meat::Reference Number_om_negative(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();

	return new meat::Value(-FLOAT(self));
}

static meat::vtable_entry_t NumberMethods[] = {
  {0x00000025, 0x0bbdc76a, VTM_NATIVE  , 1, Number_om_mod},
  {0x0000002a, 0x0bbdc76a, VTM_NATIVE  , 1, Number_om_mult},
  {0x0000002b, 0x0bbdc76a, VTM_NATIVE  , 1, Number_om_add},
  {0x0000002d, 0x0bbdc76a, VTM_NATIVE  , 1, Number_om_sub},
  {0x0000002f, 0x0bbdc76a, VTM_NATIVE  , 1, Number_om_div},
  {0x0000003c, 0x0bbdc76a, VTM_NATIVE  , 1, Number_om_less},
  {0x0000003e, 0x0bbdc76a, VTM_NATIVE  , 1, Number_om_greater},
  {0x0000005e, 0x0bbdc76a, VTM_NATIVE  , 1, Number_om_pow},
  {0x00000781, 0x0bbdc76a, VTM_NATIVE  , 1, Number_om_less_equal},
  {0x00000782, 0x0bbdc76a, VTM_NATIVE  , 1, Number_om_nequals},
  {0x000007a0, 0x0bbdc76a, VTM_NATIVE  , 1, Number_om_equals},
  {0x000007bf, 0x0bbdc76a, VTM_NATIVE  , 1, Number_om_greater_equal},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x2c025c80, 0x0bbdc76a, VTM_NATIVE  , 0, Number_om_asText},
  {0x331152ee, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x36e711e1, 0x0bbdc76a, VTM_NATIVE  , 0, Number_om_negative},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6701127b, 0x0bbdc76a, VTM_NATIVE  , 0, Number_om_absolute},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static meat::vtable_entry_t NumberCMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x17cbc00b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x2c296348, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x2ded23e0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

/******************************************************************************
 * Text Class
 */

static meat::Reference Text_constructor(meat::Reference &klass,
																				meat::uint8_t properties) {
  return new meat::Text(klass, properties);
}

// method !=
static meat::Reference Text_om_nequal(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	return Boolean(cast<Text>(self).compare(cast<Text>(other)) != 0);
}

// method *
static meat::Reference Text_om_mult(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference count = meat::cast<meat::Context>(context).parameter(0);

  meat::int32_t cnt_value = INTEGER(count);
	meat::Text *result = new meat::Text();

  for (int c = 0; c < cnt_value; c++)
    (*result) += meat::cast<meat::Text>(self);

  return result;
}

// method +
static meat::Reference Text_om_add(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	return new Text(cast<Text>(self) + cast<Text>(other));
}

// method <
static meat::Reference Text_om_less(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	return Boolean(cast<Text>(self).compare(cast<Text>(other)) < 0);
}

// method <=
static meat::Reference Text_om_less_equal(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	return Boolean(cast<Text>(self).compare(cast<Text>(other)) <= 0);
}

// method ==
static meat::Reference Text_om_equal(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	return Boolean(cast<Text>(self).compare(cast<Text>(other)) == 0);
}

// method >
static meat::Reference Text_om_greater(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	return Boolean(cast<Text>(self).compare(cast<Text>(other)) > 0);
}

// method >=
static meat::Reference Text_om_greater_equal(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	return Boolean(cast<Text>(self).compare(cast<Text>(other)) >= 0);
}

// method get:
static meat::Reference Text_om_get_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference index = meat::cast<meat::Context>(context).parameter(0);

	meat::Text *new_string = new meat::Text();
  (*new_string) += meat::cast<meat::Text>(self)[INTEGER(index) - 1];
  return new_string;
}

// method get:to:
static meat::Reference Text_om_get_to_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference start = meat::cast<meat::Context>(context).parameter(0);
  meat::Reference end = meat::cast<meat::Context>(context).parameter(1);

  meat::int32_t start_pos = INTEGER(start) - 1;
  meat::int32_t len = INTEGER(end) - start_pos;

	meat::Text *new_string = new meat::Text();
  (*new_string) += meat::cast<meat::Text>(self).substr(start_pos, len);
  return new_string;
}

// method getCharAt:
static meat::Reference Text_om_getCharAt_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference index = meat::cast<meat::Context>(context).parameter(0);

	char ch = meat::cast<meat::Text>(self)[INTEGER(index) - 1];
	return new meat::Text(&ch);
}

// method isEmpty
static meat::Reference Text_om_isEmpty(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();

	return Boolean(cast<Text>(self).empty());
}

// method length
static meat::Reference Text_om_length(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();

	unsigned int len = 0;
	meat::Text &str = meat::cast<meat::Text>(self);

	// Count UTF-8 characters not bytes.
	for (unsigned int i = 0; str[i]; ++i)
		if ((str[i] & 0xC0) != 0x80) len++;

	return new meat::Value((meat::int32_t)len);
}

static meat::vtable_entry_t TextMethods[] = {
  {0x0000002a, 0x0027b94d, VTM_NATIVE  , 1, Text_om_mult},
  {0x0000002b, 0x0027b94d, VTM_NATIVE  , 1, Text_om_add},
  {0x0000003c, 0x0027b94d, VTM_NATIVE  , 1, Text_om_less},
  {0x0000003e, 0x0027b94d, VTM_NATIVE  , 1, Text_om_greater},
  {0x0000043c, 0x0027b94d, VTM_NATIVE  , 1, Text_om_nequal},
  {0x00000781, 0x0027b94d, VTM_NATIVE  , 1, Text_om_less_equal},
  {0x000007a0, 0x0027b94d, VTM_NATIVE  , 1, Text_om_equal},
  {0x000007bf, 0x0027b94d, VTM_NATIVE  , 1, Text_om_greater_equal},
	{0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
	{0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
	{0x2c025c80, 0x0027b94d, VTM_BYTECODE, 4, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
	{0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x3e0e3ae7, 0x0027b94d, VTM_NATIVE  , 0, Text_om_length},
	{0x69eddf35, 0x0027b94d, VTM_NATIVE  , 1, Text_om_getCharAt_},
	{0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7aab326f, 0x0027b94d, VTM_NATIVE  , 0, Text_om_isEmpty},
  {0x7b7a1326, 0x0027b94d, VTM_NATIVE  , 2, Text_om_get_to_},
	{0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static meat::vtable_entry_t TextCMethods[] = {
  {0x0000043c, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x2c296348, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static meat::uint8_t TextBytecode[] = {
  0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x00, 0x0b
};

/******************************************************************************
 * List Class
 */

static meat::Reference list_constructor(meat::Reference &cls,
					 meat::uint8_t properties) {
  return new meat::List(cls, properties);
}

// method append:
static meat::Reference List_cm_append_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference value = meat::cast<meat::Context>(context).parameter(0);

  if (value->is_type(meat::Class::resolve("List"))) {
    ((meat::List &)(*self)).insert(((meat::List &)(*self)).end(),
      ((meat::List &)(*value)).begin(),
      ((meat::List &)(*value)).end());
  } else {
    ((meat::List &)(*self)).push_back(value);
  }
  return null;
}

// method at:insert:
static meat::Reference List_cm_at_insert_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference index = meat::cast<meat::Context>(context).parameter(0);
  meat::Reference value = meat::cast<meat::Context>(context).parameter(1);

  meat::cast<meat::List>(self).insert(meat::cast<meat::List>(self).begin() + (INTEGER(index) - 1), value);
  return null;
}

// method clear
static meat::Reference List_cm_clear(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();

  meat::cast<meat::List>(self).clear();
  return null;
}

// method copy
static meat::Reference List_om_copy(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();

	meat::Reference new_list = new meat::List();
	meat::cast<meat::List>(new_list) = meat::cast<meat::List>(self);
	return new_list;
}

// method forEach:do:
static meat::Reference List_cm_forEach_do_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference item = meat::cast<meat::Context>(context).parameter(0);
  meat::Reference block = meat::cast<meat::Context>(context).parameter(1);

  meat::uint8_t local_id = INTEGER(item);
  meat::cast<meat::Context>(block).messenger(context);

  meat::List::iterator it = meat::cast<meat::List>(self).begin();
  for (; it != meat::cast<meat::List>(self).end(); it++) {
		meat::cast<meat::BlockContext>(block).set_break_trap();
    meat::cast<meat::Context>(block).local(local_id) = *it;
    execute(block);

		if (meat::cast<meat::BlockContext>(block).break_called()) break;
		else meat::cast<meat::BlockContext>(block).reset();
  }
	meat::cast<meat::BlockContext>(block).reset();

  return null;
}

// method get:
static meat::Reference List_cm_get_(meat::Reference context) {
	meat::Reference self = meat::cast<meat::Context>(context).self();
	meat::Reference index = meat::cast<meat::Context>(context).parameter(0);

	try {
		return meat::cast<meat::List>(self).at(INTEGER(index) - 1);
	} catch (std::out_of_range &err) {
		throw meat::Exception("List index out of Range", context);
	}
}

// method isEmpty
static meat::Reference List_cm_isEmpty(meat::Reference context) {
	meat::Reference self = meat::cast<meat::Context>(context).self();

	return Boolean(meat::cast<meat::List>(self).empty());
}

// method last
static meat::Reference List_cm_last(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();

	return meat::cast<meat::List>(self).back();
}

// method pop
static meat::Reference List_cm_pop(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();

	meat::cast<meat::List>(self).pop_back();
	return null;
}

// method push:
static meat::Reference List_cm_push_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference value = meat::cast<meat::Context>(context).parameter(0);

  meat::cast<meat::List>(self).push_back(value);
  return null;
}

// method remove:
static meat::Reference List_cm_remove_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference value = meat::cast<meat::Context>(context).parameter(0);

  meat::List::iterator it = ((meat::List &)(*self)).begin();
  while (it != meat::cast<meat::List>(self).end()) {
    if (*it == value)
      meat::cast<meat::List>(self).erase(it);
    it++;
  }
  return null;
}

// method removeAt:
static meat::Reference List_cm_removeAt_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference index = meat::cast<meat::Context>(context).parameter(0);

  meat::cast<meat::List>(self).erase(meat::cast<meat::List>(self).begin() + (INTEGER(index) - 1));
  return null;
}

// method removeFrom:to:
static meat::Reference List_cm_removeFrom_to_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference start = meat::cast<meat::Context>(context).parameter(0);
  meat::Reference end = meat::cast<meat::Context>(context).parameter(1);

  meat::cast<meat::List>(self).erase(meat::cast<meat::List>(self).begin() + (INTEGER(start) - 1),
									 meat::cast<meat::List>(self).begin() + (INTEGER(end) - 1));
  return null;
}

// method set:to:
static meat::Reference List_cm_set_to_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference index = meat::cast<meat::Context>(context).parameter(0);
  meat::Reference value = meat::cast<meat::Context>(context).parameter(1);

  try {
    meat::cast<meat::List>(self).at(INTEGER(index) - 1) = value;
  } catch (std::out_of_range &err) {
    throw meat::Exception("Index out of Range", context);
  }
  return null;
}

// method size
static meat::Reference List_cm_size(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference klass = meat::cast<meat::Context>(context).klass();

  return new meat::Value((meat::int32_t)meat::cast<meat::List>(self).size());
}

// method swap:with:
static meat::Reference List_cm_swap_with_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference first = meat::cast<meat::Context>(context).parameter(0);
  meat::Reference second = meat::cast<meat::Context>(context).parameter(1);

  try {
    meat::Reference temp = meat::cast<meat::List>(self).at(INTEGER(first) - 1);
    meat::cast<meat::List>(self).at(INTEGER(first) - 1) = meat::cast<meat::List>(self).at(INTEGER(second) - 1);
    meat::cast<meat::List>(self).at(INTEGER(second) - 1) = temp;
  } catch (std::out_of_range &err) {
    throw meat::Exception("Index out of Range", context);
  }
  return null;
}

static meat::vtable_entry_t ListMethods[] = {
	{0x0000043c, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x0001b251, 0x002424be, VTM_NATIVE  , 0, List_cm_pop},
	{0x002eaf75, 0x002424be, VTM_NATIVE  , 0, List_om_copy},
  {0x00305ba4, 0x002424be, VTM_NATIVE  , 1, List_cm_get_},
  {0x00329296, 0x002424be, VTM_NATIVE  , 0, List_cm_last},
  {0x0035e001, 0x002424be, VTM_NATIVE  , 0, List_cm_size},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x018b0a25, 0x002424be, VTM_NATIVE  , 2, List_cm_forEach_do_},
  {0x05a5b64d, 0x002424be, VTM_NATIVE  , 0, List_cm_clear},
  {0x06613460, 0x002424be, VTM_NATIVE  , 1, List_cm_push_},
  {0x12b51eb5, 0x002424be, VTM_NATIVE  , 2, List_cm_removeFrom_to_},
  {0x13a72bf2, 0x002424be, VTM_NATIVE  , 2, List_cm_swap_with_},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x3c94d873, 0x002424be, VTM_NATIVE  , 2, List_cm_at_insert_},
  {0x41141866, 0x002424be, VTM_NATIVE  , 1, List_cm_remove_},
  {0x4c6fc15b, 0x002424be, VTM_NATIVE  , 1, List_cm_removeAt_},
  {0x50b51489, 0x002424be, VTM_NATIVE  , 1, List_cm_append_},
  {0x7644da37, 0x002424be, VTM_NATIVE  , 2, List_cm_set_to_},
  {0x7aab326f, 0x002424be, VTM_NATIVE  , 0, List_cm_isEmpty}
};

static meat::vtable_entry_t ListCMethods[] = {
	{0x0000043c, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x0001a9a0, 0x002424be, VTM_BYTECODE, 4, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x2c296348, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static meat::uint8_t ListBytecode[] = {
  0x01, 0x00, 0x54, 0xaa, 0x30, 0xe6, 0x00, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a,
  0x01, 0x00, 0x0b
};

/******************************************************************************
 * Set Class
 */

static meat::Reference Set_constructor(meat::Reference &klass,
																			 meat::uint8_t properties) {

	return new meat::Set(klass, properties);
}

// method clear
static meat::Reference Set_om_clear(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference klass = meat::cast<meat::Context>(context).klass();

	meat::cast<meat::Set>(self).clear();
	return null;
}

// method copy
static meat::Reference Set_om_copy(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference klass = meat::cast<meat::Context>(context).klass();

	meat::Reference new_set = new meat::Set();
	meat::cast<meat::Set>(new_set) = meat::cast<meat::Set>(self);
	return new_set;
}

// method entries
static meat::Reference Set_om_entries(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference klass = meat::cast<meat::Context>(context).klass();

	return new meat::Value((meat::int32_t)meat::cast<meat::Set>(self).size());
}

// method forEach:do:
static meat::Reference Set_om_forEach_do_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference klass = meat::cast<meat::Context>(context).klass();
  meat::Reference item = meat::cast<meat::Context>(context).parameter(0);
  meat::Reference block = meat::cast<meat::Context>(context).parameter(1);

	meat::uint8_t local_id = INTEGER(item);

	meat::cast<meat::Context>(block).messenger(context);

	meat::Set::iterator it = meat::cast<meat::Set>(self).begin();
	for (; it != meat::cast<meat::Set>(self).end(); it++) {
		meat::cast<meat::BlockContext>(block).local(local_id) = *it;
		execute(block);

		if (meat::cast<meat::BlockContext>(block).break_called() or
				meat::cast<meat::Context>(block).is_done())
			break;

		meat::cast<meat::BlockContext>(block).reset();
	}
	meat::cast<meat::BlockContext>(block).reset();

	return null;
}

// method hasEntry:
static meat::Reference Set_om_hasEntry_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference klass = meat::cast<meat::Context>(context).klass();
  meat::Reference value = meat::cast<meat::Context>(context).parameter(0);

	return Boolean(meat::cast<meat::Set>(self).find(value) !=
								 meat::cast<meat::Set>(self).end());
}

// method insert:
static meat::Reference Set_om_insert_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference klass = meat::cast<meat::Context>(context).klass();
  meat::Reference value = meat::cast<meat::Context>(context).parameter(0);

	meat::cast<meat::Set>(self).insert(value);
	return null;
}

// method isEmpty
static meat::Reference Set_om_isEmpty(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference klass = meat::cast<meat::Context>(context).klass();

	return Boolean(meat::cast<meat::Set>(self).empty());
}

// method remove:
static meat::Reference Set_om_remove_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference klass = meat::cast<meat::Context>(context).klass();
  meat::Reference value = meat::cast<meat::Context>(context).parameter(0);

	meat::Set::iterator it = meat::cast<meat::Set>(self).begin();
	while (it != meat::cast<meat::Set>(self).end()) {
		if (*it == value)
			meat::cast<meat::Set>(self).erase(it);
		it++;
	}

	return null;
}

// method swap:
static meat::Reference Set_om_swap_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference klass = meat::cast<meat::Context>(context).klass();
  meat::Reference other = meat::cast<meat::Context>(context).parameter(0);

	meat::cast<meat::Set>(self).swap(meat::cast<meat::Set>(other));
	return null;
}

static meat::vtable_entry_t SetMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x002eaf75, 0x00014442, VTM_NATIVE  , 0, Set_om_copy},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x018b0a25, 0x00014442, VTM_NATIVE  , 2, Set_om_forEach_do_},
  {0x05a5b64d, 0x00014442, VTM_NATIVE  , 0, Set_om_clear},
  {0x068c2107, 0x00014442, VTM_NATIVE  , 1, Set_om_swap_},
  {0x212284bb, 0x00014442, VTM_NATIVE  , 0, Set_om_entries},
  {0x331152ee, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x41141866, 0x00014442, VTM_NATIVE  , 1, Set_om_remove_},
  {0x578f4fa5, 0x00014442, VTM_NATIVE  , 1, Set_om_hasEntry_},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x74a794cd, 0x00014442, VTM_NATIVE  , 1, Set_om_insert_},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7aab326f, 0x00014442, VTM_NATIVE  , 0, Set_om_isEmpty},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static meat::vtable_entry_t SetCMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x0001a9a0, 0x00014442, VTM_BYTECODE, 4, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x2c296348, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static meat::uint8_t SetBytecode[] = {
  0x01, 0x00, 0x54, 0xaa, 0x30, 0xe6, 0x00, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a,
  0x01, 0x00, 0x0b
};

/******************************************************************************
 * Index Class
 */

static meat::Reference index_constructor(meat::Reference &cls,
																				 meat::uint8_t properties) {
  return new meat::Index(cls, properties);
}

// method get:
static meat::Reference Index_om_get_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference key = meat::cast<meat::Context>(context).parameter(0);

  if (meat::cast<meat::Index>(self).find(key) !=
			meat::cast<meat::Index>(self).end())
    return meat::cast<meat::Index>(self)[key];
  throw meat::Exception("Invalid key in index");
}

// method has:
static meat::Reference Index_om_has_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference key = meat::cast<meat::Context>(context).parameter(0);

	return meat::Boolean(meat::cast<meat::Index>(self).find(key) !=
											 meat::cast<meat::Index>(self).end());
}

// method remove:
static meat::Reference Index_om_remove_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference key = meat::cast<meat::Context>(context).parameter(0);

  meat::cast<meat::Index>(self).erase(key);
  return null;
}

// method set:to:
static meat::Reference Index_om_set_to_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference key = meat::cast<meat::Context>(context).parameter(0);
  meat::Reference value = meat::cast<meat::Context>(context).parameter(1);

  meat::cast<meat::Index>(self)[key] = value;
  return null;
}

// method size
static meat::Reference Index_om_size(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();

  meat::int32_t size = (meat::cast<meat::Index>(self)).size();
  return new meat::Value(size);
}

static meat::vtable_entry_t IndexMethods[] = {
	{0x0000043c, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x00305ba4, 0x043838b2, VTM_NATIVE, 1, Index_om_get_},
  {0x0030c0e0, 0x043838b2, VTM_NATIVE, 1, Index_om_has_},
  {0x0035e001, 0x043838b2, VTM_NATIVE, 0, Index_om_size},
	{0x00368f3a, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x41141866, 0x043838b2, VTM_NATIVE, 1, Index_om_remove_},
  {0x7644da37, 0x043838b2, VTM_NATIVE, 2, Index_om_set_to_}
};

// class method new
static meat::Reference Index_cm_new(meat::Reference context) {
  return new meat::Index;
}

static meat::vtable_entry_t IndexCMethods[] = {
	{0x0000043c, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x0001a9a0, 0x043838b2, VTM_NATIVE, 0, Index_cm_new},
	{0x00368f3a, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x2c296348, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}}
};

/******************************************************************************
 * Application Gitgo Class
 */

static int arg_count(int count = 0) {
  static int argc = 0;
  if (count > 0) argc = count;
  return argc;
}

static const char **args(const char *argv[] = (const char **)0) {
  static const char **arguments;
  if (argv != (const char **)0) arguments = argv;
  return arguments;
}

static meat::vtable_entry_t ApplicationMethods[] = {
	{0x0000043c, 0x00000000, VTM_SUPER,    0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER,    0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER,    0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER,    0, {(meat::method_ptr_t)0}},
  {0x05c30872, 0x419df72b, VTM_BYTECODE, 6, {(meat::method_ptr_t)0}},
	{0x34003578, 0x00000000, VTM_SUPER,    0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER,    0, {(meat::method_ptr_t)0}}
};

// class method parameters
static meat::Reference Application_cm_parameters(meat::Reference context) {
  return new meat::Value((meat::int32_t)arg_count());
}

// class method parameter: index
static meat::Reference Application_cm_parameter_(meat::Reference context) {
  meat::Reference index = meat::cast<meat::Context>(context).parameter(0);
  meat::int32_t idx = INTEGER(index);

  if (idx > arg_count() or idx <= 0)
    throw meat::Exception("Parameter index out of range");

  const char **argv = args();

  return new meat::Text(argv[idx - 1]);
}

// class method getEnviron: key
static meat::Reference Application_cm_getEnviron_(meat::Reference context) {
  meat::Reference key = meat::cast<meat::Context>(context).parameter(0);

#if defined(_WIN32) || defined(_WIN64)
  LPTSTR value = new TCHAR[4096];
  DWORD result;
  result = GetEnvironmentVariable(meat::cast<meat::Text>(key), value, 4096);
  if (result == 0) {
    delete[] value;
    return new meat::Text("");
  } else {
    meat::Reference retvalue = new meat::meat::cast<meat::Text>(value);
    delete[] value;
    return retvalue;
  }

#elif defined(__linux__)
  char *value = getenv(meat::cast<meat::Text>(key).c_str());
  if (value != (char *)0)
    return new meat::Text(value);
  else
    return new meat::Text("");
#else
#  error("Don't know how to read the system environment")
#endif
}

// class method setEnviron: key to: value
static meat::Reference Application_cm_setEnviron_to_(meat::Reference context) {
  meat::Reference key = meat::cast<meat::Context>(context).parameter(0);
  meat::Reference value = meat::cast<meat::Context>(context).parameter(1);

#if defined(_WIN32) || defined(_WIN64)
  SetEnvironmentVariable(meat::cast<meat::Text>(key).c_str(),
                         meat::cast<meat::Text>(value).c_str());
#elif defined(__linux__)
  setenv(meat::cast<meat::Text>(key).c_str(),
				 meat::cast<meat::Text>(value).c_str(), 1);
#else
#  error("Don't know how to set the system environment")
#endif
  return null;
}

static meat::vtable_entry_t ApplicationCMethods[] = {
	{0x0000043c, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x0462f057, 0x419df72b, VTM_NATIVE, 1, Application_cm_getEnviron_},
	{0x068b6f7b, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x1b6d6881, 0x419df72b, VTM_NATIVE, 1, Application_cm_parameter_},
  {0x1b6d68ba, 0x419df72b, VTM_NATIVE, 0, Application_cm_parameters},
	{0x2c296348, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x50b30a5d, 0x419df72b, VTM_NATIVE, 1, Application_cm_setEnviron_to_},
	{0x54aa30e6, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}}
};

static meat::uint8_t ApplicationBytecode[] = {
  0x13, 0x04, 0x4f, 0xc2, 0x61, 0x66, 0x16, 0x05, 0x41, 0x70, 0x70, 0x6c, 0x69,
  0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x6d, 0x65, 0x74, 0x68, 0x6f, 0x64,
  0x20, 0x65, 0x6e, 0x74, 0x72, 0x79, 0x20, 0x6d, 0x75, 0x73, 0x74, 0x20, 0x62,
  0x65, 0x20, 0x6f, 0x76, 0x65, 0x72, 0x72, 0x69, 0x64, 0x64, 0x65, 0x6e, 0x00,
  0x01, 0x04, 0x4b, 0xe1, 0x36, 0x15, 0x01, 0x05, 0x0b
};

static meat::uint8_t Symbols[] = {
  "%\0*\0+\0-\0/\0<\0<=\0<>\0==\0>\0>=\0Application\0BlockContext\0"
  "Boolean\0Class\0Context\0Exception\0Index\0Integer\0"
  "List\0Null\0Number\0Numeric\0Object\0Set\0Text\0^\0"
  "absolute\0and:\0append:\0asText\0at:insert:\0break\0cleanUp\0clear\0"
  "context\0continue\0copy\0entries\0entry\0execute\0executeOnBreak:\0"
  "executeOnBreak:onContinue:\0executeOnContinue:\0false\0forEach:do:\0get:\0"
  "get:to:\0getCharAt:\0getEnviron:\0getLocal:\0getName\0has:\0hasEntry:\0"
  "initialize\0insert:\0is:\0isClass\0isEmpty\0isFalse:\0isFalse:else:\0"
  "isNot:\0isNull\0isObject\0isTrue:\0isTrue:else:\0isType:\0items\0last\0"
  "length\0localVariables\0lshift\0lshift:\0maxValue\0message\0messenger\0"
  "minValue\0negative\0new\0newObject\0not\0object\0or:\0parameter:\0"
  "parameters\0pop\0push:\0remove:\0removeAt:\0removeFrom:to:\0repeat:\0"
  "return\0return:\0rshift\0rshift:\0set:to:\0setLocal:to:\0subClass:as:\0"
  "superClass\0swap:\0swap:with:\0throw\0throw:\0throw:for:\0timesDo:\0true\0"
  "try:\0try:catch:\0try:catch:do:\0type\0uplevel\0weak\0xor:\0\0"
};

/******************************************************************************
 * Public API
 */

void meat::initialize(int argc, const char *argv[]) {
  arg_count(argc);
  args(argv);

  /* Create the Class base class. */
  Class *class_cls = new Class(Null());
	class_cls->set_vtable(11, ClassMethods, meat::STATIC);
  class_cls->set_class_vtable(14, ClassCMethods, STATIC);
	class_cls->bytecode(1, ClassBytecode, meat::STATIC);
	Class::record(class_cls, "Class");

  /* Create the Object base class. */
  Class *object_cls = new Class("Class");
  object_cls->set_constructor(Object_constructor);
  object_cls->set_vtable(12, ObjectMethods, STATIC);
  object_cls->set_class_vtable(10, ObjectCMethods, STATIC);
  object_cls->bytecode(227, ObjectBytecode, STATIC);
  Class::record(object_cls, "Object");

  /*  When the first two class were created the ClassClass reference was null.
   * So here we finish the initialization of the Object and Class classes.
   */
  object_cls->_type = Class::resolve("Class");
  class_cls->_type = Class::resolve("Class").weak();

	/* Create the Context class. */
  Class *context_cls = new Class("Object");
  context_cls->set_constructor(Context_constructor);
  context_cls->set_vtable(20, ContextMethods, STATIC);
  context_cls->set_class_vtable(11, ContextCMethods, STATIC);
  context_cls->bytecode(131, ContextBytecode, STATIC);
  Class::record(context_cls, "Context");

  /* Create the BlockContext class. */
  Class *block_cls = new Class("Context");
  block_cls->set_vtable(20, BlockContextMethods, STATIC);
	block_cls->set_class_vtable(11, BlockContextCMethods, STATIC);
  Class::record(block_cls, "BlockContext");

  /* Create the Null class. */
  Class *null_cls = new Class("Object", 1, 0);
  null_cls->set_vtable(12, NullMethods, STATIC);
  null_cls->set_class_vtable(12, NullCMethods, STATIC);
  null_cls->bytecode(167, NullBytecode, STATIC);
  Class::record(null_cls, "Null");
  null_cls->property(0) = new Object(Class::resolve("Null"));

  /* Create the Exception class. */
  Class *except_cls = new meat::Class("Object", 2);
  except_cls->set_constructor(Exception_constructor);
  except_cls->set_vtable(14, ExceptionMethods, STATIC);
  except_cls->set_class_vtable(17, ExceptionCMethods, STATIC);
  except_cls->bytecode(24, ExceptionBytecode, STATIC);
  Class::record(except_cls, "Exception");

	/* Create the Boolean class and objects. */
  Class *bool_cls = new meat::Class("Object", 2, 0);
  bool_cls->set_vtable(21, BooleanMethods, meat::STATIC);
  bool_cls->set_class_vtable(13, BooleanCMethods, meat::STATIC);
  bool_cls->bytecode(145, BooleanBytecode, meat::STATIC);
  Class::record(bool_cls, "Boolean");
  bool_cls->property(0) = new Value(true);
  bool_cls->property(1) = new Value(false);

  /* Create the Numeric class. */
  Class *numeric_cls = new Class("Object");
  numeric_cls->set_vtable(25, NumericMethods, STATIC);
  numeric_cls->set_class_vtable(13, NumericCMethods, STATIC);
  numeric_cls->bytecode(683, NumericBytecode, STATIC);
  Class::record(numeric_cls, "Numeric");

	/* Create the Integer class. */
  Class *integer_cls = new Class("Numeric");
	integer_cls->set_constructor(Integer_constructor);
  integer_cls->set_vtable(33, IntegerMethods, STATIC);
	integer_cls->set_class_vtable(13, IntegerCMethods, STATIC);
  Class::record(integer_cls, "Integer");

	/* Create the Number class. */
  Class *number_cls = new Class("Numeric");
	number_cls->set_constructor(Number_constructor);
  number_cls->set_vtable(25, NumberMethods, STATIC);
	number_cls->set_class_vtable(13, NumberCMethods, STATIC);
  Class::record(number_cls, "Number");

  /* Create the Text class. */
  Class *text_cls = new Class("Object");
	text_cls->set_constructor(Text_constructor);
  text_cls->set_vtable(22, TextMethods, STATIC);
	text_cls->set_class_vtable(11, TextCMethods, STATIC);
	text_cls->bytecode(9, TextBytecode, meat::STATIC);
  Class::record(text_cls, "Text");

  /* Create the List class. */
  Class *list_cls = new Class("Object");
  list_cls->set_constructor(list_constructor);
  list_cls->set_vtable(22, ListMethods, STATIC);
  list_cls->set_class_vtable(12, ListCMethods, STATIC);
	list_cls->bytecode(16, ListBytecode, meat::STATIC);
  Class::record(list_cls, "List");

	/* Create the Set class. */
	Class *set_cls = new meat::Class("Object");
  set_cls->set_constructor(Set_constructor);
  set_cls->set_vtable(21, SetMethods, meat::STATIC);
  set_cls->set_class_vtable(12, SetCMethods, meat::STATIC);
  set_cls->bytecode(16, SetBytecode, meat::STATIC);
	Class::record(set_cls, "Set");

  /* Create the Index class. */
  Class *index_cls = new Class("Object");
  index_cls->set_constructor(index_constructor);
  index_cls->set_vtable(11, IndexMethods, STATIC);
  index_cls->set_class_vtable(10, IndexCMethods, STATIC);
  Class::record(index_cls, "Index");

  /* Create the Application class. */
  Class *app_cls = new Class("Object");
  app_cls->set_vtable(7, ApplicationMethods, STATIC);
  app_cls->set_class_vtable(13, ApplicationCMethods, STATIC);
  app_cls->bytecode(61, ApplicationBytecode, STATIC);
  Class::record(app_cls, "Application");

	//class_compiler() = compiler;
  meat::data::initialize();

#if defined(__WIN32__)
  meat::data::Library::add_path("C:\\meat\\");
#elif defined(__linux__)
  meat::data::Library::add_path("/usr/lib/meat/");
#endif

	data::Library *library = data::Library::create("__builtin__");
	library->add(class_cls, "Class");
	library->add(object_cls, "Object");
	library->add(null_cls, "Null");
	library->add(except_cls, "Exception");
	library->add(context_cls, "Context");
	library->add(block_cls, "BlockContext");
	library->add(numeric_cls, "Numeric");
	library->add(integer_cls, "Integer");
	library->add(number_cls, "Number");
	library->add(text_cls, "Text");
	library->add(bool_cls, "Boolean");
	library->add(list_cls, "List");
	library->add(set_cls, "Set");
	library->add(index_cls, "Index");
	library->add(app_cls, "Application");

	library->set_symbols(Symbols, meat::STATIC);
}
