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
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

	return (self == other ? meat::BTrue() : meat::BFalse());
}

// method isNot:
static meat::Reference Object_om_isNot_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

	return (!(self == other) ? meat::BTrue() : meat::BFalse());
}

// method isType:
static meat::Reference Object_om_isType_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference type = CONTEXT(context).get_param(0);

  return (self->is_type(type) ? meat::BTrue() : meat::BFalse());
}

// method type
static meat::Reference Object_om_type(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();

  return self->get_type();
}

// method weak
static meat::Reference Object_om_weak(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference klass = CONTEXT(context).get_class();

  return self.weak();
}

static meat::vtable_entry_t ObjectMethods[] = {
  {0x0000043c, 0x0c658f60, VTM_BYTECODE, 6, {.offset = 0}},
  {0x000007a0, 0x0c658f60, VTM_BYTECODE, 6, {.offset = 18}},
  {0x00019850, 0x0c658f60, VTM_NATIVE,   1, Object_om_is_},
  {0x00368f3a, 0x0c658f60, VTM_NATIVE,   0, Object_om_type},
  {0x00379f78, 0x0c658f60, VTM_NATIVE,   0, Object_om_weak},
  {0x34003578, 0x0c658f60, VTM_BYTECODE, 4, {.offset = 36}},
  {0x39a68c12, 0x0c658f60, VTM_NATIVE,   1, Object_om_isNot_},
  {0x39a6a1d2, 0x0c658f60, VTM_BYTECODE, 6, {.offset = 60}},
  {0x6b2d9a7a, 0x0c658f60, VTM_BYTECODE, 6, {.offset = 83}},
  {0x7a8e569a, 0x0c658f60, VTM_BYTECODE, 6, {.offset = 37}},
  {0x7b840562, 0x0c658f60, VTM_NATIVE,   1, Object_om_isType_}
};

// class method is:
static meat::Reference Object_cm_is_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference klass = CONTEXT(context).get_class();
  meat::Reference other = CONTEXT(context).get_param(0);

  	return (self == other ? meat::BTrue() : meat::BFalse());

}

// class method isNot:
static meat::Reference Object_cm_isNot_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference klass = CONTEXT(context).get_class();
  meat::Reference other = CONTEXT(context).get_param(0);

    return (!(self == other) ? meat::BTrue() : meat::BFalse());

}

static meat::vtable_entry_t ObjectCMethods[] = {
  {0x0000043c, 0x0c658f60, VTM_BYTECODE, 6, {.offset = 106}},
  {0x000007a0, 0x0c658f60, VTM_BYTECODE, 6, {.offset = 124}},
  {0x00019850, 0x0c658f60, VTM_NATIVE,   1, Object_cm_is_},
  {0x00368f3a, 0x0c658f60, VTM_BYTECODE, 5, {.offset = 211}},
  {0x068b6f7b, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x2c296348, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x39a68c12, 0x0c658f60, VTM_NATIVE,   1, Object_cm_isNot_},
  {0x39a6a1d2, 0x0c658f60, VTM_BYTECODE, 6, {.offset = 165}},
  {0x54aa30e6, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x6b2d9a7a, 0x0c658f60, VTM_BYTECODE, 6, {.offset = 188}},
  {0x7a8e569a, 0x0c658f60, VTM_BYTECODE, 6, {.offset = 142}}
};

static meat::uint8_t ObjectBytecode[] = {
  0x02, 0x00, 0x05, 0x39, 0xa6, 0x8c, 0x12, 0x01, 0x04, 0x01, 0x02, 0x41, 0x79,
  0x69, 0x3a, 0x01, 0x05, 0x0b, 0x02, 0x00, 0x05, 0x00, 0x01, 0x98, 0x50, 0x01,
  0x04, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x05, 0x0b, 0x0b, 0x13, 0x04,
  0x67, 0x14, 0x04, 0x24, 0x02, 0x04, 0x05, 0x05, 0xcb, 0x19, 0x23, 0x00, 0x01,
  0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x05, 0x0b, 0x13, 0x04, 0x67, 0x14, 0x04,
  0x24, 0x02, 0x04, 0x05, 0x05, 0xcb, 0x19, 0x23, 0x00, 0x01, 0x02, 0x41, 0x79,
  0x69, 0x3a, 0x01, 0x05, 0x0b, 0x13, 0x04, 0x67, 0x14, 0x04, 0x24, 0x02, 0x04,
  0x05, 0x00, 0x36, 0x75, 0x8e, 0x00, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01,
  0x05, 0x0b, 0x02, 0x00, 0x05, 0x39, 0xa6, 0x8c, 0x12, 0x01, 0x04, 0x01, 0x02,
  0x41, 0x79, 0x69, 0x3a, 0x01, 0x05, 0x0b, 0x02, 0x00, 0x05, 0x00, 0x01, 0x98,
  0x50, 0x01, 0x04, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x05, 0x0b, 0x13,
  0x04, 0x67, 0x14, 0x04, 0x24, 0x02, 0x04, 0x05, 0x00, 0x36, 0x75, 0x8e, 0x00,
  0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x05, 0x0b, 0x13, 0x04, 0x67, 0x14,
  0x04, 0x24, 0x02, 0x04, 0x05, 0x05, 0xcb, 0x19, 0x23, 0x00, 0x01, 0x02, 0x41,
  0x79, 0x69, 0x3a, 0x01, 0x05, 0x0b, 0x13, 0x04, 0x67, 0x14, 0x04, 0x24, 0x02,
  0x04, 0x05, 0x05, 0xcb, 0x19, 0x23, 0x00, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a,
  0x01, 0x05, 0x0b, 0x13, 0x04, 0x03, 0xe2, 0xb9, 0x58, 0x01, 0x02, 0x41, 0x79,
  0x69, 0x3a, 0x01, 0x04, 0x0b
};

/******************************************************************************
 * Class Class
 *
 */

static meat::class_compiler_fn &class_compiler() {
	static meat::class_compiler_fn compiler;
	return compiler;
}

static meat::vtable_entry_t ClassMethods[] = {
  {0x0000043c, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {.offset = 0}}
};

// class method getName
static meat::Reference Class_cm_getName(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference klass = CONTEXT(context).get_class();

	return new meat::Text(CLASS(self).get_name());
}

// class method newObject
static meat::Reference Class_cm_newObject(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
	meat::Reference uplevel = CONTEXT(context).get_messenger();

	// Create the new object and call the initialize method.
	meat::Reference new_object = CLASS(self).new_object();
	meat::Reference init_ctx = message(new_object, "initialize", context);
	execute(init_ctx);

	// We replace the self object to the new object.
	CONTEXT(uplevel).reset_self(new_object);

	return null;
}

// class method subClass:body:
static meat::Reference Class_cm_subClass_body_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference name = CONTEXT(context).get_param(0);
  meat::Reference block = CONTEXT(context).get_param(1);

	if (class_compiler() != 0) {
		class_compiler()(self, TEXT(name), TEXT(block), context);
	} else {
		throw meat::Exception("No compiler implementation is loaded.");
	}

	return null;
}

// class method super
static meat::Reference Class_cm_superClass(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();

	return CLASS(self).get_super();
}

static meat::vtable_entry_t ClassCMethods[] = {
  {0x0000043c, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x181f14c4, 0x03e2b958, VTM_NATIVE  , 0, Class_cm_superClass},
  {0x2c296348, 0x03e2b958, VTM_NATIVE  , 2, Class_cm_subClass_body_},
  {0x34003578, 0x03e2b958, VTM_BYTECODE, 4, {.offset = 0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x54aa30e6, 0x03e2b958, VTM_NATIVE  , 0, Class_cm_newObject},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
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
	meat::Reference self = CONTEXT(context).get_self();
  meat::Reference index = CONTEXT(context).get_param(0);

	return CONTEXT(self).get_local(INTEGER(index));
}

// method localVariables
static meat::Reference Context_om_localVariables(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();

	return new meat::Value(CONTEXT(self).get_num_of_locals() - 3);
}

// method repeat:
static meat::Reference Context_om_repeat_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference block = CONTEXT(context).get_param(0);

	CONTEXT(block).set_messenger(context);

	for (;;) {
		BLOCK(block).set_break_trap();
		BLOCK(block).set_continue_trap();
		execute(block);

		if (BLOCK(block).break_called() or CONTEXT(block).is_done())
			break;

		BLOCK(block).reset();
	}
	BLOCK(block).reset();

	return null;
}


// method messenger
static meat::Reference Context_om_messenger(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();

	return CONTEXT(self).get_messenger();
}

// method return
static meat::Reference Context_om_return(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();

	CONTEXT(self).finish();
	return null;
}

// method return:
static meat::Reference Context_om_return_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference value = CONTEXT(context).get_param(0);

	CONTEXT(self).set_result(value); // Set the return value
	CONTEXT(self).finish();          // Tell the context it's done.
	return null;
}

// method setLocal:to:
static meat::Reference Context_om_setLocal_to_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference index = CONTEXT(context).get_param(0);
  meat::Reference value = CONTEXT(context).get_param(1);

  CONTEXT(self).set_local(INTEGER(index), value);
  return null;
}

static meat::vtable_entry_t ContextMethods[] = {
  {0x0000043c, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x000007a0, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x00019850, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x00368f3a, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x00379f78, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x059a58ff, 0x1befcdac, VTM_BYTECODE, 4, {.offset = 0}},
  {0x23add62f, 0x1befcdac, VTM_NATIVE,   1, Context_om_getLocal_},
  {0x2a67696c, 0x1befcdac, VTM_NATIVE,   0, Context_om_messenger},
  {0x34003578, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x39a68c12, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
	{0x4139862f, 0x1befcdac, VTM_NATIVE,   1, Context_om_repeat_},
  {0x4179693a, 0x1befcdac, VTM_NATIVE,   1, Context_om_return_},
  {0x47206ce4, 0x1befcdac, VTM_NATIVE,   0, Context_om_localVariables},
  {0x484e3d31, 0x1befcdac, VTM_NATIVE,   0, Context_om_return},
  {0x5e3131ca, 0x1befcdac, VTM_BYTECODE, 4, {.offset = 1}},
  {0x675bde74, 0x1befcdac, VTM_NATIVE,   2, Context_om_setLocal_to_},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x7b840562, 0x00000000, VTM_SUPER,    0, {.offset = 0}}
};

static meat::vtable_entry_t ContextCMethods[] = {
  {0x0000043c, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x000007a0, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x00019850, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x00368f3a, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x2c296348, 0x1befcdac, VTM_BYTECODE, 8, {.offset = 72}},
  {0x39a68c12, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x54aa30e6, 0x1befcdac, VTM_BYTECODE, 7, {.offset = 2}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER,    0, {.offset = 0}}
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
  meat::Reference self = CONTEXT(context).get_self();

  BLOCK(self).c_break();
	if (not BLOCK(self).break_trap_set()) {
		meat::Reference init_ctx = BLOCK(self).get_origin();
		meat::Reference up_context = message(init_ctx, "break", context);
    execute(up_context);
	}
  return null;
}

// method continue
static meat::Reference BlockContext_om_continue(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();

  BLOCK(self).c_continue();
	if (not BLOCK(self).continue_trap_set()) {
		meat::Reference init_ctx = BLOCK(self).get_origin();
		meat::Reference up_context = message(init_ctx, "continue", context);
    execute(up_context);
	}
  return null;
}

// method execute
static meat::Reference BlockContext_om_execute(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();

	BLOCK(self).set_messenger(context);
	execute(self);
	BLOCK(self).reset();

	return null;
}

// method executeOnBreak:
static meat::Reference
BlockContext_om_executeOnBreak_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference breakBlock = CONTEXT(context).get_param(0);

	BLOCK(self).set_break_trap();
	BLOCK(self).set_messenger(context);
	execute(self);

	if (BLOCK(self).break_called()) {
		meat::Reference up_context = message(breakBlock, "execute", context);
		execute(up_context);
	}

	BLOCK(self).reset();

	return null;
}

// method executeOnBreak:onContinue:
static meat::Reference
BlockContext_om_executeOnBreak_onContinue_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference breakBlock = CONTEXT(context).get_param(0);
  meat::Reference continueBlock = CONTEXT(context).get_param(1);

	BLOCK(self).set_break_trap();
	BLOCK(self).set_continue_trap();
	BLOCK(self).set_messenger(context);
	execute(self);

	if (BLOCK(self).break_called()) {
		meat::Reference up_context = message(breakBlock, "execute", context);
		execute(up_context);
	} else if (BLOCK(self).continue_called()) {
		meat::Reference up_context = message(continueBlock, "execute", context);
		execute(up_context);
	}

	BLOCK(self).reset();

	return null;
}

// method executeOnContinue:
static meat::Reference BlockContext_om_executeOnContinue_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference continueBlock = CONTEXT(context).get_param(0);

	BLOCK(self).set_continue_trap();
	BLOCK(self).set_messenger(context);
	execute(self);

	if (BLOCK(self).continue_called()) {
		meat::Reference up_context = message(continueBlock, "execute", context);
		execute(up_context);
	}

	BLOCK(self).reset();

	return null;
}

// method return
static meat::Reference BlockContext_om_return(meat::Reference context) {
	meat::Reference self = CONTEXT(context).get_self();

	// We need to get the actual context to message the return method from.
	meat::Reference origin = BLOCK(self).get_origin();
	meat::Reference up_context = message(origin, "return", context);
	execute(up_context);

	CONTEXT(self).finish(); // Tell the context it's done.

	return null;
}

// method return:
static meat::Reference BlockContext_om_return_(meat::Reference context) {
	meat::Reference self = CONTEXT(context).get_self();
	meat::Reference value = CONTEXT(context).get_param(0);

	meat::Reference origin = BLOCK(self).get_origin();
	meat::Reference up_context = message(origin, "return:", context);
	CONTEXT(up_context).set_param(0, value);
	execute(up_context);

	CONTEXT(self).finish(); // Tell the context it's done.

	return null;
}

static meat::vtable_entry_t BlockContextMethods[] = {
  {0x0000043c, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x000007a0, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x00019850, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x00368f3a, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x00379f78, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x059a58ff, 0x46ba8a20, VTM_NATIVE, 0, BlockContext_om_break},
  {0x23add62f, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x290a0d52, 0x46ba8a20, VTM_NATIVE, 1, BlockContext_om_executeOnContinue_},
  {0x2a67696c, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x3158f7a0, 0x46ba8a20, VTM_NATIVE, 0, BlockContext_om_execute},
  {0x34003578, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x39a68c12, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
	{0x4139862f, 0x00000000, VTM_SUPER, 1, {.offset = 0}},
  {0x4179693a, 0x46ba8a20, VTM_NATIVE, 1, BlockContext_om_return_},
  {0x47206ce4, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x484e3d31, 0x46ba8a20, VTM_NATIVE, 0, BlockContext_om_return},
  {0x5a7f66e9, 0x46ba8a20, VTM_NATIVE, 1, BlockContext_om_executeOnBreak_},
  {0x5e3131ca, 0x46ba8a20, VTM_NATIVE, 0, BlockContext_om_continue},
  {0x675bde74, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x71828805, 0x46ba8a20, VTM_NATIVE, 2, BlockContext_om_executeOnBreak_onContinue_},
  {0x7a8e569a, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x7b840562, 0x00000000, VTM_SUPER,  0, {.offset = 0}}
};

static meat::vtable_entry_t BlockContextCMethods[] = {
  {0x0000043c, 0x00000000, VTM_SUPER, 0, {.offset = 0}},
  {0x000007a0, 0x00000000, VTM_SUPER, 0, {.offset = 0}},
  {0x00019850, 0x00000000, VTM_SUPER, 0, {.offset = 0}},
  {0x00368f3a, 0x00000000, VTM_SUPER, 0, {.offset = 0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER, 0, {.offset = 0}},
  {0x2c296348, 0x00000000, VTM_SUPER, 0, {.offset = 0}},
  {0x39a68c12, 0x00000000, VTM_SUPER, 0, {.offset = 0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER, 0, {.offset = 0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER, 0, {.offset = 0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER, 0, {.offset = 0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER, 0, {.offset = 0}}
};

/******************************************************************************
 * Null Class
 */

static meat::vtable_entry_t NullMethods[] = {
	{0x0000043c, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x000007a0, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x00368f3a, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x00379f78, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x34003578, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x39a6a1d2, 0x002539a7, VTM_BYTECODE, 6, {.offset = 0}}
};

static meat::vtable_entry_t NullCMethods[] = {
	{0x0000043c, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x000007a0, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x00368f3a, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x2c296348, 0x002539a7, VTM_BYTECODE, 9, {.offset = 113}},
  {0x39a6a1d2, 0x002539a7, VTM_BYTECODE, 6, {.offset = 23}},
  {0x54aa30e6, 0x002539a7, VTM_BYTECODE, 7, {.offset = 46}},
	{0x6b2d9a7a, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER,    0, {.offset = 0}}
};

static meat::uint8_t NullBytecode[] = {
	0x13, 0x04, 0x67, 0x14, 0x04, 0x24, 0x02, 0x04, 0x05, 0x00, 0x36, 0x75, 0x8e,
  0x00, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x05, 0x0b, 0x13, 0x04, 0x67,
  0x14, 0x04, 0x24, 0x02, 0x04, 0x05, 0x05, 0xcb, 0x19, 0x23, 0x00, 0x01, 0x02,
  0x41, 0x79, 0x69, 0x3a, 0x01, 0x05, 0x0b, 0x13, 0x04, 0x4f, 0xc2, 0x61, 0x66,
  0x16, 0x05, 0x43, 0x61, 0x6e, 0x6e, 0x6f, 0x74, 0x20, 0x63, 0x72, 0x65, 0x61,
  0x74, 0x65, 0x20, 0x61, 0x20, 0x6e, 0x65, 0x77, 0x20, 0x69, 0x6e, 0x74, 0x65,
  0x72, 0x6e, 0x61, 0x6c, 0x20, 0x4e, 0x75, 0x6c, 0x6c, 0x20, 0x6f, 0x62, 0x6a,
  0x65, 0x63, 0x74, 0x00, 0x02, 0x02, 0x06, 0x72, 0x79, 0x75, 0xfa, 0x00, 0x01,
  0x04, 0x6d, 0xb6, 0x8a, 0xb6, 0x02, 0x05, 0x06, 0x0b, 0x13, 0x06, 0x4f, 0xc2,
  0x61, 0x66, 0x16, 0x07, 0x43, 0x61, 0x6e, 0x6e, 0x6f, 0x74, 0x20, 0x73, 0x75,
  0x62, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x69, 0x6e,
  0x74, 0x65, 0x72, 0x6e, 0x61, 0x6c, 0x20, 0x4e, 0x75, 0x6c, 0x6c, 0x20, 0x63,
  0x6c, 0x61, 0x73, 0x73, 0x00, 0x02, 0x02, 0x08, 0x72, 0x79, 0x75, 0xfa, 0x00,
  0x01, 0x06, 0x6d, 0xb6, 0x8a, 0xb6, 0x02, 0x07, 0x08, 0x0b
};

/******************************************************************************
 * Exception Class
 */

static meat::Reference Exception_constructor(meat::Reference &klass,
																						 meat::uint8_t properties) {
  return new meat::Exception(klass, properties);
}

static meat::vtable_entry_t ExceptionMethods[] = {
	{0x0000043c, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x000007a0, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x00368f3a, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x00379f78, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x34003578, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x38b735d9, 0x4fc26166, VTM_BYTECODE, 5, {.offset = 0}},
  {0x38eb0035, 0x4fc26166, VTM_BYTECODE, 5, {.offset = 12}},
	{0x39a6a1d2, 0x00000000, VTM_SUPER,    0, {.offset = 0}}
};

// class method throw
static meat::Reference Exception_cm_throw(meat::Reference context) {
	meat::Reference ctx = CONTEXT(context).get_messenger();
	throw meat::Exception(null, ctx);
}

// class method throw:
static meat::Reference Exception_cm_throw_(meat::Reference context) {
  meat::Reference message = CONTEXT(context).get_param(0);
	meat::Reference ctx = CONTEXT(context).get_messenger();
	throw meat::Exception(message, ctx);
}

// class method throw:for:
static meat::Reference Exception_cm_throw_for_(meat::Reference context) {
  meat::Reference message = CONTEXT(context).get_param(0);
  meat::Reference ctx = CONTEXT(context).get_param(1);
	throw meat::Exception(message, ctx);
}

// class method try:
static meat::Reference Exception_cm_try_(meat::Reference context) {
  meat::Reference block = CONTEXT(context).get_param(0);

	CONTEXT(block).set_messenger(context);
	try {
		execute(block);
	} catch (meat::Exception &err) {
	} catch (...) {
	}
	return null;
}

// class method try:catch:
static meat::Reference Exception_cm_try_catch_(meat::Reference context) {
  meat::Reference try_block = CONTEXT(context).get_param(0);
  meat::Reference catch_block = CONTEXT(context).get_param(1);

	CONTEXT(try_block).set_messenger(context);
	CONTEXT(catch_block).set_messenger(context);
	try {
		execute(try_block);
	} catch (std::exception &err) {
		execute(catch_block);
	}
	return null;
}

// class method try:catch:do
static meat::Reference Exception_cm_try_catch_do_(meat::Reference context) {
  meat::Reference try_block = CONTEXT(context).get_param(0);
  meat::Reference error = CONTEXT(context).get_param(1);
  meat::Reference catch_block = CONTEXT(context).get_param(2);

	CONTEXT(try_block).set_messenger(context);
	CONTEXT(catch_block).set_messenger(context);
	try {
		execute(try_block);
	} catch (meat::Exception &err) {
		meat::Reference excp = new meat::Exception(err);
		CONTEXT(catch_block).set_local(INTEGER(error), excp);
		execute(catch_block);
	}
	return null;
}

static meat::vtable_entry_t ExceptionCMethods[] = {
	{0x0000043c, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x000007a0, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x003675df, 0x4fc26166, VTM_NATIVE, 1, Exception_cm_try_},
	{0x00368f3a, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x0693a6e6, 0x4fc26166, VTM_NATIVE, 0, Exception_cm_throw},
	{0x2c296348, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x4be13615, 0x4fc26166, VTM_NATIVE, 1, Exception_cm_throw_},
  {0x4e48308e, 0x4fc26166, VTM_NATIVE, 3, Exception_cm_try_catch_do_},
	{0x54aa30e6, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x6db68ab6, 0x4fc26166, VTM_NATIVE, 2, Exception_cm_throw_for_},
	{0x7a8e569a, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x7bebbd56, 0x4fc26166, VTM_NATIVE, 2, Exception_cm_try_catch_}
};

static meat::uint8_t ExceptionBytecode[] = {
  0x11, 0x04, 0x01, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x04, 0x0b, 0x11,
  0x04, 0x00, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x04, 0x0b
};

/******************************************************************************
 * Numeric Class
 */

static meat::vtable_entry_t NumericMethods[] = {
  {0x00000025, 0x6bfcb30e, VTM_BYTECODE, 7, {.offset = 37}},
  {0x0000002a, 0x6bfcb30e, VTM_BYTECODE, 7, {.offset = 73}},
  {0x0000002b, 0x6bfcb30e, VTM_BYTECODE, 7, {.offset = 109}},
  {0x0000002d, 0x6bfcb30e, VTM_BYTECODE, 7, {.offset = 145}},
  {0x0000002f, 0x6bfcb30e, VTM_BYTECODE, 7, {.offset = 181}},
  {0x0000003c, 0x6bfcb30e, VTM_BYTECODE, 7, {.offset = 217}},
  {0x0000003e, 0x6bfcb30e, VTM_BYTECODE, 7, {.offset = 327}},
  {0x0000005e, 0x6bfcb30e, VTM_BYTECODE, 7, {.offset = 400}},
  {0x0000043c, 0x6bfcb30e, VTM_BYTECODE, 7, {.offset = 0}},
  {0x00000781, 0x6bfcb30e, VTM_BYTECODE, 7, {.offset = 253}},
  {0x000007a0, 0x6bfcb30e, VTM_BYTECODE, 7, {.offset = 290}},
  {0x000007bf, 0x6bfcb30e, VTM_BYTECODE, 7, {.offset = 363}},
  {0x00017872, 0x6bfcb30e, VTM_BYTECODE, 6, {.offset = 436}},
  {0x0001a990, 0x6bfcb30e, VTM_BYTECODE, 6, {.offset = 531}},
	{0x00368f3a, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x00379f78, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x2c025c80, 0x6bfcb30e, VTM_BYTECODE, 6, {.offset = 474}},
	{0x34003578, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER,    0, {.offset = 0}}
};

// class method acos:
static meat::Reference Numeric_cm_acos_(meat::Reference context) {
  meat::Reference value = CONTEXT(context).get_param(0);

	return
		new meat::Value((meat::float_t)std::acos(FLOAT(value)));
}

// class method asin:
static meat::Reference Numeric_cm_asin_(meat::Reference context) {
  meat::Reference value = CONTEXT(context).get_param(0);

	return
		new meat::Value((meat::float_t)std::asin(FLOAT(value)));
}

// class method atan2y:x:
static meat::Reference Numeric_cm_atan2y_x_(meat::Reference context) {
  meat::Reference y = CONTEXT(context).get_param(0);
  meat::Reference x = CONTEXT(context).get_param(1);

	return new meat::Value((meat::float_t)std::atan2(FLOAT(y), FLOAT(x)));
}

// class method atan:
static meat::Reference Numeric_cm_atan_(meat::Reference context) {
  meat::Reference value = CONTEXT(context).get_param(0);

	return new meat::Value((meat::float_t)std::atan(FLOAT(value)));
}

// class method cos:
static meat::Reference Numeric_cm_cos_(meat::Reference context) {
  meat::Reference value = CONTEXT(context).get_param(0);

	return new meat::Value((meat::float_t)std::cos(FLOAT(value)));
}

// class method cosh:
static meat::Reference Numeric_cm_cosh_(meat::Reference context) {
  meat::Reference value = CONTEXT(context).get_param(0);

  return new meat::Value((meat::float_t)std::cosh(FLOAT(value)));
}

// class method exp:
static meat::Reference Numeric_cm_exp_(meat::Reference context) {
  meat::Reference value = CONTEXT(context).get_param(0);

  meat::float_t answer = (meat::float_t)std::exp(FLOAT(value));
  if (answer == HUGE_VAL) throw meat::Exception("exp range error");
  return new meat::Value(answer);
}

// class method log10:
static meat::Reference Numeric_cm_log10_(meat::Reference context) {
  meat::Reference value = CONTEXT(context).get_param(0);

	if (FLOAT(value) <= 0.0)
		throw meat::Exception("Log10 parameter out of range");
	return new meat::Value((meat::float_t)std::log10(FLOAT(value)));
}

// class method log:
static meat::Reference Numeric_cm_log_(meat::Reference context) {
  meat::Reference value = CONTEXT(context).get_param(0);

	if (FLOAT(value) <= 0.0)
		throw meat::Exception("Log parameter out of range");
	return new meat::Value((meat::float_t)std::log(FLOAT(value)));
}

// class method sin:
static meat::Reference Numeric_cm_sin_(meat::Reference context) {
  meat::Reference value = CONTEXT(context).get_param(0);

	return new meat::Value((meat::float_t)std::sin(FLOAT(value)));
}

// class method sinh:
static meat::Reference Numeric_cm_sinh_(meat::Reference context) {
  meat::Reference value = CONTEXT(context).get_param(0);

	return new meat::Value((meat::float_t)std::sinh(FLOAT(value)));
}

// class method sqrt:
static meat::Reference Numeric_cm_sqrt_(meat::Reference context) {
  meat::Reference value = CONTEXT(context).get_param(0);

	if (FLOAT(value) < 0.0)
		throw meat::Exception("Square root parameter out of range");
	return new meat::Value((meat::float_t)std::sqrt(FLOAT(value)));
}

// class method tan:
static meat::Reference Numeric_cm_tan_(meat::Reference context) {
  meat::Reference value = CONTEXT(context).get_param(0);

  return new meat::Value((meat::float_t)std::tan(FLOAT(value)));
}

// class method tanh:
static meat::Reference Numeric_cm_tanh_(meat::Reference context) {
  meat::Reference value = CONTEXT(context).get_param(0);

  return new meat::Value((meat::float_t)std::tanh(FLOAT(value)));
}

static meat::vtable_entry_t NumericCMethods[] = {
	{0x0000043c, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x000007a0, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x002eaf93, 0x6bfcb30e, VTM_NATIVE, 1, Numeric_cm_cos_},
  {0x002fb9bd, 0x6bfcb30e, VTM_NATIVE, 1, Numeric_cm_exp_},
  {0x0032c576, 0x6bfcb30e, VTM_NATIVE, 1, Numeric_cm_log_},
  {0x0035de62, 0x6bfcb30e, VTM_NATIVE, 1, Numeric_cm_sin_},
  {0x003634b9, 0x6bfcb30e, VTM_NATIVE, 1, Numeric_cm_tan_},
	{0x00368f3a, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x05859774, 0x6bfcb30e, VTM_NATIVE, 1, Numeric_cm_acos_},
  {0x058cc643, 0x6bfcb30e, VTM_NATIVE, 1, Numeric_cm_asin_},
  {0x058d1c9a, 0x6bfcb30e, VTM_NATIVE, 1, Numeric_cm_atan_},
  {0x05a74899, 0x6bfcb30e, VTM_NATIVE, 1, Numeric_cm_cosh_},
  {0x0685f3aa, 0x6bfcb30e, VTM_NATIVE, 1, Numeric_cm_sinh_},
  {0x0689a71a, 0x6bfcb30e, VTM_NATIVE, 1, Numeric_cm_sqrt_},
	{0x068b6f7b, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x06906833, 0x6bfcb30e, VTM_NATIVE, 1, Numeric_cm_tanh_},
  {0x09e10969, 0x6bfcb30e, VTM_NATIVE, 2, Numeric_cm_atan2y_x_},
	{0x2c296348, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x3e972438, 0x6bfcb30e, VTM_NATIVE, 1, Numeric_cm_log10_},
	{0x54aa30e6, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER,  0, {.offset = 0}}
};

static meat::uint8_t NumericBytecode[] = {
  0x13, 0x05, 0x4f, 0xc2, 0x61, 0x66, 0x16, 0x06, 0x4d, 0x65, 0x74, 0x68, 0x6f,
  0x64, 0x20, 0x21, 0x3d, 0x20, 0x75, 0x6e, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65,
  0x64, 0x00, 0x01, 0x05, 0x4b, 0xe1, 0x36, 0x15, 0x01, 0x06, 0x0b, 0x13, 0x05,
  0x4f, 0xc2, 0x61, 0x66, 0x16, 0x06, 0x4d, 0x65, 0x74, 0x68, 0x6f, 0x64, 0x20,
  0x25, 0x20, 0x75, 0x6e, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x64, 0x00, 0x01,
  0x05, 0x4b, 0xe1, 0x36, 0x15, 0x01, 0x06, 0x0b, 0x13, 0x05, 0x4f, 0xc2, 0x61,
  0x66, 0x16, 0x06, 0x4d, 0x65, 0x74, 0x68, 0x6f, 0x64, 0x20, 0x2a, 0x20, 0x75,
  0x6e, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x64, 0x00, 0x01, 0x05, 0x4b, 0xe1,
  0x36, 0x15, 0x01, 0x06, 0x0b, 0x13, 0x05, 0x4f, 0xc2, 0x61, 0x66, 0x16, 0x06,
  0x4d, 0x65, 0x74, 0x68, 0x6f, 0x64, 0x20, 0x2b, 0x20, 0x75, 0x6e, 0x64, 0x65,
  0x66, 0x69, 0x6e, 0x65, 0x64, 0x00, 0x01, 0x05, 0x4b, 0xe1, 0x36, 0x15, 0x01,
  0x06, 0x0b, 0x13, 0x05, 0x4f, 0xc2, 0x61, 0x66, 0x16, 0x06, 0x4d, 0x65, 0x74,
  0x68, 0x6f, 0x64, 0x20, 0x2d, 0x20, 0x75, 0x6e, 0x64, 0x65, 0x66, 0x69, 0x6e,
  0x65, 0x64, 0x00, 0x01, 0x05, 0x4b, 0xe1, 0x36, 0x15, 0x01, 0x06, 0x0b, 0x13,
  0x05, 0x4f, 0xc2, 0x61, 0x66, 0x16, 0x06, 0x4d, 0x65, 0x74, 0x68, 0x6f, 0x64,
  0x20, 0x2f, 0x20, 0x75, 0x6e, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x64, 0x00,
  0x01, 0x05, 0x4b, 0xe1, 0x36, 0x15, 0x01, 0x06, 0x0b, 0x13, 0x05, 0x4f, 0xc2,
  0x61, 0x66, 0x16, 0x06, 0x4d, 0x65, 0x74, 0x68, 0x6f, 0x64, 0x20, 0x3c, 0x20,
  0x75, 0x6e, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x64, 0x00, 0x01, 0x05, 0x4b,
  0xe1, 0x36, 0x15, 0x01, 0x06, 0x0b, 0x13, 0x05, 0x4f, 0xc2, 0x61, 0x66, 0x16,
  0x06, 0x4d, 0x65, 0x74, 0x68, 0x6f, 0x64, 0x20, 0x3c, 0x3d, 0x20, 0x75, 0x6e,
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
  0x16, 0x05, 0x4d, 0x65, 0x74, 0x68, 0x6f, 0x64, 0x20, 0x61, 0x62, 0x73, 0x20,
  0x75, 0x6e, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x64, 0x00, 0x01, 0x04, 0x4b,
  0xe1, 0x36, 0x15, 0x01, 0x05, 0x0b, 0x13, 0x04, 0x4f, 0xc2, 0x61, 0x66, 0x16,
  0x05, 0x55, 0x6e, 0x61, 0x62, 0x6c, 0x65, 0x20, 0x74, 0x6f, 0x20, 0x63, 0x6f,
  0x6e, 0x76, 0x65, 0x72, 0x74, 0x20, 0x6e, 0x75, 0x6d, 0x65, 0x72, 0x69, 0x63,
  0x20, 0x76, 0x61, 0x6c, 0x75, 0x65, 0x20, 0x74, 0x6f, 0x20, 0x54, 0x65, 0x78,
  0x74, 0x00, 0x01, 0x04, 0x4b, 0xe1, 0x36, 0x15, 0x01, 0x05, 0x0b, 0x13, 0x04,
  0x4f, 0xc2, 0x61, 0x66, 0x16, 0x05, 0x4d, 0x65, 0x74, 0x68, 0x6f, 0x64, 0x20,
  0x6e, 0x65, 0x67, 0x20, 0x75, 0x6e, 0x64, 0x65, 0x66, 0x69, 0x6e, 0x65, 0x64,
  0x00, 0x01, 0x04, 0x4b, 0xe1, 0x36, 0x15, 0x01, 0x05, 0x0b
};

/******************************************************************************
 * Integer Class
 */

static meat::Reference Integer_constructor(meat::Reference &klass,
																					 meat::uint8_t properties) {
  return new meat::Value(klass, properties);
}

// method !=
static meat::Reference Integer_om_nequals(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

  try {
    if (INTEGER(self) != INTEGER(other))
      return meat::BTrue();
  } catch (...) {}
  return meat::BFalse();
}

// method %
static meat::Reference Integer_om_mod(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

  return new meat::Value(INTEGER(self) % INTEGER(other));
}

// method *
static meat::Reference Integer_om_mult(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

	int32_t res;
	if (__builtin_mul_overflow(INTEGER(self), INTEGER(other), &res))
		throw meat::Exception("Integer multiplication overflow");
  return new meat::Value(res);
}

// method +
static meat::Reference Integer_om_add(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

	int32_t res;
	if (__builtin_add_overflow(INTEGER(self), INTEGER(other), &res))
		throw meat::Exception("Integer addition overflow");
  return new meat::Value(res);
}

// method -
static meat::Reference Integer_om_sub(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

	int32_t res;
	if (__builtin_sub_overflow(INTEGER(self), INTEGER(other), &res))
		throw meat::Exception("Integer subtraction underflow");
  return new meat::Value(res);
}

// method /
static meat::Reference Integer_om_div(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

  return new meat::Value(INTEGER(self) / INTEGER(other));
}

// method <
static meat::Reference Integer_om_less(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

  try {
    if (INTEGER(self) < INTEGER(other))
      return meat::BTrue();
  } catch (...) {}
  return meat::BFalse();
}

// method <=
static meat::Reference Integer_om_less_equal(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

  try {
    if (INTEGER(self) <= INTEGER(other))
      return meat::BTrue();
  } catch (...) {}
  return meat::BFalse();
}

// method ==
static meat::Reference Integer_om_equals(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

  try {
    if (INTEGER(self) == INTEGER(other))
      return meat::BTrue();
  } catch (...) {}
  return meat::BFalse();
}

// method >
static meat::Reference Integer_om_greater(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

  try {
    if (INTEGER(self) > INTEGER(other))
      return meat::BTrue();
  } catch (...) {}
  return meat::BFalse();
}

// method >=
static meat::Reference Integer_om_greater_equal(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

  try {
    if (INTEGER(self) >= INTEGER(other))
      return meat::BTrue();
  } catch (...) {}
  return meat::BFalse();
}

// method ^
static meat::Reference Integer_om_pow(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

  return new meat::Value((meat::float_t)std::pow(FLOAT(self),
																								 FLOAT(other)));
}

// method and:
static meat::Reference Integer_om_and_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

  return new meat::Value(INTEGER(self) & INTEGER(other));
}

// method asText
static meat::Reference Integer_om_asText(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();

  std::stringstream result;
  result << INTEGER(self);
  return new meat::Text(result.str());
}

// method lshift
static meat::Reference Integer_om_lshift(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();

  return new meat::Value(INTEGER(self) << 1);
}

// method lshift:
static meat::Reference Integer_om_lshift_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference amount = CONTEXT(context).get_param(0);

  return new meat::Value(INTEGER(self) << INTEGER(amount));
}

// method neg
static meat::Reference Integer_om_neg(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();

  return new meat::Value(-INTEGER(self));
}

// method abs
static meat::Reference Integer_om_abs(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();

  return new meat::Value(std::abs(INTEGER(self)));
}

// method or:
static meat::Reference Integer_om_or_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

  return new meat::Value(INTEGER(self) | INTEGER(other));
}

// method rshift
static meat::Reference Integer_om_rshift(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();

  return new meat::Value(INTEGER(self) >> 1);
}

// method rshift:
static meat::Reference Integer_om_rshift_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference amount = CONTEXT(context).get_param(0);

  return new meat::Value(INTEGER(self) >> INTEGER(amount));
}

// method timesDo:
static meat::Reference Integer_om_timesDo_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference block = CONTEXT(context).get_param(0);

  meat::int32_t limit = INTEGER(self);
  for (meat::int32_t c = 0; c < limit; c++) {
    meat::execute(block);
    BLOCK(block).reset();
  }
  return null;
}

// method xor:
static meat::Reference Integer_om_xor_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

  return new meat::Value(INTEGER(self) ^ INTEGER(other));
}

static meat::vtable_entry_t IntegerMethods[] = {
  {0x00000025, 0x57ee193d, VTM_NATIVE, 1, Integer_om_mod},
  {0x0000002a, 0x57ee193d, VTM_NATIVE, 1, Integer_om_mult},
  {0x0000002b, 0x57ee193d, VTM_NATIVE, 1, Integer_om_add},
  {0x0000002d, 0x57ee193d, VTM_NATIVE, 1, Integer_om_sub},
  {0x0000002f, 0x57ee193d, VTM_NATIVE, 1, Integer_om_div},
  {0x0000003c, 0x57ee193d, VTM_NATIVE, 1, Integer_om_less},
  {0x0000003e, 0x57ee193d, VTM_NATIVE, 1, Integer_om_greater},
  {0x0000005e, 0x57ee193d, VTM_NATIVE, 1, Integer_om_pow},
  {0x0000043c, 0x57ee193d, VTM_NATIVE, 1, Integer_om_nequals},
  {0x00000781, 0x57ee193d, VTM_NATIVE, 1, Integer_om_less_equal},
  {0x000007a0, 0x57ee193d, VTM_NATIVE, 1, Integer_om_equals},
  {0x000007bf, 0x57ee193d, VTM_NATIVE, 1, Integer_om_greater_equal},
  {0x00017872, 0x57ee193d, VTM_NATIVE, 0, Integer_om_abs},
  {0x0001a990, 0x57ee193d, VTM_NATIVE, 0, Integer_om_neg},
  {0x0001aeb7, 0x57ee193d, VTM_NATIVE, 1, Integer_om_or_},
  {0x002dc143, 0x57ee193d, VTM_NATIVE, 1, Integer_om_and_},
  {0x00383b3f, 0x57ee193d, VTM_NATIVE, 1, Integer_om_xor_},
  {0x043b7de4, 0x57ee193d, VTM_NATIVE, 1, Integer_om_timesDo_},
  {0x1b492392, 0x57ee193d, VTM_NATIVE, 1, Integer_om_lshift_},
  {0x2c025c80, 0x57ee193d, VTM_NATIVE, 0, Integer_om_asText},
  {0x3ed0cf97, 0x57ee193d, VTM_NATIVE, 0, Integer_om_lshift},
  {0x490de351, 0x57ee193d, VTM_NATIVE, 0, Integer_om_rshift},
  {0x58ae871a, 0x57ee193d, VTM_NATIVE, 1, Integer_om_rshift_}
};

/******************************************************************************
 * Number Class
 */

static meat::Reference Number_constructor(meat::Reference &klass,
																					meat::uint8_t properties) {
  return new meat::Value(klass, properties);
}

// method !=
static meat::Reference Number_cm_nequal(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

	try {
		if (FLOAT(self) != FLOAT(other))
			return meat::BTrue();
	} catch (...) {}
	return null;
}

// method %
static meat::Reference Number_cm_mod(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

	return new meat::Value(std::fmod(FLOAT(self), FLOAT(other)));
}

// method *
static meat::Reference Number_cm_mult(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

	return new meat::Value(FLOAT(self) * FLOAT(other));
}

// method +
static meat::Reference Number_cm_add(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

	return new meat::Value(FLOAT(self) + FLOAT(other));
}

// method -
static meat::Reference Number_cm_sub(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

	return new meat::Value(FLOAT(self) - FLOAT(other));
}

// method /
static meat::Reference Number_cm_div(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

  if (FLOAT(other) == 0.0)
    throw meat::Exception("Divide by zero");

	return new meat::Value(FLOAT(self) / FLOAT(other));
}

// method <
static meat::Reference Number_cm_less(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

	try {
		if (FLOAT(self) < FLOAT(other))
			return meat::BTrue();
	} catch (...) {}
	return meat::BFalse();
}

// method <=
static meat::Reference Number_cm_less_equal(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

	try {
		if (FLOAT(self) <= FLOAT(other))
			return meat::BTrue();
	} catch (...) {}
	return meat::BFalse();
}

// method ==
static meat::Reference Number_cm_equal(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
	meat::Reference other = CONTEXT(context).get_param(0);

	try {
		if (FLOAT(self) == FLOAT(other))
			return meat::BTrue();
	} catch (...) {}
	return meat::BFalse();
}

// method >
static meat::Reference Number_cm_greater(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

	try {
		if (FLOAT(self) > FLOAT(other))
			return meat::BTrue();
	} catch (...) {}
	return meat::BFalse();
}

// method >=
static meat::Reference Number_cm_greater_equal(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

	try {
		if (FLOAT(self) >= FLOAT(other))
			return meat::BTrue();
	} catch (...) {}
	return meat::BFalse();
}

// method ^
static meat::Reference Number_cm_pow(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

	return new meat::Value((meat::float_t)std::pow(FLOAT(self),
																								 FLOAT(other)));
}

// method abs
static meat::Reference Number_cm_abs(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();

	return new meat::Value(std::abs(FLOAT(self)));
}

// method asText
static meat::Reference Number_cm_asText(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();

	std::stringstream result;
	result << (double)FLOAT(self);
	return new meat::Text(result.str());
}

// method neg
static meat::Reference Number_cm_neg(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();

	return new meat::Value(-FLOAT(self));
}

static meat::vtable_entry_t NumberMethods[] = {
  {0x00000025, 0x0bbdc76a, VTM_NATIVE, 1, Number_cm_mod},
  {0x0000002a, 0x0bbdc76a, VTM_NATIVE, 1, Number_cm_mult},
  {0x0000002b, 0x0bbdc76a, VTM_NATIVE, 1, Number_cm_add},
  {0x0000002d, 0x0bbdc76a, VTM_NATIVE, 1, Number_cm_sub},
  {0x0000002f, 0x0bbdc76a, VTM_NATIVE, 1, Number_cm_div},
  {0x0000003c, 0x0bbdc76a, VTM_NATIVE, 1, Number_cm_less},
  {0x0000003e, 0x0bbdc76a, VTM_NATIVE, 1, Number_cm_greater},
  {0x0000005e, 0x0bbdc76a, VTM_NATIVE, 1, Number_cm_pow},
  {0x0000043c, 0x0bbdc76a, VTM_NATIVE, 1, Number_cm_nequal},
  {0x00000781, 0x0bbdc76a, VTM_NATIVE, 1, Number_cm_less_equal},
  {0x000007a0, 0x0bbdc76a, VTM_NATIVE, 1, Number_cm_equal},
  {0x000007bf, 0x0bbdc76a, VTM_NATIVE, 1, Number_cm_greater_equal},
  {0x00017872, 0x0bbdc76a, VTM_NATIVE, 0, Number_cm_abs},
  {0x0001a990, 0x0bbdc76a, VTM_NATIVE, 0, Number_cm_neg},
  {0x2c025c80, 0x0bbdc76a, VTM_NATIVE, 0, Number_cm_asText}
};

static meat::vtable_entry_t NumberCMethods[] = {
  {0x0000043c, 0x00000000, VTM_SUPER, 0, {.offset = 0}},
  {0x000007a0, 0x00000000, VTM_SUPER, 0, {.offset = 0}},
  {0x002eaf93, 0x00000000, VTM_SUPER, 0, {.offset = 0}},
  {0x002fb9bd, 0x00000000, VTM_SUPER, 0, {.offset = 0}},
  {0x0032c576, 0x00000000, VTM_SUPER, 0, {.offset = 0}},
  {0x0035de62, 0x00000000, VTM_SUPER, 0, {.offset = 0}},
  {0x003634b9, 0x00000000, VTM_SUPER, 0, {.offset = 0}},
  {0x00368f3a, 0x00000000, VTM_SUPER, 0, {.offset = 0}},
  {0x05859774, 0x00000000, VTM_SUPER, 0, {.offset = 0}},
  {0x058cc643, 0x00000000, VTM_SUPER, 0, {.offset = 0}},
  {0x058d1c9a, 0x00000000, VTM_SUPER, 0, {.offset = 0}},
  {0x05a74899, 0x00000000, VTM_SUPER, 0, {.offset = 0}},
  {0x0685f3aa, 0x00000000, VTM_SUPER, 0, {.offset = 0}},
  {0x0689a71a, 0x00000000, VTM_SUPER, 0, {.offset = 0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER, 0, {.offset = 0}},
  {0x06906833, 0x00000000, VTM_SUPER, 0, {.offset = 0}},
  {0x09e10969, 0x00000000, VTM_SUPER, 0, {.offset = 0}},
  {0x2c296348, 0x00000000, VTM_SUPER, 0, {.offset = 0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER, 0, {.offset = 0}}
};

/******************************************************************************
 * Boolean Class
 */

#define trueObject (klass->property(0))
#define falseObject (klass->property(1))

// method and:
static meat::Reference Boolean_om_and_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference klass = CONTEXT(context).get_class();
  meat::Reference other = CONTEXT(context).get_param(0);

	return ((self == trueObject and other == trueObject) ?
					trueObject : falseObject);
}

// method isFalse:
static meat::Reference Boolean_om_isFalse_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference klass = CONTEXT(context).get_class();
  meat::Reference block = CONTEXT(context).get_param(0);

	if (self == falseObject) execute(block);
	return null;
}

// method isFalse:else:
static meat::Reference Boolean_om_isFalse_else_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference klass = CONTEXT(context).get_class();
  meat::Reference true_block = CONTEXT(context).get_param(0);
  meat::Reference else_block = CONTEXT(context).get_param(1);

	if (self == falseObject) {
		execute(true_block);
	} else {
		execute(else_block);
	}
	return null;
}

// method isTrue:
static meat::Reference Boolean_om_isTrue_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference klass = CONTEXT(context).get_class();
  meat::Reference block = CONTEXT(context).get_param(0);

	if (self == trueObject) execute(block);
	return null;
}

// method isTrue:else:
static meat::Reference Boolean_om_isTrue_else_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference klass = CONTEXT(context).get_class();
  meat::Reference true_block = CONTEXT(context).get_param(0);
  meat::Reference else_block = CONTEXT(context).get_param(1);

	if (self == trueObject) {
		execute(true_block);
	} else {
		execute(else_block);
	}
	return null;
}

// method or:
static meat::Reference Boolean_om_or_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference klass = CONTEXT(context).get_class();
  meat::Reference other = CONTEXT(context).get_param(0);

	return ((self == trueObject or other == trueObject) ?
					trueObject : falseObject);
}

// method xor:
static meat::Reference Boolean_om_xor_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference klass = CONTEXT(context).get_class();
  meat::Reference other = CONTEXT(context).get_param(0);

	return (((self == trueObject) xor (other == trueObject)) ?
					trueObject : falseObject);
}

static meat::vtable_entry_t BooleanMethods[] = {
  {0x0000043c, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x000007a0, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x0001aad3, 0x67140424, VTM_BYTECODE, 8, {.offset = 65}},
  {0x0001aeb7, 0x67140424, VTM_NATIVE,   1, Boolean_om_or_},
  {0x002dc143, 0x67140424, VTM_NATIVE,   1, Boolean_om_and_},
	{0x00368f3a, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x00379f78, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x00383b3f, 0x67140424, VTM_NATIVE,   1, Boolean_om_xor_},
  {0x2c025c80, 0x67140424, VTM_BYTECODE, 8, {.offset = 0}},
	{0x34003578, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x47607971, 0x67140424, VTM_NATIVE,   2, Boolean_om_isTrue_else_},
  {0x57c628fe, 0x67140424, VTM_NATIVE,   2, Boolean_om_isFalse_else_},
  {0x5bc502b2, 0x67140424, VTM_NATIVE,   1, Boolean_om_isFalse_},
	{0x6b2d9a7a, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x7b80e98e, 0x67140424, VTM_NATIVE,   1, Boolean_om_isTrue_},
	{0x7b840562, 0x00000000, VTM_SUPER,    0, {.offset = 0}}
};

static meat::vtable_entry_t BooleanCMethods[] = {
	{0x0000043c, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x000007a0, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x0036758e, 0x67140424, VTM_BYTECODE, 5, {.offset = 133}},
	{0x00368f3a, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x05cb1923, 0x67140424, VTM_BYTECODE, 5, {.offset = 121}},
	{0x068b6f7b, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x2c296348, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER,    0, {.offset = 0}}
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
 * Text Class
 */

static meat::Reference Text_constructor(meat::Reference &klass,
																				meat::uint8_t properties) {
  return new meat::Text(klass, properties);
}

// method !=
static meat::Reference Text_om_nequal(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

	return (TEXT(self).compare(TEXT(other)) != 0 ?
					meat::BTrue() : meat::BFalse());
}

// method *
static meat::Reference Text_om_mult(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference count = CONTEXT(context).get_param(0);

  meat::int32_t cnt_value = INTEGER(count);
	meat::Text *result = new meat::Text();

  for (int c = 0; c < cnt_value; c++)
    (*result) += TEXT(self);

  return result;
}

// method +
static meat::Reference Text_om_add(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

	return new meat::Text(TEXT(self) + TEXT(other));
}

// method <
static meat::Reference Text_om_less(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

	return (TEXT(self).compare(TEXT(other)) < 0 ?
					meat::BTrue() : meat::BFalse());
}

// method <=
static meat::Reference Text_om_less_equal(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

	return (TEXT(self).compare(TEXT(other)) <= 0 ?
					meat::BTrue() : meat::BFalse());
}

// method ==
static meat::Reference Text_om_equal(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

	return (TEXT(self).compare(TEXT(other)) == 0 ?
					meat::BTrue() : meat::BFalse());
}

// method >
static meat::Reference Text_om_greater(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

	return (TEXT(self).compare(TEXT(other)) > 0 ?
					meat::BTrue() : meat::BFalse());
}

// method >=
static meat::Reference Text_om_greater_equal(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference other = CONTEXT(context).get_param(0);

	return (TEXT(self).compare(TEXT(other)) >= 0 ?
					meat::BTrue() : meat::BFalse());
}

// method get:
static meat::Reference Text_om_get_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference index = CONTEXT(context).get_param(0);

	meat::Text *new_string = new meat::Text();
  (*new_string) += TEXT(self)[INTEGER(index) - 1];
  return new_string;
}

// method get:to:
static meat::Reference Text_om_get_to_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference start = CONTEXT(context).get_param(0);
  meat::Reference end = CONTEXT(context).get_param(1);

  meat::int32_t start_pos = INTEGER(start) - 1;
  meat::int32_t len = INTEGER(end) - start_pos;

	meat::Text *new_string = new meat::Text();
  (*new_string) += TEXT(self).substr(start_pos, len);
  return new_string;
}

// method getCharAt:
static meat::Reference Text_om_getCharAt_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference index = CONTEXT(context).get_param(0);

	char ch = TEXT(self)[INTEGER(index) - 1];
	return new meat::Text(&ch);
}

// method isEmpty
static meat::Reference Text_om_isEmpty(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();

	return (TEXT(self).empty() ? meat::BTrue() : meat::BFalse());
}

// method length
static meat::Reference Text_om_length(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();

	unsigned int len = 0;
	meat::Text &str = TEXT(self);

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
	{0x00019850, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
	{0x00368f3a, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
	{0x2c025c80, 0x0027b94d, VTM_BYTECODE, 4, {.offset = 0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
	{0x39a68c12, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x3e0e3ae7, 0x0027b94d, VTM_NATIVE  , 0, Text_om_length},
	{0x69eddf35, 0x0027b94d, VTM_NATIVE  , 1, Text_om_getCharAt_},
	{0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x7aab326f, 0x0027b94d, VTM_NATIVE  , 0, Text_om_isEmpty},
  {0x7b7a1326, 0x0027b94d, VTM_NATIVE  , 2, Text_om_get_to_},
	{0x7b840562, 0x00000000, VTM_SUPER   , 0, {.offset = 0}}
};

static meat::vtable_entry_t TextCMethods[] = {
  {0x0000043c, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x2c296348, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {.offset = 0}}
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
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference value = CONTEXT(context).get_param(0);

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
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference index = CONTEXT(context).get_param(0);
  meat::Reference value = CONTEXT(context).get_param(1);

  LIST(self).insert(LIST(self).begin() + (INTEGER(index) - 1), value);
  return null;
}

// method clear
static meat::Reference List_cm_clear(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();

  LIST(self).clear();
  return null;
}

// method copy
static meat::Reference List_om_copy(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();

	meat::Reference new_list = new meat::List();
	LIST(new_list) = LIST(self);
	return new_list;
}

// method forEach:do:
static meat::Reference List_cm_forEach_do_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference item = CONTEXT(context).get_param(0);
  meat::Reference block = CONTEXT(context).get_param(1);

  meat::uint8_t local_id = INTEGER(item);
  CONTEXT(block).set_messenger(context);

  meat::List::iterator it = LIST(self).begin();
  for (; it != LIST(self).end(); it++) {
		BLOCK(block).set_break_trap();
    CONTEXT(block).set_local(local_id, *it);
    execute(block);

		if (BLOCK(block).break_called()) break;
		else BLOCK(block).reset();
  }
	BLOCK(block).reset();

  return null;
}

// method get:
static meat::Reference List_cm_get_(meat::Reference context) {
	meat::Reference self = CONTEXT(context).get_self();
	meat::Reference index = CONTEXT(context).get_param(0);

	try {
		return LIST(self).at(INTEGER(index) - 1);
	} catch (std::out_of_range &err) {
		throw meat::Exception("List index out of Range", context);
	}
}

// method isEmpty
static meat::Reference List_cm_isEmpty(meat::Reference context) {
	meat::Reference self = CONTEXT(context).get_self();

	return (LIST(self).empty() ? meat::BTrue() : meat::BFalse());
}

// method last
static meat::Reference List_cm_last(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();

	return LIST(self).back();
}

// method pop
static meat::Reference List_cm_pop(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();

	LIST(self).pop_back();
	return null;
}

// method push:
static meat::Reference List_cm_push_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference value = CONTEXT(context).get_param(0);

  LIST(self).push_back(value);
  return null;
}

// method remove:
static meat::Reference List_cm_remove_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference value = CONTEXT(context).get_param(0);

  meat::List::iterator it = ((meat::List &)(*self)).begin();
  while (it != LIST(self).end()) {
    if (*it == value)
      LIST(self).erase(it);
    it++;
  }
  return null;
}

// method removeAt:
static meat::Reference List_cm_removeAt_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference index = CONTEXT(context).get_param(0);

  LIST(self).erase(LIST(self).begin() + (INTEGER(index) - 1));
  return null;
}

// method removeFrom:to:
static meat::Reference List_cm_removeFrom_to_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference start = CONTEXT(context).get_param(0);
  meat::Reference end = CONTEXT(context).get_param(1);

  LIST(self).erase(LIST(self).begin() + (INTEGER(start) - 1),
									 LIST(self).begin() + (INTEGER(end) - 1));
  return null;
}

// method set:to:
static meat::Reference List_cm_set_to_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference index = CONTEXT(context).get_param(0);
  meat::Reference value = CONTEXT(context).get_param(1);

  try {
    LIST(self).at(INTEGER(index) - 1) = value;
  } catch (std::out_of_range &err) {
    throw meat::Exception("Index out of Range", context);
  }
  return null;
}

// method size
static meat::Reference List_cm_size(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference klass = CONTEXT(context).get_class();

  return new meat::Value((meat::int32_t)LIST(self).size());
}

// method swap:with:
static meat::Reference List_cm_swap_with_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference first = CONTEXT(context).get_param(0);
  meat::Reference second = CONTEXT(context).get_param(1);

  try {
    meat::Reference temp = LIST(self).at(INTEGER(first) - 1);
    LIST(self).at(INTEGER(first) - 1) = LIST(self).at(INTEGER(second) - 1);
    LIST(self).at(INTEGER(second) - 1) = temp;
  } catch (std::out_of_range &err) {
    throw meat::Exception("Index out of Range", context);
  }
  return null;
}

static meat::vtable_entry_t ListMethods[] = {
	{0x0000043c, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x0001b251, 0x002424be, VTM_NATIVE  , 0, List_cm_pop},
	{0x002eaf75, 0x002424be, VTM_NATIVE  , 0, List_om_copy},
  {0x00305ba4, 0x002424be, VTM_NATIVE  , 1, List_cm_get_},
  {0x00329296, 0x002424be, VTM_NATIVE  , 0, List_cm_last},
  {0x0035e001, 0x002424be, VTM_NATIVE  , 0, List_cm_size},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x018b0a25, 0x002424be, VTM_NATIVE  , 2, List_cm_forEach_do_},
  {0x05a5b64d, 0x002424be, VTM_NATIVE  , 0, List_cm_clear},
  {0x06613460, 0x002424be, VTM_NATIVE  , 1, List_cm_push_},
  {0x12b51eb5, 0x002424be, VTM_NATIVE  , 2, List_cm_removeFrom_to_},
  {0x13a72bf2, 0x002424be, VTM_NATIVE  , 2, List_cm_swap_with_},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x3c94d873, 0x002424be, VTM_NATIVE  , 2, List_cm_at_insert_},
  {0x41141866, 0x002424be, VTM_NATIVE  , 1, List_cm_remove_},
  {0x4c6fc15b, 0x002424be, VTM_NATIVE  , 1, List_cm_removeAt_},
  {0x50b51489, 0x002424be, VTM_NATIVE  , 1, List_cm_append_},
  {0x7644da37, 0x002424be, VTM_NATIVE  , 2, List_cm_set_to_},
  {0x7aab326f, 0x002424be, VTM_NATIVE  , 0, List_cm_isEmpty}
};

static meat::vtable_entry_t ListCMethods[] = {
	{0x0000043c, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x0001a9a0, 0x002424be, VTM_BYTECODE, 4, {.offset = 0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x2c296348, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {.offset = 0}}
};

static meat::uint8_t ListBytecode[] = {
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
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference key = CONTEXT(context).get_param(0);

  if (INDEX(self).find(key) != INDEX(self).end())
    return INDEX(self)[key];
  throw meat::Exception("Invalid key in index");
}

// method has:
static meat::Reference Index_om_has_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference key = CONTEXT(context).get_param(0);

  if (INDEX(self).find(key) != INDEX(self).end())
    return meat::BTrue();
  return meat::BFalse();
}

// method remove:
static meat::Reference Index_om_remove_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference key = CONTEXT(context).get_param(0);

  INDEX(self).erase(key);
  return null;
}

// method set:to:
static meat::Reference Index_om_set_to_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference key = CONTEXT(context).get_param(0);
  meat::Reference value = CONTEXT(context).get_param(1);

  INDEX(self)[key] = value;
  return null;
}

// method size
static meat::Reference Index_om_size(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();

  meat::int32_t size = (INDEX(self)).size();
  return new meat::Value(size);
}

static meat::vtable_entry_t IndexMethods[] = {
	{0x0000043c, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x000007a0, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x00305ba4, 0x043838b2, VTM_NATIVE, 1, Index_om_get_},
  {0x0030c0e0, 0x043838b2, VTM_NATIVE, 1, Index_om_has_},
  {0x0035e001, 0x043838b2, VTM_NATIVE, 0, Index_om_size},
	{0x00368f3a, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x00379f78, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x34003578, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x41141866, 0x043838b2, VTM_NATIVE, 1, Index_om_remove_},
  {0x7644da37, 0x043838b2, VTM_NATIVE, 2, Index_om_set_to_}
};

// class method new
static meat::Reference Index_cm_new(meat::Reference context) {
  return new meat::Index;
}

static meat::vtable_entry_t IndexCMethods[] = {
	{0x0000043c, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x000007a0, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x0001a9a0, 0x043838b2, VTM_NATIVE, 0, Index_cm_new},
	{0x00368f3a, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x2c296348, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER,  0, {.offset = 0}}
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
	{0x0000043c, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x000007a0, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x00368f3a, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x00379f78, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x05c30872, 0x419df72b, VTM_BYTECODE, 6, {.offset = 0}},
	{0x34003578, 0x00000000, VTM_SUPER,    0, {.offset = 0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER,    0, {.offset = 0}}
};

// class method parameters
static meat::Reference Application_cm_parameters(meat::Reference context) {
  return new meat::Value((meat::int32_t)arg_count());
}

// class method parameter: index
static meat::Reference Application_cm_parameter_(meat::Reference context) {
  meat::Reference index = CONTEXT(context).get_param(0);
  meat::int32_t idx = INTEGER(index);

  if (idx > arg_count() or idx <= 0)
    throw meat::Exception("Parameter index out of range");

  const char **argv = args();

  return new meat::Text(argv[idx - 1]);
}

// class method getEnviron: key
static meat::Reference Application_cm_getEnviron_(meat::Reference context) {
  meat::Reference key = CONTEXT(context).get_param(0);

#if defined(_WIN32) || defined(_WIN64)
  LPTSTR value = new TCHAR[4096];
  DWORD result;
  result = GetEnvironmentVariable(TEXT(key), value, 4096);
  if (result == 0) {
    delete[] value;
    return new meat::Text("");
  } else {
    meat::Reference retvalue = new meat::TEXT(value);
    delete[] value;
    return retvalue;
  }

#elif defined(__linux__)
  char *value = getenv(TEXT(key).c_str());
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
  meat::Reference key = CONTEXT(context).get_param(0);
  meat::Reference value = CONTEXT(context).get_param(1);

#if defined(_WIN32) || defined(_WIN64)
  SetEnvironmentVariable(TEXT(key).c_str(),
                         TEXT(value).c_str());
#elif defined(__linux__)
  setenv(TEXT(key).c_str(), TEXT(value).c_str(), 1);
#else
#  error("Don't know how to set the system environment")
#endif
  return null;
}

static meat::vtable_entry_t ApplicationCMethods[] = {
	{0x0000043c, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x000007a0, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x00368f3a, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x0462f057, 0x419df72b, VTM_NATIVE, 1, Application_cm_getEnviron_},
	{0x068b6f7b, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x1b6d6881, 0x419df72b, VTM_NATIVE, 1, Application_cm_parameter_},
  {0x1b6d68ba, 0x419df72b, VTM_NATIVE, 0, Application_cm_parameters},
	{0x2c296348, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x50b30a5d, 0x419df72b, VTM_NATIVE, 1, Application_cm_setEnviron_to_},
	{0x54aa30e6, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER,  0, {.offset = 0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER,  0, {.offset = 0}}
};

static meat::uint8_t ApplicationBytecode[] = {
  0x13, 0x04, 0x4f, 0xc2, 0x61, 0x66, 0x16, 0x05, 0x41, 0x70, 0x70, 0x6c, 0x69,
  0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x6d, 0x65, 0x74, 0x68, 0x6f, 0x64,
  0x20, 0x65, 0x6e, 0x74, 0x72, 0x79, 0x20, 0x6d, 0x75, 0x73, 0x74, 0x20, 0x62,
  0x65, 0x20, 0x6f, 0x76, 0x65, 0x72, 0x72, 0x69, 0x64, 0x64, 0x65, 0x6e, 0x00,
  0x01, 0x04, 0x4b, 0xe1, 0x36, 0x15, 0x01, 0x05, 0x0b
};

static meat::uint8_t Symbols[] = {
	"!=\0%\0*\0+\0-\0/\0<\0<=\0==\0>\0>=\0Application\0BlockContext\0"
  "Boolean\0Class\0Context\0Exception\0Index\0Integer\0"
  "Library\0List\0Null\0Number\0Numeric\0Object\0Set\0"
  "Text\0^\0abs\0acos:\0and:\0append:\0asText\0asin:\0at:insert:\0"
  "atan2y:x:\0atan:\0break\0clear\0context\0continue\0copy\0cos:\0cosh:\0"
  "entries\0entry\0execute\0executeOnBreak:\0executeOnBreak:onContinue:\0"
  "executeOnContinue:\0exp:\0false\0forEach:do:\0get:\0get:to:\0getCharAt:\0"
  "getEnviron:\0getLocal:\0getName\0has:\0hasEntry:\0import:\0include:\0"
  "initialize\0insert:\0is:\0isClass\0isEmpty\0isFalse:\0isFalse:else:\0"
  "isNot:\0isNull\0isObject\0isTrue:\0isTrue:else:\0isType:\0items\0last\0"
  "length\0localVariables\0log10:\0log:\0lshift\0lshift:\0message\0messenger\0"
  "neg\0new\0newObject\0not\0or:\0parameter:\0parameters\0pop\0push:\0"
  "remove:\0removeAt:\0removeFrom:to:\0repeat:\0return\0return:\0rshift\0"
  "rshift:\0set:to:\0setApplication:\0setLocal:to:\0sin:\0sinh:\0sqrt:\0"
  "subClass:body:\0superClass\0swap:\0swap:with:\0tan:\0tanh:\0throw\0throw:\0"
  "throw:for:\0timesDo:\0true\0try:\0try:catch:\0try:catch:do:\0type\0weak\0"
  "xor:\0\0"
};

/******************************************************************************
 * Public API
 */

void meat::initialize(int argc, const char *argv[],
											compiler_import_fn import,
											class_compiler_fn compiler) {
  arg_count(argc);
  args(argv);

  /* Create the Class base class. */
  Class *class_cls = new Class(Null());
	class_cls->set_vtable(11, ClassMethods, meat::STATIC);
  class_cls->set_class_vtable(14, ClassCMethods, STATIC);
	class_cls->set_bytecode(1, ClassBytecode, meat::STATIC);
	Class::record(class_cls, "Class");

  /* Create the Object base class. */
  Class *object_cls = new Class(meat::ClassClass());
  object_cls->set_constructor(Object_constructor);
  object_cls->set_vtable(11, ObjectMethods, STATIC);
  object_cls->set_class_vtable(11, ObjectCMethods, STATIC);
  object_cls->set_bytecode(226, ObjectBytecode, STATIC);
  Class::record(object_cls, "Object");

  /*  When the first two class were created the ClassClass reference was null.
   * So here we finish the initialization of the Object and Class classes.
   */
  Reference ObjectClass = Class::resolve("Object");
  ObjectClass->o_type = ClassClass();
  ClassClass()->o_type = ClassClass().weak();

  /* Create the Null class. */
  Class *null_cls = new Class("Object", 1, 0);
  null_cls->set_vtable(6, NullMethods, STATIC);
  null_cls->set_class_vtable(9, NullCMethods, STATIC);
  null_cls->set_bytecode(46, NullBytecode, STATIC);
  Class::record(null_cls, "Null");
  null_cls->property(0) = new Object(Class::resolve("Null"));

  /* Create the Exception class. */
  Class *except_cls = new meat::Class("Object", 2);
  except_cls->set_constructor(Exception_constructor);
  except_cls->set_vtable(8, ExceptionMethods, STATIC);
  except_cls->set_class_vtable(15, ExceptionCMethods, STATIC);
  except_cls->set_bytecode(24, ExceptionBytecode, STATIC);
  Class::record(except_cls, "Exception");

  /* Create the Context class. */
  Class *context_cls = new Class("Object");
  context_cls->set_constructor(Context_constructor);
  context_cls->set_vtable(20, ContextMethods, STATIC);
  context_cls->set_class_vtable(11, ContextCMethods, STATIC);
  context_cls->set_bytecode(131, ContextBytecode, STATIC);
  Class::record(context_cls, "Context");

  /* Create the BlockContext class. */
  Class *block_cls = new Class("Context");
  block_cls->set_vtable(24, BlockContextMethods, STATIC);
	block_cls->set_class_vtable(11, BlockContextCMethods, STATIC);
  Class::record(block_cls, "BlockContext");

  /* Create the Numeric class. */
  Class *numeric_cls = new Class("Object");
  numeric_cls->set_vtable(19, NumericMethods, STATIC);
  numeric_cls->set_class_vtable(23, NumericCMethods, STATIC);
  numeric_cls->set_bytecode(569, NumericBytecode, STATIC);
  Class::record(numeric_cls, "Numeric");

	/* Create the Integer class. */
  Class *integer_cls = new Class("Numeric");
	integer_cls->set_constructor(Integer_constructor);
  integer_cls->set_vtable(23, IntegerMethods, STATIC);
  Class::record(integer_cls, "Integer");

	/* Create the Number class. */
  Class *number_cls = new Class("Numeric");
	number_cls->set_constructor(Number_constructor);
  number_cls->set_vtable(15, NumberMethods, STATIC);
	number_cls->set_class_vtable(19, NumberCMethods, STATIC);
  Class::record(number_cls, "Number");

  /* Create the Text class. */
  Class *text_cls = new Class("Object");
	text_cls->set_constructor(Text_constructor);
  text_cls->set_vtable(22, TextMethods, STATIC);
	text_cls->set_class_vtable(11, TextCMethods, STATIC);
	text_cls->set_bytecode(9, TextBytecode, meat::STATIC);
  Class::record(text_cls, "Text");

  /* Create the Boolean class and objects. */
  Class *bool_cls = new meat::Class("Object", 2, 0);
  bool_cls->set_vtable(18, BooleanMethods, meat::STATIC);
  bool_cls->set_class_vtable(11, BooleanCMethods, meat::STATIC);
  bool_cls->set_bytecode(145, BooleanBytecode, meat::STATIC);
  Class::record(bool_cls, "Boolean");
  bool_cls->property(0) = new Value(true);
  bool_cls->property(1) = new Value(false);

  /* Create the List class. */
  Class *list_cls = new Class("Object");
  list_cls->set_constructor(list_constructor);
  list_cls->set_vtable(22, ListMethods, STATIC);
  list_cls->set_class_vtable(12, ListCMethods, STATIC);
	list_cls->set_bytecode(16, ListBytecode, meat::STATIC);
  Class::record(list_cls, "List");

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
  app_cls->set_bytecode(61, ApplicationBytecode, STATIC);
  Class::record(app_cls, "Application");

	class_compiler() = compiler;
  meat::data::initialize(import);

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
	library->add(index_cls, "Index");
	library->add(app_cls, "Application");
	library->set_symbols(Symbols, meat::STATIC);
}
