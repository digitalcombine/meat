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

#include <meat.h>
#include <meat/datastore.h>
#include "compiler.h"

using namespace meat;

#define null (meat::Null())

/******************************************************************************
 * Grinder.Library Class
 */

static Reference Grinder_Library_constructor(Reference &klass,
                                             uint8_t properties) {
  return new meat::grinder::Library(klass, properties);
}

#define _name (self->property(0))
#define classes (self->property(1))
#define applicationClass (self->property(2))
#define requiredLibraries (self->property(3))
#define cppInclude (self->property(4))

// method addClass:
static Reference Grinder_Library_om_addClass_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference theClass = cast<Context>(context).parameter(0);

  if (theClass->is_type("Grinder.Class"))
    cast<grinder::Library>(self).add_class(theClass);
  else
    throw Exception(std::string("Can not add ") +
                    cast<Class>(theClass->type()).name() +
                    " to a Grinder.Library");
  return null;
}

// method compile
static Reference Grinder_Library_om_compile(Reference context) {
  Reference self = cast<Context>(context).self();

  cast<grinder::Library>(self).compile();
  return null;
}

static meat::vtable_entry_t Grinder_LibraryMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x02a060ae, 0x335f47b6, VTM_BYTECODE, 6, {(meat::method_ptr_t)166}},
  {0x0422afcc, 0x335f47b6, VTM_BYTECODE, 7, {(meat::method_ptr_t)266}},
  {0x14274dc2, 0x335f47b6, VTM_BYTECODE, 12, {(meat::method_ptr_t)0}},
  {0x331152ee, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x335f47b6, VTM_BYTECODE, 6, {(meat::method_ptr_t)224}},
  {0x38a75a5d, 0x335f47b6, VTM_NATIVE  , 0, Grinder_Library_om_compile},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x401f185a, 0x335f47b6, VTM_BYTECODE, 6, {(meat::method_ptr_t)287}},
  {0x49db9c88, 0x335f47b6, VTM_BYTECODE, 6, {(meat::method_ptr_t)146}},
  {0x6447c96a, 0x335f47b6, VTM_NATIVE  , 1, Grinder_Library_om_addClass_},
  {0x645271d8, 0x335f47b6, VTM_BYTECODE, 5, {(meat::method_ptr_t)365}},
  {0x68f39719, 0x335f47b6, VTM_BYTECODE, 7, {(meat::method_ptr_t)299}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x72cd0161, 0x335f47b6, VTM_BYTECODE, 6, {(meat::method_ptr_t)198}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b82e32c, 0x335f47b6, VTM_BYTECODE, 5, {(meat::method_ptr_t)186}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

#undef _name
#undef classes
#undef applicationClass
#undef requiredLibraries
#undef cppInclude

// class method new:
static Reference Grinder_Library_cm_new_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference libraryName = cast<Context>(context).parameter(0);

  Reference new_object = cast<meat::Class>(self).new_object();

  cast<grinder::Library>(new_object).property(0) = libraryName;
  cast<grinder::Library>(new_object).register_as(cast<Text>(libraryName));

  return new_object;
}

static meat::vtable_entry_t Grinder_LibraryCMethods[] = {
  {0x0000043c, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00338a9a, 0x335f47b6, VTM_NATIVE  , 1, Grinder_Library_cm_new_},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x2c296348, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static std::uint8_t Grinder_LibraryBytecode[] = {
  0x11, 0x05, 0x01, 0x14, 0x06, 0x00, 0x00, 0x00, 0x0c, 0x0a, 0x07, 0x04, 0x00,
  0x28, 0x02, 0x0c, 0x0d, 0x7b, 0x82, 0xe3, 0x2c, 0x00, 0x02, 0x04, 0x0e, 0x00,
  0x00, 0x07, 0xa0, 0x01, 0x0d, 0x0a, 0x0f, 0x00, 0x00, 0x09, 0x01, 0x02, 0x41,
  0x79, 0x69, 0x3a, 0x01, 0x0c, 0x0b, 0x01, 0x0e, 0x7b, 0x80, 0xe9, 0x8e, 0x01,
  0x0f, 0x0b, 0x01, 0x05, 0x01, 0x8b, 0x0a, 0x25, 0x02, 0x06, 0x07, 0x13, 0x05,
  0x4f, 0xc2, 0x61, 0x66, 0x16, 0x06, 0x43, 0x6c, 0x61, 0x73, 0x73, 0x20, 0x00,
  0x02, 0x06, 0x07, 0x00, 0x00, 0x00, 0x2b, 0x01, 0x04, 0x16, 0x08, 0x20, 0x77,
  0x61, 0x73, 0x20, 0x6e, 0x6f, 0x74, 0x20, 0x66, 0x6f, 0x75, 0x6e, 0x64, 0x20,
  0x69, 0x6e, 0x20, 0x6c, 0x69, 0x62, 0x72, 0x61, 0x72, 0x79, 0x20, 0x00, 0x02,
  0x07, 0x09, 0x00, 0x00, 0x00, 0x2b, 0x01, 0x08, 0x11, 0x0a, 0x00, 0x02, 0x09,
  0x0b, 0x00, 0x00, 0x00, 0x2b, 0x01, 0x0a, 0x01, 0x05, 0x4b, 0xe1, 0x36, 0x15,
  0x01, 0x0b, 0x0b, 0x11, 0x04, 0x01, 0x02, 0x04, 0x05, 0x00, 0x2e, 0xaf, 0x75,
  0x00, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x05, 0x0b, 0x11, 0x04, 0x03,
  0x02, 0x04, 0x05, 0x00, 0x2e, 0xaf, 0x75, 0x00, 0x01, 0x02, 0x41, 0x79, 0x69,
  0x3a, 0x01, 0x05, 0x0b, 0x11, 0x04, 0x00, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a,
  0x01, 0x04, 0x0b, 0x13, 0x05, 0x6d, 0x20, 0xbc, 0xbb, 0x01, 0x05, 0x72, 0xcd,
  0x01, 0x61, 0x01, 0x04, 0x11, 0x05, 0x03, 0x01, 0x05, 0x50, 0xb5, 0x14, 0x89,
  0x01, 0x04, 0x0b, 0x03, 0x00, 0x34, 0x00, 0x35, 0x78, 0x00, 0x13, 0x04, 0x00,
  0x24, 0x24, 0xbe, 0x02, 0x04, 0x05, 0x00, 0x01, 0xa9, 0xa0, 0x00, 0x20, 0x01,
  0x05, 0x13, 0x04, 0x00, 0x01, 0x44, 0x42, 0x02, 0x04, 0x05, 0x00, 0x01, 0xa9,
  0xa0, 0x00, 0x20, 0x03, 0x05, 0x0b, 0x02, 0x00, 0x05, 0x14, 0x27, 0x4d, 0xc2,
  0x01, 0x04, 0x11, 0x06, 0x01, 0x01, 0x06, 0x41, 0x14, 0x18, 0x66, 0x01, 0x05,
  0x0b, 0x11, 0x05, 0x03, 0x01, 0x05, 0x41, 0x14, 0x18, 0x66, 0x01, 0x04, 0x0b,
  0x13, 0x05, 0x4f, 0xc2, 0x61, 0x66, 0x16, 0x06, 0x43, 0x75, 0x72, 0x72, 0x65,
  0x6e, 0x74, 0x6c, 0x79, 0x20, 0x75, 0x6e, 0x61, 0x62, 0x6c, 0x65, 0x20, 0x74,
  0x6f, 0x20, 0x72, 0x65, 0x6e, 0x61, 0x6d, 0x65, 0x20, 0x6c, 0x69, 0x62, 0x72,
  0x61, 0x72, 0x69, 0x65, 0x73, 0x2e, 0x00, 0x01, 0x05, 0x4b, 0xe1, 0x36, 0x15,
  0x01, 0x06, 0x02, 0x04, 0x05, 0x2c, 0x02, 0x5c, 0x80, 0x00, 0x20, 0x00, 0x05,
  0x0b, 0x20, 0x02, 0x04, 0x0b
};

/******************************************************************************
 * Grinder.Class Class
 */

static Reference Grinder_Class_constructor(Reference &klass,
                                           uint8_t properties) {
  return new grinder::Class(klass, properties);
}

#define className (self->property(0))
#define superClass (self->property(1))
#define objectProperties (self->property(2))
#define classProperties (self->property(3))
#define objectMethods (self->property(4))
#define classMethods (self->property(5))
#define constr (self->property(6))
#define documentation (self->property(7))

// method addClassMethod:
static Reference Grinder_Class_om_addClassMethod_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference theMethod = cast<Context>(context).parameter(0);

  if (theMethod->is_type("Grinder.Method")) {
    cast<grinder::Class &>(self).add_method(theMethod);
  } else {
    throw meat::Exception("Attempting to add a non Grinder.Method to a "
                          "Grinder.Class");
  }

  return null;
}

// method addMethod:
static Reference Grinder_Class_om_addMethod_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference theMethod = cast<Context>(context).parameter(0);

  if (theMethod->is_type("Grinder.Method")) {
    cast<grinder::Class>(self).add_method(theMethod);
  } else {
    throw meat::Exception("Attempting to add a non Grinder.Method to a "
                          "Grinder.Class");
  }

  return null;
}

static meat::vtable_entry_t Grinder_ClassMethods[] = {
  {0x0000043c, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x064662a0, 0x475aee3c, VTM_BYTECODE, 6, {(meat::method_ptr_t)244}},
  {0x13778806, 0x475aee3c, VTM_BYTECODE, 7, {(meat::method_ptr_t)20}},
  {0x13e67172, 0x475aee3c, VTM_BYTECODE, 7, {(meat::method_ptr_t)0}},
  {0x2ac47ced, 0x475aee3c, VTM_NATIVE  , 1, Grinder_Class_om_addMethod_},
  {0x34003578, 0x475aee3c, VTM_BYTECODE, 6, {(meat::method_ptr_t)156}},
  {0x36998c44, 0x475aee3c, VTM_BYTECODE, 5, {(meat::method_ptr_t)144}},
  {0x3751e4ef, 0x475aee3c, VTM_BYTECODE, 5, {(meat::method_ptr_t)256}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x3ab368f5, 0x475aee3c, VTM_BYTECODE, 6, {(meat::method_ptr_t)60}},
  {0x4856517b, 0x475aee3c, VTM_NATIVE  , 1, Grinder_Class_om_addClassMethod_},
  {0x58dc7410, 0x475aee3c, VTM_BYTECODE, 6, {(meat::method_ptr_t)92}},
  {0x66e5dca8, 0x475aee3c, VTM_BYTECODE, 5, {(meat::method_ptr_t)80}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6c1e06d0, 0x475aee3c, VTM_BYTECODE, 6, {(meat::method_ptr_t)232}},
  {0x6f8450c1, 0x475aee3c, VTM_BYTECODE, 6, {(meat::method_ptr_t)124}},
  {0x72e85a72, 0x475aee3c, VTM_BYTECODE, 6, {(meat::method_ptr_t)40}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b82e32c, 0x475aee3c, VTM_BYTECODE, 5, {(meat::method_ptr_t)112}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

#undef className
#undef superClass
#undef objectProperties
#undef classProperties
#undef objectMethods
#undef classMethods
#undef constr
#undef documentation

static meat::vtable_entry_t Grinder_ClassCMethods[] = {
  {0x0000043c, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x06c015a2, 0x475aee3c, VTM_BYTECODE, 7, {(meat::method_ptr_t)260}},
  {0x2c296348, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static std::uint8_t Grinder_ClassBytecode[] = {
  0x11, 0x05, 0x03, 0x02, 0x04, 0x06, 0x2c, 0x02, 0x5c, 0x80, 0x00, 0x01, 0x05,
  0x06, 0x61, 0x34, 0x60, 0x01, 0x06, 0x0b, 0x11, 0x05, 0x02, 0x02, 0x04, 0x06,
  0x2c, 0x02, 0x5c, 0x80, 0x00, 0x01, 0x05, 0x06, 0x61, 0x34, 0x60, 0x01, 0x06,
  0x0b, 0x11, 0x04, 0x05, 0x02, 0x04, 0x05, 0x00, 0x2e, 0xaf, 0x75, 0x00, 0x01,
  0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x05, 0x0b, 0x11, 0x04, 0x03, 0x02, 0x04,
  0x05, 0x00, 0x2e, 0xaf, 0x75, 0x00, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01,
  0x05, 0x0b, 0x11, 0x04, 0x07, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x04,
  0x0b, 0x11, 0x04, 0x04, 0x02, 0x04, 0x05, 0x00, 0x2e, 0xaf, 0x75, 0x00, 0x01,
  0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x05, 0x0b, 0x11, 0x04, 0x00, 0x01, 0x02,
  0x41, 0x79, 0x69, 0x3a, 0x01, 0x04, 0x0b, 0x11, 0x04, 0x02, 0x02, 0x04, 0x05,
  0x00, 0x2e, 0xaf, 0x75, 0x00, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x05,
  0x0b, 0x11, 0x04, 0x01, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x04, 0x0b,
  0x03, 0x00, 0x34, 0x00, 0x35, 0x78, 0x00, 0x13, 0x04, 0x00, 0x24, 0x24, 0xbe,
  0x02, 0x04, 0x05, 0x00, 0x01, 0xa9, 0xa0, 0x00, 0x20, 0x02, 0x05, 0x13, 0x04,
  0x00, 0x24, 0x24, 0xbe, 0x02, 0x04, 0x05, 0x00, 0x01, 0xa9, 0xa0, 0x00, 0x20,
  0x03, 0x05, 0x13, 0x04, 0x04, 0x38, 0x38, 0xb2, 0x02, 0x04, 0x05, 0x00, 0x01,
  0xa9, 0xa0, 0x00, 0x20, 0x04, 0x05, 0x13, 0x04, 0x04, 0x38, 0x38, 0xb2, 0x02,
  0x04, 0x05, 0x00, 0x01, 0xa9, 0xa0, 0x00, 0x20, 0x05, 0x05, 0x0b, 0x11, 0x05,
  0x05, 0x01, 0x05, 0x41, 0x14, 0x18, 0x66, 0x01, 0x04, 0x0b, 0x11, 0x05, 0x04,
  0x01, 0x05, 0x41, 0x14, 0x18, 0x66, 0x01, 0x04, 0x0b, 0x20, 0x07, 0x04, 0x0b,
  0x01, 0x00, 0x54, 0xaa, 0x30, 0xe6, 0x00, 0x20, 0x00, 0x05, 0x02, 0x04, 0x06,
  0x7b, 0x82, 0xe3, 0x2c, 0x00, 0x20, 0x01, 0x06, 0x01, 0x02, 0x41, 0x79, 0x69,
  0x3a, 0x01, 0x00, 0x0b
};

/******************************************************************************
 * Grinder.Method Class
 */

static meat::Reference Grinder_Method_constructor(Reference &klass,
                                                  uint8_t properties) {
  return new grinder::Method(klass, properties);
}

#define methodName (self->property(0))
#define parameters (self->property(1))
#define body (self->property(2))
#define documentation (self->property(3))

// method isNative
static Reference Grinder_Method_om_isNative(Reference context) {
  Reference self = cast<Context>(context).self();

  return meat::Boolean(cast<grinder::Method>(self).is_cpp());
}

// method isNative:
static Reference Grinder_Method_om_isNative_(Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference value = meat::cast<meat::Context>(context).parameter(0);

  if (meat::Boolean(value))
    cast<grinder::Method>(self).is_cpp(true);
  else if (not meat::Boolean(value))
    cast<grinder::Method>(self).is_cpp(false);
  else
    throw meat::Exception("Grinder.Method isNative value is not a boolean");

  return null;
}

static meat::vtable_entry_t Grinder_MethodMethods[] = {
  {0x0000043c, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x2e035c85, 0x34b9249a, VTM_BYTECODE, 7, {(meat::method_ptr_t)0}},
  {0x34003578, 0x34b9249a, VTM_BYTECODE, 6, {(meat::method_ptr_t)64}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x4479a341, 0x34b9249a, VTM_NATIVE  , 1, Grinder_Method_om_isNative_},
  {0x52c4c182, 0x34b9249a, VTM_BYTECODE, 6, {(meat::method_ptr_t)109}},
  {0x696f4752, 0x34b9249a, VTM_NATIVE  , 0, Grinder_Method_om_isNative},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6e7311fa, 0x34b9249a, VTM_BYTECODE, 6, {(meat::method_ptr_t)44}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b7da243, 0x34b9249a, VTM_BYTECODE, 5, {(meat::method_ptr_t)20}},
  {0x7b82e32c, 0x34b9249a, VTM_BYTECODE, 5, {(meat::method_ptr_t)32}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7f4c8f54, 0x34b9249a, VTM_BYTECODE, 7, {(meat::method_ptr_t)89}}
};

#undef methodName
#undef parameters
#undef body
#undef documentation

static meat::vtable_entry_t Grinder_MethodCMethods[] = {
  {0x0000043c, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00338a9a, 0x34b9249a, VTM_BYTECODE, 6, {(meat::method_ptr_t)121}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x2c296348, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static std::uint8_t Grinder_MethodBytecode[] = {
  0x11, 0x05, 0x01, 0x02, 0x04, 0x06, 0x2c, 0x02, 0x5c, 0x80, 0x00, 0x01, 0x05,
  0x06, 0x61, 0x34, 0x60, 0x01, 0x06, 0x0b, 0x11, 0x04, 0x02, 0x01, 0x02, 0x41,
  0x79, 0x69, 0x3a, 0x01, 0x04, 0x0b, 0x11, 0x04, 0x00, 0x01, 0x02, 0x41, 0x79,
  0x69, 0x3a, 0x01, 0x04, 0x0b, 0x11, 0x04, 0x01, 0x02, 0x04, 0x05, 0x00, 0x2e,
  0xaf, 0x75, 0x00, 0x01, 0x02, 0x41, 0x79, 0x69, 0x3a, 0x01, 0x05, 0x0b, 0x03,
  0x00, 0x34, 0x00, 0x35, 0x78, 0x00, 0x13, 0x04, 0x00, 0x24, 0x24, 0xbe, 0x02,
  0x04, 0x05, 0x00, 0x01, 0xa9, 0xa0, 0x00, 0x20, 0x01, 0x05, 0x0b, 0x11, 0x05,
  0x01, 0x02, 0x04, 0x06, 0x2c, 0x02, 0x5c, 0x80, 0x00, 0x01, 0x05, 0x41, 0x14,
  0x18, 0x66, 0x01, 0x06, 0x0b, 0x02, 0x04, 0x05, 0x2c, 0x02, 0x5c, 0x80, 0x00,
  0x20, 0x02, 0x05, 0x0b, 0x01, 0x00, 0x54, 0xaa, 0x30, 0xe6, 0x00, 0x02, 0x04,
  0x05, 0x2c, 0x02, 0x5c, 0x80, 0x00, 0x20, 0x00, 0x05, 0x01, 0x02, 0x41, 0x79,
  0x69, 0x3a, 0x01, 0x00, 0x0b
};

static std::uint8_t Symbols[] = {
  "+\0==\0Grinder.Class\0Grinder.Library\0Grinder.Method\0addClass:\0"
  "addClassMethod:\0addClassProperty:\0addMethod:\0addParameter:\0"
  "addProperty:\0append:\0asText\0compile\0copy\0forEach:do:\0getBody\0"
  "getClass:\0getClassList\0getClassMethodList\0getClassPropertiesList\0"
  "getImportList\0getMethodList\0getName\0getParameterList\0"
  "getPropertiesList\0getSuperName\0import:\0initialize\0isNative\0isNative:\0"
  "isTrue:\0new\0new:\0newObject\0push:\0remove:\0removeClass:\0"
  "removeClassMethod:\0removeImport:\0removeMethod:\0removeParameter:\0"
  "renameTo:\0return:\0setApplicationClass:\0setBody:\0subClass:as:\0throw:\0"
  "\0"
};

/******************************************************************************
 */

extern "C" {
  DECLSPEC void init_Grinder(meat::data::Library &library);
}

void init_Grinder(meat::data::Library &library) {
  meat::Class *cls;

  /* Create the Comiler.Library class. */
  cls = new meat::Class("Object", 5);
  cls->set_constructor(Grinder_Library_constructor);
  cls->set_vtable(23, Grinder_LibraryMethods, meat::STATIC);
  cls->set_class_vtable(12, Grinder_LibraryCMethods, meat::STATIC);
  cls->bytecode(369, Grinder_LibraryBytecode, meat::STATIC);
  library.add(cls, "Grinder.Library");

  /* Create the Compiler.Class class */
  cls = new meat::Class("Object", 8);
  cls->set_constructor(Grinder_Class_constructor);
  cls->set_vtable(25, Grinder_ClassMethods, meat::STATIC);
  cls->set_class_vtable(12, Grinder_ClassCMethods, meat::STATIC);
  cls->bytecode(290, Grinder_ClassBytecode, meat::STATIC);
  library.add(cls, "Grinder.Class");

  /* Create the Compiler.Method class. */
  cls = new meat::Class(meat::Class::resolve(0xc658f60), 4, 0);
  cls->set_constructor(Grinder_Method_constructor);
  cls->set_vtable(19, Grinder_MethodMethods, meat::STATIC);
  cls->set_class_vtable(12, Grinder_MethodCMethods, meat::STATIC);
  cls->bytecode(148, Grinder_MethodBytecode, meat::STATIC);
  library.add(cls, "Grinder.Method");

  library.set_symbols(Symbols, meat::STATIC);
}
