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
 *
 * This file should not be modified directly. It is generated from the
 * builtin.meat found in the tests directory. Make any modification to the
 * tests/builtin.meat file, do a make test and use the tests/fix-builtin.pl
 * script to regenerate this file.
 *
 * This might seem like a chore but it sure beats maintaining this code by
 * hand ;)
 */

#include <meat.h>
#include <meat/datastore.h>

#include <meat/utilities.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <cmath>

#ifdef TESTING
#include <testing.h>
#endif /* TESTING */

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

using namespace meat;

#define null (meat::Null())

/******************************************************************************
 * Class Class
 */

static meat::vtable_entry_t ClassMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x331152ee, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x331156ce, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x48dbf560, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

// class method forEach:do:
static Reference Class_cm_forEach_do_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference _class = cast<Context>(context).parameter(0);
  Reference block = cast<Context>(context).parameter(1);

    std::uint8_t local_id = INTEGER(_class);
    cast<Context>(block).messenger(context);

    Class::iterator it = Class::begin();
    for (; it != Class::end(); ++it) {
      cast<BlockContext>(block).set_break_trap();
      cast<Context>(block).local(local_id) = it->second;
      execute(block);

      if (cast<BlockContext>(block).break_called()) break;
      else cast<BlockContext>(block).reset();
    }

    cast<BlockContext>(block).reset();
    return null;
  }

// class method name
static Reference Class_cm_name(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return new Text(cast<Class>(self).name());
  }

// class method newObject
static Reference Class_cm_newObject(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    Reference messenger = cast<Context>(context).messenger();

    // Create the new object and call the initialize method.
    Reference new_object = cast<Class>(self).new_object();
    Reference init_ctx = message(new_object, "initialize", context);
    execute(init_ctx);

    // We replace the self object to the new object.
    cast<Context>(messenger).self(new_object);

    return null;
  }

// class method subclass:as:
static Reference Class_cm_subclass_as_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference name = cast<Context>(context).parameter(0);
  Reference block = cast<Context>(context).parameter(1);

    if (compiler() != NULL) {
      compiler()->create_class(self, cast<Text>(name),
                               cast<Text>(block), context);
    } else {
      throw Exception("No compiler implementation is loaded");
    }

    return null;
  }

// class method superClass
static Reference Class_cm_superClass(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return cast<Class>(self).super();
  }

static meat::vtable_entry_t ClassCMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00337a8b, 0x03e2b958, VTM_NATIVE  , 0, {(meat::method_ptr_t)Class_cm_name}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x018b0a25, 0x03e2b958, VTM_NATIVE  , 2, {(meat::method_ptr_t)Class_cm_forEach_do_}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x181f14c4, 0x03e2b958, VTM_NATIVE  , 0, {(meat::method_ptr_t)Class_cm_superClass}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x03e2b958, VTM_BYTECODE, 4, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x54aa30e6, 0x03e2b958, VTM_NATIVE  , 0, {(meat::method_ptr_t)Class_cm_newObject}},
  {0x58a69b58, 0x03e2b958, VTM_NATIVE  , 2, {(meat::method_ptr_t)Class_cm_subclass_as_}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static std::uint8_t ClassBytecode[] = {
  0x0b
};

/******************************************************************************
 * Object Class
 */

static Reference Object_constructor(
  Reference &klass,
  std::uint8_t properties) {

    return new meat::Object(klass, properties);
  
}

// method is:
static Reference Object_om_is_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    return Boolean(self == other);
  }

// method isNot:
static Reference Object_om_isNot_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    return Boolean(not (self == other));
  }

// method isType:
static Reference Object_om_isType_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference type = cast<Context>(context).parameter(0);

    return Boolean(self->is_type(type));
  }

// method isWeakReference
static Reference Object_om_isWeakReference(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return Boolean(self.is_weak());
  }

// method normalReference
static Reference Object_om_normalReference(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return self.normal();
  }

// method type
static Reference Object_om_type(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return self->type();
  }

// method weakReference
static Reference Object_om_weakReference(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return self.weak();
  }

static meat::vtable_entry_t ObjectMethods[] = {
  {0x00000782, 0x0c658f60, VTM_BYTECODE, 6, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x0c658f60, VTM_BYTECODE, 6, {(meat::method_ptr_t)18}},
  {0x00019850, 0x0c658f60, VTM_NATIVE  , 1, {(meat::method_ptr_t)Object_om_is_}},
  {0x00368f3a, 0x0c658f60, VTM_NATIVE  , 0, {(meat::method_ptr_t)Object_om_type}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x20be875b, 0x0c658f60, VTM_NATIVE  , 0, {(meat::method_ptr_t)Object_om_weakReference}},
  {0x24ab71da, 0x0c658f60, VTM_NATIVE  , 0, {(meat::method_ptr_t)Object_om_normalReference}},
  {0x331152ee, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x331156ce, 0x0c658f60, VTM_BYTECODE, 4, {(meat::method_ptr_t)36}},
  {0x34003578, 0x0c658f60, VTM_BYTECODE, 4, {(meat::method_ptr_t)37}},
  {0x39a68c12, 0x0c658f60, VTM_NATIVE  , 1, {(meat::method_ptr_t)Object_om_isNot_}},
  {0x39a6a1d2, 0x0c658f60, VTM_BYTECODE, 6, {(meat::method_ptr_t)61}},
  {0x48dbf560, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x0c658f60, VTM_BYTECODE, 6, {(meat::method_ptr_t)84}},
  {0x7a8e569a, 0x0c658f60, VTM_BYTECODE, 6, {(meat::method_ptr_t)38}},
  {0x7b840562, 0x0c658f60, VTM_NATIVE  , 1, {(meat::method_ptr_t)Object_om_isType_}},
  {0x7d180801, 0x0c658f60, VTM_NATIVE  , 0, {(meat::method_ptr_t)Object_om_isWeakReference}}
};

// class method is:
static Reference Object_cm_is_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    return Boolean(self == other);
  }

// class method isNot:
static Reference Object_cm_isNot_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    return Boolean(not (self == other));
  }

// class method isWeakReference
static Reference Object_cm_isWeakReference(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return Boolean(self.is_weak());
  }

// class method normalReference
static Reference Object_cm_normalReference(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return self.normal();
  }

// class method weakReference
static Reference Object_cm_weakReference(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return self.weak();
  }

static meat::vtable_entry_t ObjectCMethods[] = {
  {0x00000782, 0x0c658f60, VTM_BYTECODE, 6, {(meat::method_ptr_t)107}},
  {0x000007a0, 0x0c658f60, VTM_BYTECODE, 6, {(meat::method_ptr_t)125}},
  {0x00019850, 0x0c658f60, VTM_NATIVE  , 1, {(meat::method_ptr_t)Object_cm_is_}},
  {0x00368f3a, 0x0c658f60, VTM_BYTECODE, 5, {(meat::method_ptr_t)236}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x20be875b, 0x0c658f60, VTM_NATIVE  , 0, {(meat::method_ptr_t)Object_cm_weakReference}},
  {0x24ab71da, 0x0c658f60, VTM_NATIVE  , 0, {(meat::method_ptr_t)Object_cm_normalReference}},
  {0x39a68c12, 0x0c658f60, VTM_NATIVE  , 1, {(meat::method_ptr_t)Object_cm_isNot_}},
  {0x39a6a1d2, 0x0c658f60, VTM_BYTECODE, 6, {(meat::method_ptr_t)166}},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x0c658f60, VTM_BYTECODE, 6, {(meat::method_ptr_t)189}},
  {0x7a8e569a, 0x0c658f60, VTM_BYTECODE, 6, {(meat::method_ptr_t)143}},
  {0x7b840562, 0x0c658f60, VTM_BYTECODE, 7, {(meat::method_ptr_t)212}},
  {0x7d180801, 0x0c658f60, VTM_NATIVE  , 0, {(meat::method_ptr_t)Object_cm_isWeakReference}}
};

static std::uint8_t ObjectBytecode[] = {
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
  0x3a, 0x01, 0x05, 0x0b, 0x13, 0x05, 0x03, 0xe2, 0xb9, 0x58, 0x02, 0x05, 0x06,
  0x00, 0x00, 0x07, 0xa0, 0x01, 0x04, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01,
  0x06, 0x0b, 0x13, 0x04, 0x03, 0xe2, 0xb9, 0x58, 0x01, 0x02, 0x41, 0x79, 0x69,
  0x3a, 0x01, 0x04, 0x0b
};

/******************************************************************************
 * Context Class
 */

static Reference Context_constructor(
  Reference &klass,
  std::uint8_t properties) {

    return new meat::Context(properties);
  
}

// method getLocal:
static Reference Context_om_getLocal_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference index = cast<Context>(context).parameter(0);

    return cast<Context>(self).local(INTEGER(index));
  }

// method localVariables
static Reference Context_om_localVariables(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return new meat::Value(cast<Context>(self).locals() - 4);
  }

// method messenger
static Reference Context_om_messenger(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return cast<Context>(self).messenger();
  }

// method repeat:
static Reference Context_om_repeat_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference block = cast<Context>(context).parameter(0);

    cast<Context>(block).messenger(context);

    for (;;) {
      cast<BlockContext>(block).set_break_trap();
      cast<BlockContext>(block).set_continue_trap();
      execute(block);

      if (cast<BlockContext>(block).break_called() or
          cast<Context>(block).is_done())
        break;

      cast<BlockContext>(block).reset();
    }
    cast<BlockContext>(block).reset();

    return null;
  }

// method return
static Reference Context_om_return(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    cast<Context>(self).finish();
    return null;
  }

// method return:
static Reference Context_om_return_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference value = cast<Context>(context).parameter(0);

    cast<Context>(self).result(value); // Set the return value
    cast<Context>(self).finish();      // Tell the context it's done.
    return null;
  }

// method setLocal:to:
static Reference Context_om_setLocal_to_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference index = cast<Context>(context).parameter(0);
  Reference value = cast<Context>(context).parameter(1);

    cast<Context>(self).local(INTEGER(index)) = value;
    return null;
  }

static meat::vtable_entry_t ContextMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x059a58ff, 0x1befcdac, VTM_BYTECODE, 4, {(meat::method_ptr_t)0}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x23add62f, 0x1befcdac, VTM_NATIVE  , 1, {(meat::method_ptr_t)Context_om_getLocal_}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x2a67696c, 0x1befcdac, VTM_NATIVE  , 0, {(meat::method_ptr_t)Context_om_messenger}},
  {0x331152ee, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x331156ce, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x4139862f, 0x1befcdac, VTM_NATIVE  , 1, {(meat::method_ptr_t)Context_om_repeat_}},
  {0x4179693a, 0x1befcdac, VTM_NATIVE  , 1, {(meat::method_ptr_t)Context_om_return_}},
  {0x47206ce4, 0x1befcdac, VTM_NATIVE  , 0, {(meat::method_ptr_t)Context_om_localVariables}},
  {0x484e3d31, 0x1befcdac, VTM_NATIVE  , 0, {(meat::method_ptr_t)Context_om_return}},
  {0x48dbf560, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x5e3131ca, 0x1befcdac, VTM_BYTECODE, 4, {(meat::method_ptr_t)1}},
  {0x675bde74, 0x1befcdac, VTM_NATIVE  , 2, {(meat::method_ptr_t)Context_om_setLocal_to_}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static meat::vtable_entry_t ContextCMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x54aa30e6, 0x1befcdac, VTM_BYTECODE, 7, {(meat::method_ptr_t)2}},
  {0x58a69b58, 0x1befcdac, VTM_BYTECODE, 8, {(meat::method_ptr_t)72}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static std::uint8_t ContextBytecode[] = {
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
static Reference BlockContext_om_break(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    cast<BlockContext>(self).c_break();
    if (not cast<BlockContext>(self).break_trap_set()) {
      Reference init_ctx = cast<BlockContext>(self).origin();
      Reference up_context = message(init_ctx, "break", context);
      execute(up_context);
    }
    return null;
  }

// method continue
static Reference BlockContext_om_continue(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    cast<BlockContext>(self).c_continue();
    if (not meat::cast<meat::BlockContext>(self).continue_trap_set()) {
      Reference init_ctx = cast<BlockContext>(self).origin();
      Reference up_context = message(init_ctx, "continue", context);
      execute(up_context);
    }
    return null;
  }

// method execute
static Reference BlockContext_om_execute(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    cast<BlockContext>(self).messenger(context);
    cast<BlockContext>(self).reset();
    execute(self);

    return null;
  }

// method executeOnBreak:
static Reference BlockContext_om_executeOnBreak_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference breakBlock = cast<Context>(context).parameter(0);

    cast<BlockContext>(self).set_break_trap();
    cast<BlockContext>(self).messenger(context);

    cast<BlockContext>(self).reset();
    execute(self);

    if (cast<BlockContext>(self).break_called()) {
      Reference up_context = message(breakBlock, "execute", context);
      execute(up_context);
    }

    return null;
  }

// method executeOnBreak:onContinue:
static Reference BlockContext_om_executeOnBreak_onContinue_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference breakBlock = cast<Context>(context).parameter(0);
  Reference continueBlock = cast<Context>(context).parameter(1);

    cast<BlockContext>(self).set_break_trap();
    cast<BlockContext>(self).set_continue_trap();
    cast<BlockContext>(self).messenger(context);

    cast<BlockContext>(self).reset();
    execute(self);

    if (cast<BlockContext>(self).break_called()) {
      Reference up_context = message(breakBlock, "execute", context);
      execute(up_context);
    } else if (meat::cast<BlockContext>(self).continue_called()) {
      Reference up_context = message(continueBlock, "execute", context);
      execute(up_context);
    }

    return null;
  }

// method executeOnContinue:
static Reference BlockContext_om_executeOnContinue_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference continueBlock = cast<Context>(context).parameter(0);

    cast<BlockContext>(self).set_continue_trap();
    cast<BlockContext>(self).messenger(context);

    cast<BlockContext>(self).reset();
    execute(self);

    if (cast<BlockContext>(self).continue_called()) {
      Reference up_context = message(continueBlock, "execute", context);
      execute(up_context);
    }

    return null;
  }

// method reset
static Reference BlockContext_om_reset(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    cast<BlockContext>(self).reset();
    return null;
  }

// method return
static Reference BlockContext_om_return(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    // We need to get the actual context to message the return method from.
    Reference origin = cast<BlockContext>(self).origin();
    Reference up_context = message(origin, "return", context);
    execute(up_context);

    cast<Context>(self).finish(); // Tell the context it's done.

    return null;
  }

// method return:
static Reference BlockContext_om_return_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference value = cast<Context>(context).parameter(0);

    Reference origin = cast<BlockContext>(self).origin();
    Reference up_context = message(origin, "return:", context);
    cast<Context>(up_context).parameter(0, value);
    execute(up_context);

    cast<Context>(self).finish(); // Tell the context it's done.

    return null;
  }

static meat::vtable_entry_t BlockContextMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x059a58ff, 0x46ba8a20, VTM_NATIVE  , 0, {(meat::method_ptr_t)BlockContext_om_break}},
  {0x06761d4f, 0x46ba8a20, VTM_NATIVE  , 0, {(meat::method_ptr_t)BlockContext_om_reset}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x290a0d52, 0x46ba8a20, VTM_NATIVE  , 1, {(meat::method_ptr_t)BlockContext_om_executeOnContinue_}},
  {0x3158f7a0, 0x46ba8a20, VTM_NATIVE  , 0, {(meat::method_ptr_t)BlockContext_om_execute}},
  {0x331152ee, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x331156ce, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x4179693a, 0x46ba8a20, VTM_NATIVE  , 1, {(meat::method_ptr_t)BlockContext_om_return_}},
  {0x484e3d31, 0x46ba8a20, VTM_NATIVE  , 0, {(meat::method_ptr_t)BlockContext_om_return}},
  {0x48dbf560, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x5a7f66e9, 0x46ba8a20, VTM_NATIVE  , 1, {(meat::method_ptr_t)BlockContext_om_executeOnBreak_}},
  {0x5e3131ca, 0x46ba8a20, VTM_NATIVE  , 0, {(meat::method_ptr_t)BlockContext_om_continue}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x71828805, 0x46ba8a20, VTM_NATIVE  , 2, {(meat::method_ptr_t)BlockContext_om_executeOnBreak_onContinue_}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static meat::vtable_entry_t BlockContextCMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
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
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x331152ee, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x331156ce, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x002539a7, VTM_BYTECODE, 6, {(meat::method_ptr_t)0}},
  {0x4300a340, 0x002539a7, VTM_BYTECODE, 5, {(meat::method_ptr_t)23}},
  {0x48dbf560, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static meat::vtable_entry_t NullCMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x54aa30e6, 0x002539a7, VTM_BYTECODE, 7, {(meat::method_ptr_t)35}},
  {0x58a69b58, 0x002539a7, VTM_BYTECODE, 9, {(meat::method_ptr_t)102}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

#undef nullObject

static std::uint8_t NullBytecode[] = {
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

static Reference Exception_constructor(
  Reference &klass,
  std::uint8_t properties) {

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
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x331152ee, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x331156ce, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x38b735d9, 0x4fc26166, VTM_BYTECODE, 5, {(meat::method_ptr_t)0}},
  {0x38eb0035, 0x4fc26166, VTM_BYTECODE, 5, {(meat::method_ptr_t)12}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x48dbf560, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

#undef mesg
#undef cntxt

// class method throw
static Reference Exception_cm_throw(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    Reference ctx = cast<Context>(context).messenger();
    throw Exception(null, ctx);
  }

// class method throw:
static Reference Exception_cm_throw_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference message = cast<Context>(context).parameter(0);

    Reference ctx = cast<Context>(context).messenger();
    throw Exception(message, ctx);
  }

// class method throw:for:
static Reference Exception_cm_throw_for_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference message = cast<Context>(context).parameter(0);
  Reference ctx = cast<Context>(context).parameter(1);

    throw Exception(message, ctx);
  }

// class method throwFor:
static Reference Exception_cm_throwFor_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference ctx = cast<Context>(context).parameter(0);

    throw Exception(null, ctx);
  }

// class method try:
static Reference Exception_cm_try_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference block = cast<Context>(context).parameter(0);

    cast<Context>(block).messenger(context);

    try {
      execute(block);
    } catch (meat::Exception &err) {
      cast<BlockContext>(block).reset();
    } catch (...) {
      cast<BlockContext>(block).reset();
    }

    return null;
  }

// class method try:catch:
static Reference Exception_cm_try_catch_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference try_block = cast<Context>(context).parameter(0);
  Reference catch_block = cast<Context>(context).parameter(1);

    try {
      cast<meat::Context>(try_block).messenger(context);
      execute(try_block);
    } catch (std::exception &err) {
      cast<Context>(catch_block).messenger(context);
      execute(catch_block);
      cast<BlockContext>(try_block).reset();
    }

    return null;
  }

// class method try:catch:do:
static Reference Exception_cm_try_catch_do_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference try_block = cast<Context>(context).parameter(0);
  Reference error = cast<Context>(context).parameter(1);
  Reference catch_block = cast<Context>(context).parameter(2);

    try {
      cast<Context>(try_block).messenger(context);
      execute(try_block);
    } catch (Exception &err) {
      Reference excp = new meat::Exception(err);
      cast<Context>(catch_block).local(INTEGER(error)) = excp;
      cast<Context>(catch_block).messenger(context);
      execute(catch_block);
      cast<BlockContext>(try_block).reset();
    }

    return null;
  }

static meat::vtable_entry_t ExceptionCMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x003675df, 0x4fc26166, VTM_NATIVE  , 1, {(meat::method_ptr_t)Exception_cm_try_}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x0693a6e6, 0x4fc26166, VTM_NATIVE  , 0, {(meat::method_ptr_t)Exception_cm_throw}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x351bfdf2, 0x4fc26166, VTM_NATIVE  , 1, {(meat::method_ptr_t)Exception_cm_throwFor_}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x4be13615, 0x4fc26166, VTM_NATIVE  , 1, {(meat::method_ptr_t)Exception_cm_throw_}},
  {0x4e48308e, 0x4fc26166, VTM_NATIVE  , 3, {(meat::method_ptr_t)Exception_cm_try_catch_do_}},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6db68ab6, 0x4fc26166, VTM_NATIVE  , 2, {(meat::method_ptr_t)Exception_cm_throw_for_}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7bebbd56, 0x4fc26166, VTM_NATIVE  , 2, {(meat::method_ptr_t)Exception_cm_try_catch_}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static std::uint8_t ExceptionBytecode[] = {
  0x11, 0x04, 0x01, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x04, 0x0b, 0x11,
  0x04, 0x00, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x04, 0x0b
};

/******************************************************************************
 * Boolean Class
 */

#define trueObject (klass->property(0))
#define falseObject (klass->property(1))

// method and:
static Reference Boolean_om_and_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    return Boolean((self == trueObject) and (other == trueObject));
  }

// method isFalse:
static Reference Boolean_om_isFalse_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference block = cast<Context>(context).parameter(0);

    if (self == falseObject) {
      cast<BlockContext>(block).messenger(context);
      execute(block);
      cast<BlockContext>(block).reset();
    }
    return null;
  }

// method isFalse:else:
static Reference Boolean_om_isFalse_else_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference falseBlock = cast<Context>(context).parameter(0);
  Reference elseBlock = cast<Context>(context).parameter(1);

    if (self == falseObject) {
      cast<BlockContext>(falseBlock).messenger(context);
      execute(falseBlock);
      cast<BlockContext>(falseBlock).reset();
    } else {
      cast<BlockContext>(elseBlock).messenger(context);
      execute(elseBlock);
      cast<BlockContext>(elseBlock).reset();
    }
    return null;
  }

// method isTrue:
static Reference Boolean_om_isTrue_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference block = cast<Context>(context).parameter(0);

    if (self == trueObject) {
      cast<BlockContext>(block).messenger(context);
      execute(block);
    }
    return null;
  }

// method isTrue:else:
static Reference Boolean_om_isTrue_else_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference trueBlock = cast<Context>(context).parameter(0);
  Reference elseBlock = cast<Context>(context).parameter(1);

    if (self == trueObject) {
      cast<BlockContext>(trueBlock).messenger(context);
      execute(trueBlock);
    } else {
      cast<BlockContext>(elseBlock).messenger(context);
      execute(elseBlock);
    }
    return null;
  }

// method or:
static Reference Boolean_om_or_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    return Boolean((self == trueObject) or (other == trueObject));
  }

// method xor:
static Reference Boolean_om_xor_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    return Boolean((self == trueObject) xor (other == trueObject));
  }

static meat::vtable_entry_t BooleanMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x0001aad3, 0x67140424, VTM_BYTECODE, 8, {(meat::method_ptr_t)65}},
  {0x0001aeb7, 0x67140424, VTM_NATIVE  , 1, {(meat::method_ptr_t)Boolean_om_or_}},
  {0x002dc143, 0x67140424, VTM_NATIVE  , 1, {(meat::method_ptr_t)Boolean_om_and_}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00383b3f, 0x67140424, VTM_NATIVE  , 1, {(meat::method_ptr_t)Boolean_om_xor_}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x2c025c80, 0x67140424, VTM_BYTECODE, 8, {(meat::method_ptr_t)0}},
  {0x331152ee, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x331156ce, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x47607971, 0x67140424, VTM_NATIVE  , 2, {(meat::method_ptr_t)Boolean_om_isTrue_else_}},
  {0x48dbf560, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x57c628fe, 0x67140424, VTM_NATIVE  , 2, {(meat::method_ptr_t)Boolean_om_isFalse_else_}},
  {0x5bc502b2, 0x67140424, VTM_NATIVE  , 1, {(meat::method_ptr_t)Boolean_om_isFalse_}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b80e98e, 0x67140424, VTM_NATIVE  , 1, {(meat::method_ptr_t)Boolean_om_isTrue_}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static meat::vtable_entry_t BooleanCMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x0036758e, 0x67140424, VTM_BYTECODE, 5, {(meat::method_ptr_t)133}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x05cb1923, 0x67140424, VTM_BYTECODE, 5, {(meat::method_ptr_t)121}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

#undef trueObject
#undef falseObject

static std::uint8_t BooleanBytecode[] = {
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
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x2c025c80, 0x6bfcb30e, VTM_BYTECODE, 6, {(meat::method_ptr_t)479}},
  {0x331152ee, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x331156ce, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x36e711e1, 0x6bfcb30e, VTM_BYTECODE, 6, {(meat::method_ptr_t)536}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x48dbf560, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6701127b, 0x6bfcb30e, VTM_BYTECODE, 6, {(meat::method_ptr_t)436}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static meat::vtable_entry_t NumericCMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x17cbc00b, 0x6bfcb30e, VTM_BYTECODE, 6, {(meat::method_ptr_t)579}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x2ded23e0, 0x6bfcb30e, VTM_BYTECODE, 6, {(meat::method_ptr_t)631}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static std::uint8_t NumericBytecode[] = {
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

static Reference Integer_constructor(
  Reference &klass,
  std::uint8_t properties) {

    return new Value(klass, properties);
  
}

// method %
static Reference Integer_om_mod(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    return new Value(INTEGER(self) % INTEGER(other));
  }

// method *
static Reference Integer_om_mult(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    return new Value(INTEGER(self) * INTEGER(other));
  }

// method +
static Reference Integer_om_add(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    std::int32_t res;
    bool overflow = __builtin_add_overflow(INTEGER(self),
                                           INTEGER(other),
                                           &res);
    if (overflow)
      throw Exception("Addition overflow");
    return new Value(res);
  }

// method -
static Reference Integer_om_sub(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    return new Value(INTEGER(self) - INTEGER(other));
  }

// method /
static Reference Integer_om_div(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    return new Value(INTEGER(self) / INTEGER(other));
  }

// method <
static Reference Integer_om_less(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    try {
      return Boolean(INTEGER(self) < INTEGER(other));
    } catch (...) {}
    return Boolean(true);
  }

// method <=
static Reference Integer_om_less_equal(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    try {
      return Boolean(INTEGER(self) <= INTEGER(other));
    } catch (...) {}
    return Boolean(true);
  }

// method <>
static Reference Integer_om_nequals(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    try {
      return Boolean(INTEGER(self) != INTEGER(other));
    } catch (...) {}
    return Boolean(true);
  }

// method ==
static Reference Integer_om_equals(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    try {
      return Boolean(INTEGER(self) == INTEGER(other));
    } catch (...) {}
    return Boolean(false);
  }

// method >
static Reference Integer_om_greater(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    try {
      return Boolean(INTEGER(self) > INTEGER(other));
    } catch (...) {}
    return Boolean(true);
  }

// method >=
static Reference Integer_om_greater_equal(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    try {
      return Boolean(INTEGER(self) >= INTEGER(other));
    } catch (...) {}
    return Boolean(true);
  }

// method ^
static Reference Integer_om_pow(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    return new Value((double)std::pow(FLOAT(self),
                                      FLOAT(other)));
  }

// method absolute
static Reference Integer_om_absolute(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return new Value(std::abs(INTEGER(self)));
  }

// method and:
static Reference Integer_om_and_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    return new Value(INTEGER(self) & INTEGER(other));
  }

// method asText
static Reference Integer_om_asText(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    std::stringstream result;
    result << INTEGER(self);
    return new Text(result.str());
  }

// method lshift
static Reference Integer_om_lshift(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return new Value(INTEGER(self) << 1);
  }

// method lshift:
static Reference Integer_om_lshift_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference amount = cast<Context>(context).parameter(0);

    return new Value(INTEGER(self) << INTEGER(amount));
  }

// method negative
static Reference Integer_om_negative(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return new meat::Value(-INTEGER(self));
  }

// method or:
static Reference Integer_om_or_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    return new Value(INTEGER(self) | INTEGER(other));
  }

// method rshift
static Reference Integer_om_rshift(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return new Value(INTEGER(self) >> 1);
  }

// method rshift:
static Reference Integer_om_rshift_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference amount = cast<Context>(context).parameter(0);

    return new Value(INTEGER(self) >> INTEGER(amount));
  }

// method timesDo:
static Reference Integer_om_timesDo_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference block = cast<Context>(context).parameter(0);

    std::int32_t limit = INTEGER(self);
    for (std::int_fast32_t c = 0; c < limit; c++) {
      execute(block);
    }
    return null;
  }

// method xor:
static Reference Integer_om_xor_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    return new Value(INTEGER(self) ^ INTEGER(other));
  }

static meat::vtable_entry_t IntegerMethods[] = {
  {0x00000025, 0x57ee193d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Integer_om_mod}},
  {0x0000002a, 0x57ee193d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Integer_om_mult}},
  {0x0000002b, 0x57ee193d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Integer_om_add}},
  {0x0000002d, 0x57ee193d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Integer_om_sub}},
  {0x0000002f, 0x57ee193d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Integer_om_div}},
  {0x0000003c, 0x57ee193d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Integer_om_less}},
  {0x0000003e, 0x57ee193d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Integer_om_greater}},
  {0x0000005e, 0x57ee193d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Integer_om_pow}},
  {0x00000781, 0x57ee193d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Integer_om_less_equal}},
  {0x00000782, 0x57ee193d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Integer_om_nequals}},
  {0x000007a0, 0x57ee193d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Integer_om_equals}},
  {0x000007bf, 0x57ee193d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Integer_om_greater_equal}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x0001aeb7, 0x57ee193d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Integer_om_or_}},
  {0x002dc143, 0x57ee193d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Integer_om_and_}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00383b3f, 0x57ee193d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Integer_om_xor_}},
  {0x043b7de4, 0x57ee193d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Integer_om_timesDo_}},
  {0x1b492392, 0x57ee193d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Integer_om_lshift_}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x2c025c80, 0x57ee193d, VTM_NATIVE  , 0, {(meat::method_ptr_t)Integer_om_asText}},
  {0x331152ee, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x331156ce, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x36e711e1, 0x57ee193d, VTM_NATIVE  , 0, {(meat::method_ptr_t)Integer_om_negative}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x3ed0cf97, 0x57ee193d, VTM_NATIVE  , 0, {(meat::method_ptr_t)Integer_om_lshift}},
  {0x48dbf560, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x490de351, 0x57ee193d, VTM_NATIVE  , 0, {(meat::method_ptr_t)Integer_om_rshift}},
  {0x58ae871a, 0x57ee193d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Integer_om_rshift_}},
  {0x6701127b, 0x57ee193d, VTM_NATIVE  , 0, {(meat::method_ptr_t)Integer_om_absolute}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

// class method maxValue
static Reference Integer_cm_maxValue(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return new Value(2147483647);
  }

// class method minValue
static Reference Integer_cm_minValue(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return new Value(-2147483647-1);
  }

static meat::vtable_entry_t IntegerCMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x17cbc00b, 0x57ee193d, VTM_NATIVE  , 0, {(meat::method_ptr_t)Integer_cm_maxValue}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x2ded23e0, 0x57ee193d, VTM_NATIVE  , 0, {(meat::method_ptr_t)Integer_cm_minValue}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

/******************************************************************************
 * Number Class
 */

static Reference Number_constructor(
  Reference &klass,
  std::uint8_t properties) {

    return new Value(klass, properties);
  
}

// method %
static Reference Number_om_mod(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    return new Value(std::fmod(FLOAT(self), FLOAT(other)));
  }

// method *
static Reference Number_om_mult(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    return new Value(FLOAT(self) * FLOAT(other));
  }

// method +
static Reference Number_om_add(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    return new Value(FLOAT(self) + FLOAT(other));
  }

// method -
static Reference Number_om_sub(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    return new Value(FLOAT(self) - FLOAT(other));
  }

// method /
static Reference Number_om_div(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    if (FLOAT(other) == 0.0)
      throw Exception("Divide by zero");

    return new Value(FLOAT(self) / FLOAT(other));
  }

// method <
static Reference Number_om_less(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    try {
      return Boolean(FLOAT(self) < FLOAT(other));
    } catch (...) {}
    return Boolean(false);
  }

// method <=
static Reference Number_om_less_equal(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    try {
      return Boolean(FLOAT(self) <= FLOAT(other));
    } catch (...) {}
    return Boolean(false);
  }

// method <>
static Reference Number_om_nequals(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    try {
      return Boolean(FLOAT(self) != FLOAT(other));
    } catch (...) {}
    return Boolean(true);
  }

// method ==
static Reference Number_om_equals(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    try {
      return Boolean(FLOAT(self) == FLOAT(other));
    } catch (...) {}
    return Boolean(false);
  }

// method >
static Reference Number_om_greater(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    try {
      return Boolean(FLOAT(self) > FLOAT(other));
    } catch (...) {}
    return Boolean(false);
  }

// method >=
static Reference Number_om_greater_equal(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    try {
      return Boolean(FLOAT(self) >= FLOAT(other));
    } catch (...) {}
    return Boolean(false);
  }

// method ^
static Reference Number_om_pow(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    return new Value((double)std::pow(FLOAT(self),
                                      FLOAT(other)));
  }

// method absolute
static Reference Number_om_absolute(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return new Value(std::abs(FLOAT(self)));
  }

// method asText
static Reference Number_om_asText(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    std::stringstream result;
    result << (double)FLOAT(self);
    return new Text(result.str());
  }

// method negative
static Reference Number_om_negative(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return new Value(-FLOAT(self));
  }

static meat::vtable_entry_t NumberMethods[] = {
  {0x00000025, 0x0bbdc76a, VTM_NATIVE  , 1, {(meat::method_ptr_t)Number_om_mod}},
  {0x0000002a, 0x0bbdc76a, VTM_NATIVE  , 1, {(meat::method_ptr_t)Number_om_mult}},
  {0x0000002b, 0x0bbdc76a, VTM_NATIVE  , 1, {(meat::method_ptr_t)Number_om_add}},
  {0x0000002d, 0x0bbdc76a, VTM_NATIVE  , 1, {(meat::method_ptr_t)Number_om_sub}},
  {0x0000002f, 0x0bbdc76a, VTM_NATIVE  , 1, {(meat::method_ptr_t)Number_om_div}},
  {0x0000003c, 0x0bbdc76a, VTM_NATIVE  , 1, {(meat::method_ptr_t)Number_om_less}},
  {0x0000003e, 0x0bbdc76a, VTM_NATIVE  , 1, {(meat::method_ptr_t)Number_om_greater}},
  {0x0000005e, 0x0bbdc76a, VTM_NATIVE  , 1, {(meat::method_ptr_t)Number_om_pow}},
  {0x00000781, 0x0bbdc76a, VTM_NATIVE  , 1, {(meat::method_ptr_t)Number_om_less_equal}},
  {0x00000782, 0x0bbdc76a, VTM_NATIVE  , 1, {(meat::method_ptr_t)Number_om_nequals}},
  {0x000007a0, 0x0bbdc76a, VTM_NATIVE  , 1, {(meat::method_ptr_t)Number_om_equals}},
  {0x000007bf, 0x0bbdc76a, VTM_NATIVE  , 1, {(meat::method_ptr_t)Number_om_greater_equal}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x2c025c80, 0x0bbdc76a, VTM_NATIVE  , 0, {(meat::method_ptr_t)Number_om_asText}},
  {0x331152ee, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x331156ce, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x36e711e1, 0x0bbdc76a, VTM_NATIVE  , 0, {(meat::method_ptr_t)Number_om_negative}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x48dbf560, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6701127b, 0x0bbdc76a, VTM_NATIVE  , 0, {(meat::method_ptr_t)Number_om_absolute}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static meat::vtable_entry_t NumberCMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x17cbc00b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x2ded23e0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

/******************************************************************************
 * Text Class
 */

static Reference Text_constructor(
  Reference &klass,
  std::uint8_t properties) {

    return new Text(klass, properties);
  
}

// method *
static Reference Text_om_mult(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference count = cast<Context>(context).parameter(0);

    std::int32_t cnt_value = INTEGER(count);
    Text *result = new Text();

    for (std::int_fast32_t c = 0; c < cnt_value; ++c)
      (*result) += cast<Text>(self);

    return result;
  }

// method +
static Reference Text_om_add(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    return new Text(cast<Text>(self) + cast<Text>(other));
  }

// method <
static Reference Text_om_less(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    return Boolean(cast<Text>(self).compare(cast<Text>(other)) < 0);
  }

// method <=
static Reference Text_om_less_equal(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    return Boolean(cast<Text>(self).compare(cast<Text>(other)) <= 0);
  }

// method <>
static Reference Text_om_nequals(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    return Boolean(cast<Text>(self).compare(cast<Text>(other)) != 0);
  }

// method ==
static Reference Text_om_equals(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    return Boolean(cast<Text>(self).compare(cast<Text>(other)) == 0);
  }

// method >
static Reference Text_om_greater(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    return Boolean(cast<Text>(self).compare(cast<Text>(other)) > 0);
  }

// method >=
static Reference Text_om_greater_equal(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    return Boolean(cast<Text>(self).compare(cast<Text>(other)) >= 0);
  }

// method asInteger
static Reference Text_om_asInteger(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return null;
  }

// method asNumber
static Reference Text_om_asNumber(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return null;
  }

// method copy
static Reference Text_om_copy(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return new Text(cast<Text>(self));
  }

// method findFirst:
static Reference Text_om_findFirst_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference value = cast<Context>(context).parameter(0);

    std::int32_t index = 1;
    utf8::Iterator first_char(cast<const Text>(value));
    utf8::Iterator it(cast<const Text>(self));

    for (; it != it.end(); ++it, ++index) {
      if ((*it == *first_char) and
          (cast<Text>(self).compare(it.position(),
                                    cast<Text>(value).length(),
                                    cast<Text>(value)) == 0)) {
        return new Value(index);
      }
    }

    return new Value(0);
  }

// method findFirst:at:
static Reference Text_om_findFirst_at_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference value = cast<Context>(context).parameter(0);
  Reference index = cast<Context>(context).parameter(1);

    std::int32_t result = 1;
    utf8::Iterator first_char(cast<const Text>(value));
    utf8::Iterator it(cast<const Text>(self));

    int start_idx = INTEGER(index);

    // Make sure the starting index is in range.
    if (start_idx <= 0)
      throw Exception("Text character index out of range");

    for (; it != it.end() and start_idx > 1; ++it, --start_idx, ++result);

    if (it == it.end())
      throw Exception("Text character index out of range");

    for (; it != it.end(); ++it, ++result) {
      if ((*it == *first_char) and
          (cast<Text>(self).compare(it.position(),
                                    cast<Text>(value).length(),
                                    cast<Text>(value)) == 0)) {
        return new Value(result);
      }
    }

    return new Value(0);
  }

// method findLast:
static Reference Text_om_findLast_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference value = cast<Context>(context).parameter(0);

    return null;
  }

// method findLast:at:
static Reference Text_om_findLast_at_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference value = cast<Context>(context).parameter(0);
  Reference index = cast<Context>(context).parameter(1);

    return null;
  }

// method forEach:do:
static Reference Text_om_forEach_do_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference character = cast<Context>(context).parameter(0);
  Reference block = cast<Context>(context).parameter(1);

    std::uint8_t local_id = INTEGER(character);
    cast<Context>(block).messenger(context);

    utf8::Iterator it(cast<const Text>(self));
    for (; it != it.end(); ++it) {
      cast<BlockContext>(block).set_break_trap();
      cast<Context>(block).local(local_id) = new Text(*it);
      execute(block);

      if (cast<BlockContext>(block).break_called()) break;
      else cast<BlockContext>(block).reset();
    }
    cast<BlockContext>(block).reset();

    return null;
  }

// method getCharAt:
static Reference Text_om_getCharAt_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference index = cast<Context>(context).parameter(0);

    int i = INTEGER(index);

    if (i <= 0) throw Exception("Text character index out of range");

    utf8::Iterator it(cast<const Text>(self));
    for (; it != it.end() and i > 1; ++it, --i);

    if (it == it.end())
      throw Exception("Text character index out of range");

    return new Text(*it);
  }

// method getFrom:count:
static Reference Text_om_getFrom_count_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference start = cast<Context>(context).parameter(0);
  Reference end = cast<Context>(context).parameter(1);

    int start_idx = INTEGER(start);
    int end_count = INTEGER(end);

    // Make sure the starting index is in range.
    if (start_idx <= 0)
      throw Exception("Text character index out of range");

    utf8::Iterator it(cast<const Text>(self));
    for (; it != it.end() and start_idx > 1; ++it, --start_idx);

    if (it == it.end())
      throw Exception("Text character index out of range");

    utf8::Iterator start_it(it);

    for (; it != it.end() and end_count > 2; ++it, --end_count);

    return new Text(utf8::substr(start_it, it));
  }

// method isEmpty
static Reference Text_om_isEmpty(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return Boolean(cast<Text>(self).empty());
  }

// method length
static Reference Text_om_length(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    std::int32_t length = 0;
    utf8::Iterator it(cast<const Text>(self));
    for (; it != it.end(); ++it, ++length);

    return new Value(length);
  }

// method replaceAll:with:
static Reference Text_om_replaceAll_with_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference text = cast<Context>(context).parameter(0);
  Reference value = cast<Context>(context).parameter(1);

    const std::string &tvalue = cast<const Text>(text);
    const std::string &vvalue = cast<const Text>(value);
    std::string result = cast<Text>(self);

    size_t index = result.find(tvalue);
    while (index != std::string::npos) {
      result.replace(index, tvalue.length(), vvalue);
      index = result.find(tvalue, index + vvalue.length());
    }

    return new Text(result);
  }

// method replaceFrom:count:with:
static Reference Text_om_replaceFrom_count_with_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference start = cast<Context>(context).parameter(0);
  Reference end = cast<Context>(context).parameter(1);
  Reference value = cast<Context>(context).parameter(2);

    int start_idx = INTEGER(start);
    int end_count = INTEGER(end);

    // Make sure the starting index is in range.
    if (start_idx <= 0)
      throw Exception("Text character index out of range");

    utf8::Iterator it(cast<const Text>(self));
    for (; it != it.end() and start_idx > 1; ++it, --start_idx);

    if (it == it.end())
      throw Exception("Text character index out of range");

    utf8::Iterator start_it(it);

    for (; it != it.end() and end_count > 1; ++it, --end_count);

    return new Text(utf8::replace(start_it, it, cast<Text>(value)));
  }

static meat::vtable_entry_t TextMethods[] = {
  {0x0000002a, 0x0027b94d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Text_om_mult}},
  {0x0000002b, 0x0027b94d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Text_om_add}},
  {0x0000003c, 0x0027b94d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Text_om_less}},
  {0x0000003e, 0x0027b94d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Text_om_greater}},
  {0x00000781, 0x0027b94d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Text_om_less_equal}},
  {0x00000782, 0x0027b94d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Text_om_nequals}},
  {0x000007a0, 0x0027b94d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Text_om_equals}},
  {0x000007bf, 0x0027b94d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Text_om_greater_equal}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x002eaf75, 0x0027b94d, VTM_NATIVE  , 0, {(meat::method_ptr_t)Text_om_copy}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x018b0a25, 0x0027b94d, VTM_NATIVE  , 2, {(meat::method_ptr_t)Text_om_forEach_do_}},
  {0x02b7e8da, 0x0027b94d, VTM_BYTECODE, 4, {(meat::method_ptr_t)0}},
  {0x0fd97630, 0x0027b94d, VTM_NATIVE  , 3, {(meat::method_ptr_t)Text_om_replaceFrom_count_with_}},
  {0x188f29d9, 0x0027b94d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Text_om_findLast_}},
  {0x18e824c4, 0x0027b94d, VTM_NATIVE  , 2, {(meat::method_ptr_t)Text_om_findFirst_at_}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x2b7c6b26, 0x0027b94d, VTM_NATIVE  , 0, {(meat::method_ptr_t)Text_om_asNumber}},
  {0x2c025c80, 0x0027b94d, VTM_BYTECODE, 4, {(meat::method_ptr_t)9}},
  {0x2eb56bfb, 0x0027b94d, VTM_BYTECODE, 4, {(meat::method_ptr_t)18}},
  {0x3003ed09, 0x0027b94d, VTM_NATIVE  , 0, {(meat::method_ptr_t)Text_om_asInteger}},
  {0x30c68f6a, 0x0027b94d, VTM_NATIVE  , 2, {(meat::method_ptr_t)Text_om_replaceAll_with_}},
  {0x331152ee, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x331156ce, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x3e0e3ae7, 0x0027b94d, VTM_NATIVE  , 0, {(meat::method_ptr_t)Text_om_length}},
  {0x46a8bf3a, 0x0027b94d, VTM_NATIVE  , 2, {(meat::method_ptr_t)Text_om_getFrom_count_}},
  {0x48dbf560, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x69eddf35, 0x0027b94d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Text_om_getCharAt_}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6f8948ba, 0x0027b94d, VTM_NATIVE  , 1, {(meat::method_ptr_t)Text_om_findFirst_}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7aab326f, 0x0027b94d, VTM_NATIVE  , 0, {(meat::method_ptr_t)Text_om_isEmpty}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7c186c41, 0x0027b94d, VTM_NATIVE  , 2, {(meat::method_ptr_t)Text_om_findLast_at_}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static meat::vtable_entry_t TextCMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static std::uint8_t TextBytecode[] = {
  0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x00, 0x0b, 0x01, 0x02, 0x41, 0x79,
  0x69, 0x3a, 0x01, 0x00, 0x0b, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x00,
  0x0b
};

/******************************************************************************
 * List Class
 */

static Reference List_constructor(
  Reference &klass,
  std::uint8_t properties) {

    return new List(klass, properties);
  
}

// method append:
static Reference List_om_append_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference value = cast<Context>(context).parameter(0);

    if (value->is_type(meat::Class::resolve("List"))) {
      cast<List>(self).insert(cast<List>(self).end(),
                              cast<List>(value).begin(),
                              cast<List>(value).end());
    } else {
      cast<List>(self).push_back(value);
    }
    return null;
  }

// method at:insert:
static Reference List_om_at_insert_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference index = cast<Context>(context).parameter(0);
  Reference value = cast<Context>(context).parameter(1);

    cast<List>(self).insert(cast<List>(self).begin() + (INTEGER(index) - 1),
                            value);
    return null;
  }

// method clear
static Reference List_om_clear(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    cast<List>(self).clear();
    return null;
  }

// method copy
static Reference List_om_copy(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    Reference new_list = new List(cast<List>(self));
    return new_list;
  }

// method entries
static Reference List_om_entries(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return new Value((int32_t)(cast<List>(self).size()));
  }

// method forEach:do:
static Reference List_om_forEach_do_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference item = cast<Context>(context).parameter(0);
  Reference block = cast<Context>(context).parameter(1);

    std::uint8_t local_id = INTEGER(item);
    cast<Context>(block).messenger(context);

    List::iterator it = cast<List>(self).begin();
    for (; it != cast<List>(self).end(); it++) {
      cast<BlockContext>(block).set_break_trap();
      cast<Context>(block).local(local_id) = *it;
      execute(block);

      if (cast<BlockContext>(block).break_called()) break;
      else cast<BlockContext>(block).reset();
    }

    return null;
  }

// method front
static Reference List_om_front(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return cast<List>(self).front();
  }

// method get:
static Reference List_om_get_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference index = cast<Context>(context).parameter(0);

    try {
      return cast<List>(self).at(INTEGER(index) - 1);
    } catch (std::out_of_range &err) {
      throw Exception("List index out of Range", context);
    }
  }

// method isEmpty
static Reference List_om_isEmpty(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return Boolean(cast<List>(self).empty());
  }

// method last
static Reference List_om_last(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return cast<List>(self).back();
  }

// method pop
static Reference List_om_pop(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    cast<List>(self).pop_back();
    return null;
  }

// method popFront
static Reference List_om_popFront(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    cast<List>(self).pop_back();
    return null;
  }

// method push:
static Reference List_om_push_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference value = cast<Context>(context).parameter(0);

    cast<List>(self).push_back(value);
    return null;
  }

// method pushFront:
static Reference List_om_pushFront_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference value = cast<Context>(context).parameter(0);

    cast<List>(self).push_front(value);
    return null;
  }

// method remove:
static Reference List_om_remove_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference value = cast<Context>(context).parameter(0);

    List::iterator it = cast<List>(self).begin();
    while (it != cast<List>(self).end()) {
      if (*it == value)
        cast<List>(self).erase(it);
      it++;
    }
    return null;
  }

// method removeAt:
static Reference List_om_removeAt_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference index = cast<Context>(context).parameter(0);

    cast<List>(self).erase(cast<List>(self).begin() + (INTEGER(index) - 1));
    return null;
  }

// method removeFrom:to:
static Reference List_om_removeFrom_to_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference start = cast<Context>(context).parameter(0);
  Reference end = cast<Context>(context).parameter(1);

    cast<List>(self).erase(cast<List>(self).begin() + (INTEGER(start) - 1),
                           cast<List>(self).begin() + (INTEGER(end) - 1));
    return null;
  }

// method set:to:
static Reference List_om_set_to_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference index = cast<Context>(context).parameter(0);
  Reference value = cast<Context>(context).parameter(1);

    try {
      cast<List>(self).at(INTEGER(index) - 1) = value;
    } catch (std::out_of_range &err) {
      throw Exception("Index out of Range", context);
    }
    return null;
  }

// method sort
static Reference List_om_sort(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    //std::sort(cast<List>(self).begin(), cast<List>(self).end());
    return null;
  }

// method swap:with:
static Reference List_om_swap_with_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference first = cast<Context>(context).parameter(0);
  Reference second = cast<Context>(context).parameter(1);

    try {
      Reference temp = cast<List>(self).at(INTEGER(first) - 1);
      cast<List>(self).at(INTEGER(first) - 1) =
        cast<List>(self).at(INTEGER(second) - 1);
      cast<List>(self).at(INTEGER(second) - 1) = temp;
    } catch (std::out_of_range &err) {
      throw Exception("Index out of Range", context);
    }
    return null;
  }

static meat::vtable_entry_t ListMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x0001b251, 0x002424be, VTM_NATIVE  , 0, {(meat::method_ptr_t)List_om_pop}},
  {0x002eaf75, 0x002424be, VTM_NATIVE  , 0, {(meat::method_ptr_t)List_om_copy}},
  {0x00305ba4, 0x002424be, VTM_NATIVE  , 1, {(meat::method_ptr_t)List_om_get_}},
  {0x00329296, 0x002424be, VTM_NATIVE  , 0, {(meat::method_ptr_t)List_om_last}},
  {0x0035f59e, 0x002424be, VTM_NATIVE  , 0, {(meat::method_ptr_t)List_om_sort}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x018b0a25, 0x002424be, VTM_NATIVE  , 2, {(meat::method_ptr_t)List_om_forEach_do_}},
  {0x05a5b64d, 0x002424be, VTM_NATIVE  , 0, {(meat::method_ptr_t)List_om_clear}},
  {0x05d2de29, 0x002424be, VTM_NATIVE  , 0, {(meat::method_ptr_t)List_om_front}},
  {0x06613460, 0x002424be, VTM_NATIVE  , 1, {(meat::method_ptr_t)List_om_push_}},
  {0x12b51eb5, 0x002424be, VTM_NATIVE  , 2, {(meat::method_ptr_t)List_om_removeFrom_to_}},
  {0x13a72bf2, 0x002424be, VTM_NATIVE  , 2, {(meat::method_ptr_t)List_om_swap_with_}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x212284bb, 0x002424be, VTM_NATIVE  , 0, {(meat::method_ptr_t)List_om_entries}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x25860c22, 0x002424be, VTM_NATIVE  , 0, {(meat::method_ptr_t)List_om_popFront}},
  {0x331152ee, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x331156ce, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x3c94d873, 0x002424be, VTM_NATIVE  , 2, {(meat::method_ptr_t)List_om_at_insert_}},
  {0x41141866, 0x002424be, VTM_NATIVE  , 1, {(meat::method_ptr_t)List_om_remove_}},
  {0x48dbf560, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x491b0cd1, 0x002424be, VTM_NATIVE  , 1, {(meat::method_ptr_t)List_om_pushFront_}},
  {0x4c6fc15b, 0x002424be, VTM_NATIVE  , 1, {(meat::method_ptr_t)List_om_removeAt_}},
  {0x50b51489, 0x002424be, VTM_NATIVE  , 1, {(meat::method_ptr_t)List_om_append_}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7644da37, 0x002424be, VTM_NATIVE  , 2, {(meat::method_ptr_t)List_om_set_to_}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7aab326f, 0x002424be, VTM_NATIVE  , 0, {(meat::method_ptr_t)List_om_isEmpty}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static meat::vtable_entry_t ListCMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x0001a9a0, 0x002424be, VTM_BYTECODE, 4, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static std::uint8_t ListBytecode[] = {
  0x01, 0x00, 0x54, 0xaa, 0x30, 0xe6, 0x00, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a,
  0x01, 0x00, 0x0b
};

/******************************************************************************
 * Set Class
 */

static Reference Set_constructor(
  Reference &klass,
  std::uint8_t properties) {

    return new Set(klass, properties);
  
}

// method clear
static Reference Set_om_clear(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    cast<Set>(self).clear();
    return null;
  }

// method copy
static Reference Set_om_copy(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    Reference new_set = new Set(cast<Set>(self));
    return new_set;
  }

// method entries
static Reference Set_om_entries(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return new Value((std::int32_t)cast<Set>(self).size());
  }

// method forEach:do:
static Reference Set_om_forEach_do_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference item = cast<Context>(context).parameter(0);
  Reference block = cast<Context>(context).parameter(1);

    std::uint8_t local_id = INTEGER(item);

    cast<Context>(block).messenger(context);

    Set::iterator it = cast<Set>(self).begin();
    for (; it != cast<Set>(self).end(); it++) {
      cast<BlockContext>(block).local(local_id) = *it;
      execute(block);

      if (cast<BlockContext>(block).break_called() or
          cast<Context>(block).is_done())
        break;

      cast<BlockContext>(block).reset();
    }
    cast<BlockContext>(block).reset();

    return null;
  }

// method hasEntry:
static Reference Set_om_hasEntry_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference value = cast<Context>(context).parameter(0);

    return Boolean(cast<Set>(self).find(value) !=
                   cast<Set>(self).end());
  }

// method insert:
static Reference Set_om_insert_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference value = cast<Context>(context).parameter(0);

    cast<Set>(self).insert(value);
    return null;
  }

// method isEmpty
static Reference Set_om_isEmpty(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return Boolean(cast<Set>(self).empty());
  }

// method remove:
static Reference Set_om_remove_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference value = cast<Context>(context).parameter(0);

    Set::iterator it = cast<Set>(self).begin();
    while (it != cast<Set>(self).end()) {
      if (*it == value)
        cast<Set>(self).erase(it);
      it++;
    }

    return null;
  }

// method swap:
static Reference Set_om_swap_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference other = cast<Context>(context).parameter(0);

    cast<Set>(self).swap(cast<Set>(other));
    return null;
  }

static meat::vtable_entry_t SetMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x002eaf75, 0x00014442, VTM_NATIVE  , 0, {(meat::method_ptr_t)Set_om_copy}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x018b0a25, 0x00014442, VTM_NATIVE  , 2, {(meat::method_ptr_t)Set_om_forEach_do_}},
  {0x05a5b64d, 0x00014442, VTM_NATIVE  , 0, {(meat::method_ptr_t)Set_om_clear}},
  {0x068c2107, 0x00014442, VTM_NATIVE  , 1, {(meat::method_ptr_t)Set_om_swap_}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x212284bb, 0x00014442, VTM_NATIVE  , 0, {(meat::method_ptr_t)Set_om_entries}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x331152ee, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x331156ce, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x41141866, 0x00014442, VTM_NATIVE  , 1, {(meat::method_ptr_t)Set_om_remove_}},
  {0x48dbf560, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x578f4fa5, 0x00014442, VTM_NATIVE  , 1, {(meat::method_ptr_t)Set_om_hasEntry_}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x74a794cd, 0x00014442, VTM_NATIVE  , 1, {(meat::method_ptr_t)Set_om_insert_}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7aab326f, 0x00014442, VTM_NATIVE  , 0, {(meat::method_ptr_t)Set_om_isEmpty}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static meat::vtable_entry_t SetCMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x0001a9a0, 0x00014442, VTM_BYTECODE, 4, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static std::uint8_t SetBytecode[] = {
  0x01, 0x00, 0x54, 0xaa, 0x30, 0xe6, 0x00, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a,
  0x01, 0x00, 0x0b
};

/******************************************************************************
 * Index Class
 */

static Reference Index_constructor(
  Reference &klass,
  std::uint8_t properties) {

    return new Index(klass, properties);
  
}

// method copy
static Reference Index_om_copy(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    Reference new_index = new Index(cast<Index>(self));
    return new_index;
  }

// method entries
static Reference Index_om_entries(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    std::int32_t size = cast<Index>(self).size();
    return new Value(size);
  }

// method get:
static Reference Index_om_get_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference key = cast<Context>(context).parameter(0);

    if (cast<Index>(self).find(key) != cast<Index>(self).end())
      return cast<Index>(self)[key];
    throw Exception("Invalid key in index");
  }

// method has:
static Reference Index_om_has_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference key = cast<Context>(context).parameter(0);

    return Boolean(cast<Index>(self).find(key) !=
                   cast<Index>(self).end());
  }

// method isEmpty
static Reference Index_om_isEmpty(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return Boolean(cast<Index>(self).empty());
  }

// method remove:
static Reference Index_om_remove_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference key = cast<Context>(context).parameter(0);

    cast<Index>(self).erase(key);
    return null;
  }

// method set:to:
static Reference Index_om_set_to_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference key = cast<Context>(context).parameter(0);
  Reference value = cast<Context>(context).parameter(1);

    cast<Index>(self)[key] = value;
    return null;
  }

static meat::vtable_entry_t IndexMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x002eaf75, 0x043838b2, VTM_NATIVE  , 0, {(meat::method_ptr_t)Index_om_copy}},
  {0x00305ba4, 0x043838b2, VTM_NATIVE  , 1, {(meat::method_ptr_t)Index_om_get_}},
  {0x0030c0e0, 0x043838b2, VTM_NATIVE  , 1, {(meat::method_ptr_t)Index_om_has_}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x212284bb, 0x043838b2, VTM_NATIVE  , 0, {(meat::method_ptr_t)Index_om_entries}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x331152ee, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x331156ce, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x41141866, 0x043838b2, VTM_NATIVE  , 1, {(meat::method_ptr_t)Index_om_remove_}},
  {0x48dbf560, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7644da37, 0x043838b2, VTM_NATIVE  , 2, {(meat::method_ptr_t)Index_om_set_to_}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7aab326f, 0x043838b2, VTM_NATIVE  , 0, {(meat::method_ptr_t)Index_om_isEmpty}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static meat::vtable_entry_t IndexCMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x0001a9a0, 0x043838b2, VTM_BYTECODE, 4, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static std::uint8_t IndexBytecode[] = {
  0x01, 0x00, 0x54, 0xaa, 0x30, 0xe6, 0x00, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a,
  0x01, 0x00, 0x0b
};

/******************************************************************************
 * Application Class
 */

static meat::vtable_entry_t ApplicationMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x05c30872, 0x419df72b, VTM_BYTECODE, 6, {(meat::method_ptr_t)0}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x331152ee, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x331156ce, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x48dbf560, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

// class method getEnviron:
static Reference Application_cm_getEnviron_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference key = cast<Context>(context).parameter(0);

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

// class method parameter:
static Reference Application_cm_parameter_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference index = cast<Context>(context).parameter(0);

    std::int32_t idx = INTEGER(index);

    if (idx > arg_count() or idx <= 0)
      throw meat::Exception("Parameter index out of range");

    const char **argv = args();

    return new meat::Text(argv[idx - 1]);
  }

// class method parameters
static Reference Application_cm_parameters(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return new meat::Value((std::int32_t)arg_count());
  }

static meat::vtable_entry_t ApplicationCMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x0462f057, 0x419df72b, VTM_NATIVE  , 1, {(meat::method_ptr_t)Application_cm_getEnviron_}},
  {0x05c30872, 0x419df72b, VTM_BYTECODE, 6, {(meat::method_ptr_t)61}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x1b6d6881, 0x419df72b, VTM_NATIVE  , 1, {(meat::method_ptr_t)Application_cm_parameter_}},
  {0x1b6d68ba, 0x419df72b, VTM_NATIVE  , 0, {(meat::method_ptr_t)Application_cm_parameters}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static std::uint8_t ApplicationBytecode[] = {
  0x13, 0x04, 0x4f, 0xc2, 0x61, 0x66, 0x16, 0x05, 0x41, 0x70, 0x70, 0x6c, 0x69,
  0x63, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x6d, 0x65, 0x74, 0x68, 0x6f, 0x64,
  0x20, 0x65, 0x6e, 0x74, 0x72, 0x79, 0x20, 0x6d, 0x75, 0x73, 0x74, 0x20, 0x62,
  0x65, 0x20, 0x6f, 0x76, 0x65, 0x72, 0x72, 0x69, 0x64, 0x64, 0x65, 0x6e, 0x00,
  0x01, 0x04, 0x4b, 0xe1, 0x36, 0x15, 0x01, 0x05, 0x0b, 0x13, 0x04, 0x4f, 0xc2,
  0x61, 0x66, 0x16, 0x05, 0x41, 0x70, 0x70, 0x6c, 0x69, 0x63, 0x61, 0x74, 0x69,
  0x6f, 0x6e, 0x20, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x20, 0x6d, 0x65, 0x74, 0x68,
  0x6f, 0x64, 0x20, 0x65, 0x6e, 0x74, 0x72, 0x79, 0x20, 0x6d, 0x75, 0x73, 0x74,
  0x20, 0x62, 0x65, 0x20, 0x6f, 0x76, 0x65, 0x72, 0x72, 0x69, 0x64, 0x64, 0x65,
  0x6e, 0x00, 0x01, 0x04, 0x4b, 0xe1, 0x36, 0x15, 0x01, 0x05, 0x0b
};

/******************************************************************************
 * Archive Class
 */

// method getObject
static Reference Archive_om_getObject(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    return cast<data::Archive>(self).get_object();
  }

// method requires:
static Reference Archive_om_requires_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference libraryName = cast<Context>(context).parameter(0);

    data::Archive &archive_obj = cast<data::Archive>(self);
    data::Library::import(cast<Text>(libraryName).c_str());
    archive_obj.add_import(cast<Text>(libraryName).c_str());
    return null;
  }

// method setObject:
static Reference Archive_om_setObject_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference value = cast<Context>(context).parameter(0);

    cast<data::Archive>(self).set_object(value);
    return null;
  }

// method sync
static Reference Archive_om_sync(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

    cast<data::Archive>(self).sync();
    return null;
  }

static meat::vtable_entry_t ArchiveMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00361a9b, 0x36a178be, VTM_NATIVE  , 0, {(meat::method_ptr_t)Archive_om_sync}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x28186b3f, 0x36a178be, VTM_NATIVE  , 0, {(meat::method_ptr_t)Archive_om_getObject}},
  {0x29950066, 0x36a178be, VTM_NATIVE  , 1, {(meat::method_ptr_t)Archive_om_requires_}},
  {0x331152ee, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x331156ce, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x48dbf560, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x4e688b77, 0x36a178be, VTM_NATIVE  , 1, {(meat::method_ptr_t)Archive_om_setObject_}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

// class method create:
static Reference Archive_cm_create_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference filename = cast<Context>(context).parameter(0);

    return new data::Archive(cast<Text>(filename), true);
  }

// class method open:
static Reference Archive_cm_open_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference filename = cast<Context>(context).parameter(0);

    return new data::Archive(cast<Text>(filename));
  }

static meat::vtable_entry_t ArchiveCMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x0650a330, 0x36a178be, VTM_NATIVE  , 1, {(meat::method_ptr_t)Archive_cm_open_}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x3d4e7ee8, 0x36a178be, VTM_NATIVE  , 1, {(meat::method_ptr_t)Archive_cm_create_}},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

/******************************************************************************
 * Library Class
 */

static meat::vtable_entry_t LibraryMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x331152ee, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x331156ce, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x48dbf560, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

// class method import:
static Reference Library_cm_import_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference libraryName = cast<Context>(context).parameter(0);

#ifdef DEBUG
    std::cout << "DEBUG: Import library " << cast<Text>(libraryName)
              << std::endl;
#endif

    data::Library::import(cast<Text>(libraryName));
    return null;
  }

// class method include:
static Reference Library_cm_include_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference cpp_includes = cast<Context>(context).parameter(0);

    if (compiler() != NULL)
      compiler()->include(cast<Text>(cpp_includes));
    else
      throw Exception("Method Library include: compiler not loaded");

    return null;
  }

// class method requires:
static Reference Library_cm_requires_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference libraryName = cast<Context>(context).parameter(0);

#ifdef DEBUG
    std::cout << "DEBUG: Importing library " << cast<Text>(libraryName)
              << std::endl;
#endif /* DEBUG */

    if (compiler() != NULL)
      compiler()->import(cast<Text>(libraryName), context);
    else
      throw Exception("Method \"Library requires:\" compiler not loaded");

    return null;
  }

// class method setApplicationClass:
static Reference Library_cm_setApplicationClass_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference theClass = cast<Context>(context).parameter(0);

    if (compiler() != NULL)
      compiler()->set_application_class(theClass);
    else
      throw Exception("Method Library setApplicationClass: compiler not loaded");
    return null;
  }

static meat::vtable_entry_t LibraryCMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x05614602, 0x6d20bcbb, VTM_NATIVE  , 1, {(meat::method_ptr_t)Library_cm_include_}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x29950066, 0x6d20bcbb, VTM_NATIVE  , 1, {(meat::method_ptr_t)Library_cm_requires_}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x645271d8, 0x6d20bcbb, VTM_NATIVE  , 1, {(meat::method_ptr_t)Library_cm_setApplicationClass_}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x72cd0161, 0x6d20bcbb, VTM_NATIVE  , 1, {(meat::method_ptr_t)Library_cm_import_}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static std::uint8_t Symbols[] = {
  "%\0*\0+\0-\0/\0<\0<=\0<>\0==\0>\0>=\0Application\0Archive\0"
  "BlockContext\0Boolean\0Class\0Context\0Exception\0Index\0"
  "Integer\0Library\0List\0Null\0Number\0Numeric\0"
  "Object\0Set\0Text\0^\0absolute\0and:\0append:\0asInteger\0"
  "asLowercase\0asNumber\0asText\0asUppercase\0at:insert:\0break\0cleanup\0"
  "clear\0context\0continue\0copy\0create:\0entries\0entry\0execute\0"
  "executeOnBreak:\0executeOnBreak:onContinue:\0executeOnContinue:\0false\0"
  "findFirst:\0findFirst:at:\0findLast:\0findLast:at:\0forEach:do:\0front\0"
  "get:\0getCharAt:\0getEnviron:\0getFrom:count:\0getLocal:\0getObject\0has:\0"
  "hasEntry:\0import:\0include:\0initialize\0insert:\0is:\0isClass\0isEmpty\0"
  "isFalse:\0isFalse:else:\0isNot:\0isNull\0isObject\0isTrue:\0isTrue:else:\0"
  "isType:\0isWeakReference\0last\0length\0localVariables\0lshift\0lshift:\0"
  "maxValue\0message\0messenger\0minValue\0name\0negative\0new\0newObject\0"
  "normalReference\0not\0object\0open:\0or:\0parameter:\0parameters\0pop\0"
  "popFront\0push:\0pushFront:\0remove:\0removeAt:\0removeFrom:to:\0repeat:\0"
  "replaceAll:with:\0replaceFrom:count:with:\0requires:\0reset\0return\0"
  "return:\0rshift\0rshift:\0set:to:\0setApplicationClass:\0setLocal:to:\0"
  "setObject:\0sort\0subclass:as:\0superClass\0swap:\0swap:with:\0sync\0"
  "throw\0throw:\0throw:for:\0throwFor:\0timesDo:\0true\0try:\0try:catch:\0"
  "try:catch:do:\0type\0uplevel\0weakReference\0xor:\0\0"
};

/******************************************************************************
 * Public API
 */

void meat::initialize(int argc, const char *argv[]) {
  arg_count(argc);
  args(argv);

  // Import required libraries.

  Class *class_cls = new Class(Null());
  class_cls->set_vtable(17, ClassMethods, meat::STATIC);
  class_cls->set_class_vtable(19, ClassCMethods, meat::STATIC);
  class_cls->bytecode(1, ClassBytecode, meat::STATIC);
  Class::record(class_cls, "Class");

  Class *object_cls = new Class("Class");
  object_cls->set_constructor(Object_constructor);
  object_cls->set_vtable(17, ObjectMethods, meat::STATIC);
  object_cls->set_class_vtable(14, ObjectCMethods, meat::STATIC);
  object_cls->bytecode(251, ObjectBytecode, meat::STATIC);
  Class::record(object_cls, "Object");

  /*  When the first two class were created the ClassClass reference was null.
   * So here we finish the initialization of the Object and Class classes.
   */
  object_cls->_type = Class::resolve("Class");
  class_cls->_type = Class::resolve("Class").weak();

  Class *context_cls = new meat::Class(meat::Class::resolve(0x0c658f60), 0, 0);
  context_cls->set_constructor(Context_constructor);
  context_cls->set_vtable(26, ContextMethods, meat::STATIC);
  context_cls->set_class_vtable(15, ContextCMethods, meat::STATIC);
  context_cls->bytecode(131, ContextBytecode, meat::STATIC);
  Class::record(context_cls, "Context");

  Class *blockcontext_cls = new meat::Class(meat::Class::resolve(0x1befcdac), 0, 0);
  blockcontext_cls->set_vtable(26, BlockContextMethods, meat::STATIC);
  blockcontext_cls->set_class_vtable(14, BlockContextCMethods, meat::STATIC);
  Class::record(blockcontext_cls, "BlockContext");

  Class *null_cls = new meat::Class(meat::Class::resolve(0x0c658f60), 1, 0);
  null_cls->set_vtable(18, NullMethods, meat::STATIC);
  null_cls->set_class_vtable(15, NullCMethods, meat::STATIC);
  null_cls->bytecode(167, NullBytecode, meat::STATIC);
  Class::record(null_cls, "Null");
  null_cls->property(0) = new Object(Class::resolve("Null"));

  Class *exception_cls = new meat::Class(meat::Class::resolve(0x0c658f60), 0, 2);
  exception_cls->set_constructor(Exception_constructor);
  exception_cls->set_vtable(19, ExceptionMethods, meat::STATIC);
  exception_cls->set_class_vtable(21, ExceptionCMethods, meat::STATIC);
  exception_cls->bytecode(24, ExceptionBytecode, meat::STATIC);
  Class::record(exception_cls, "Exception");

  Class *boolean_cls = new meat::Class(meat::Class::resolve(0x0c658f60), 2, 0);
  boolean_cls->set_vtable(26, BooleanMethods, meat::STATIC);
  boolean_cls->set_class_vtable(16, BooleanCMethods, meat::STATIC);
  boolean_cls->bytecode(145, BooleanBytecode, meat::STATIC);
  Class::record(boolean_cls, "Boolean");
  boolean_cls->property(0) = new Value(true);
  boolean_cls->property(1) = new Value(false);

  Class *numeric_cls = new meat::Class(meat::Class::resolve(0x0c658f60), 0, 0);
  numeric_cls->set_vtable(30, NumericMethods, meat::STATIC);
  numeric_cls->set_class_vtable(16, NumericCMethods, meat::STATIC);
  numeric_cls->bytecode(683, NumericBytecode, meat::STATIC);
  Class::record(numeric_cls, "Numeric");

  Class *integer_cls = new meat::Class(meat::Class::resolve(0x6bfcb30e), 0, 0);
  integer_cls->set_constructor(Integer_constructor);
  integer_cls->set_vtable(38, IntegerMethods, meat::STATIC);
  integer_cls->set_class_vtable(16, IntegerCMethods, meat::STATIC);
  Class::record(integer_cls, "Integer");

  Class *number_cls = new meat::Class(meat::Class::resolve(0x6bfcb30e), 0, 0);
  number_cls->set_constructor(Number_constructor);
  number_cls->set_vtable(30, NumberMethods, meat::STATIC);
  number_cls->set_class_vtable(16, NumberCMethods, meat::STATIC);
  Class::record(number_cls, "Number");

  Class *text_cls = new meat::Class(meat::Class::resolve(0x0c658f60), 0, 0);
  text_cls->set_constructor(Text_constructor);
  text_cls->set_vtable(40, TextMethods, meat::STATIC);
  text_cls->set_class_vtable(14, TextCMethods, meat::STATIC);
  text_cls->bytecode(27, TextBytecode, meat::STATIC);
  Class::record(text_cls, "Text");

  Class *list_cls = new meat::Class(meat::Class::resolve(0x0c658f60), 0, 0);
  list_cls->set_constructor(List_constructor);
  list_cls->set_vtable(37, ListMethods, meat::STATIC);
  list_cls->set_class_vtable(15, ListCMethods, meat::STATIC);
  list_cls->bytecode(16, ListBytecode, meat::STATIC);
  Class::record(list_cls, "List");

  Class *set_cls = new meat::Class(meat::Class::resolve(0x0c658f60), 0, 0);
  set_cls->set_constructor(Set_constructor);
  set_cls->set_vtable(26, SetMethods, meat::STATIC);
  set_cls->set_class_vtable(15, SetCMethods, meat::STATIC);
  set_cls->bytecode(16, SetBytecode, meat::STATIC);
  Class::record(set_cls, "Set");

  Class *index_cls = new meat::Class(meat::Class::resolve(0x0c658f60), 0, 0);
  index_cls->set_constructor(Index_constructor);
  index_cls->set_vtable(24, IndexMethods, meat::STATIC);
  index_cls->set_class_vtable(15, IndexCMethods, meat::STATIC);
  index_cls->bytecode(16, IndexBytecode, meat::STATIC);
  Class::record(index_cls, "Index");

  Class *application_cls = new meat::Class(meat::Class::resolve(0x0c658f60), 0, 0);
  application_cls->set_vtable(18, ApplicationMethods, meat::STATIC);
  application_cls->set_class_vtable(18, ApplicationCMethods, meat::STATIC);
  application_cls->bytecode(128, ApplicationBytecode, meat::STATIC);
  Class::record(application_cls, "Application");

  Class *archive_cls = new meat::Class(meat::Class::resolve(0x0c658f60), 0, 0);
  archive_cls->set_vtable(21, ArchiveMethods, meat::STATIC);
  archive_cls->set_class_vtable(16, ArchiveCMethods, meat::STATIC);
  Class::record(archive_cls, "Archive");

  Class *library_cls = new meat::Class(meat::Class::resolve(0x0c658f60), 0, 0);
  library_cls->set_vtable(17, LibraryMethods, meat::STATIC);
  library_cls->set_class_vtable(18, LibraryCMethods, meat::STATIC);
  Class::record(library_cls, "Library");

  meat::data::initialize();

#if defined(__WIN32__)
  meat::data::Library::add_path("C:\\meat\\");
#elif defined(__linux__)
  meat::data::Library::add_path("/usr/lib/meat/");
#endif

  data::Library *library = data::Library::create("__builtin__");
  library->add(object_cls, "Object");
  library->add(class_cls, "Class");
  library->add(context_cls, "Context");
  library->add(blockcontext_cls, "BlockContext");
  library->add(null_cls, "Null");
  library->add(exception_cls, "Exception");
  library->add(boolean_cls, "Boolean");
  library->add(numeric_cls, "Numeric");
  library->add(integer_cls, "Integer");
  library->add(number_cls, "Number");
  library->add(text_cls, "Text");
  library->add(list_cls, "List");
  library->add(set_cls, "Set");
  library->add(index_cls, "Index");
  library->add(archive_cls, "Archive");
  library->add(library_cls, "Library");
  library->add(application_cls, "Application");

  library->set_symbols(Symbols, meat::STATIC);
}
