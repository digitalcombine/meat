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
#include <meat/utilities.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>

#ifdef TESTING
#include <testing.h>
#endif

/******************************************************************************
 * meat::Object Class
 */

/************************
 * meat::Object::Object *
 ************************/

meat::Object::Object(Reference type) : _type(type), _property(0) {
#ifdef TESTING
  meat::test::test("Object type setting", false);
  if (!type.is_null()) {
    if (!type->is_class())
      throw Exception("An Object type must be a class.");
  } else {
    meat::test::failed("Object type setting", false);
  }
#endif
  _properties = meat::cast<meat::Class>(type).obj_properties();
  if (_properties > 0) {
    // Initialize the properties and set them all to Null.
    _property = new Reference[_properties];
    for (unsigned int c = 0; c < _properties; c++)
      _property[c] = Null();
  }
}

meat::Object::Object(Reference type, meat::uint8_t properties)
  : _type(type), _properties(properties), _property(0) {
#ifdef TESTING
  static bool first_fail = true;

  meat::test::test("Object type setting with properties", false);
  if (!type.is_null()) {
    if (!type->is_class())
      throw Exception("An Object type must be a class.");
  } else {
    if (not first_fail)
      meat::test::failed("Object type setting with properties", false);
    first_fail = false;
  }
#endif
  if (_properties > 0) {
    // Initialize the properties and set them all to Null.
    _property = new Reference[_properties];
    for (unsigned int c = 0; c < _properties; c++)
      _property[c] = Null();
  }
}

/*************************
 * meat::Object::~Object *
 *************************/

meat::Object::~Object() throw () {
  // Clean up all the properties
  if (_property)
    delete [] _property;
}

/*************************
 * meat::Object::is_type *
 *************************/

bool meat::Object::is_type(Reference cls) const {
  Reference my_type = _type;

#ifdef TESTING
  meat::test::test("Type testing against a class", false);
  if (cls.is_null() or not cls->is_class())
    meat::test::failed("Type testing against a class", false);
#endif

  while (!my_type.is_null()) {
    if (my_type == cls)
      return true;
    my_type = cast<Class>(my_type).super();
  }

  return false;
}

bool meat::Object::is_type(const char *class_name) const {
  return is_type(meat::Class::resolve(class_name));
}

/***************************
 * meat::Object::serialize *
 ***************************/

void meat::Object::serialize(data::Archive &store,
                             std::ostream &data_stream) const {
}

/*****************************
 * meat::Object::unserialize *
 *****************************/

void meat::Object::unserialize(data::Archive &store,
                               std::istream &data_stream) {
}

/**************************
 * meat::Object::property *
 **************************/

meat::Reference &meat::Object::property(meat::uint8_t index) {
  if (index < _properties)
    return _property[index];
  std::stringstream msg;
  msg << "Invalid property index " << (unsigned int)index
      << " ( >= " << (unsigned int)_properties << ")"
      << std::endl;
  throw Exception(msg.str());
}

const meat::Reference &meat::Object::property(meat::uint8_t index) const {
  if (index < _properties)
    return _property[index];
  std::stringstream msg;
  msg << "Invalid property index " << (unsigned int)index
      << " ( >= " << (unsigned int)_properties << ")"
      << std::endl;
  throw Exception(msg.str());
}

/******************************************************************************
 * meat::Class Class
 */

/** The mapping type for the class registry.
 */
typedef std::map<meat::uint32_t, meat::Reference> class_registry_t;

/** Since Meat is compiled as a shared library we cannot use class properties
 * since the properties will get duplicated instead of be a global source. To
 * fix this problem we need to use the following work around.
 */
static class_registry_t &class_registry() {
  static class_registry_t classes;

  return classes;
}

/**********************
 * meat::Class::Class *
 **********************/

meat::Class::Class(const char *parent, uint8_t obj_props)
  : Object(meat::ClassClass(), 0), _hash_id(0), _obj_properties(obj_props),
    library(NULL), _bytecode_size(0), _bytecode(0), _bytecode_static(true) {

  _super = resolve(parent);
}

meat::Class::Class(const char *parent, uint8_t cls_props, uint8_t obj_props)
  : Object(meat::ClassClass(), cls_props), _hash_id(0),
    _obj_properties(obj_props), library(NULL), _bytecode_size(0), _bytecode(0),
    _bytecode_static(true) {

  _super = resolve(parent);
}

meat::Class::Class(meat::Reference parent, uint8_t obj_props)
  : Object(meat::ClassClass(true), 0), _hash_id(0), _super(parent),
    _obj_properties(obj_props), library(NULL), _bytecode_size(0), _bytecode(0),
    _bytecode_static(true) {
}

meat::Class::Class(meat::Reference parent,
                   uint8_t cls_props,
                   uint8_t obj_props)
  : Object(meat::ClassClass(), cls_props), _hash_id(0), _super(parent),
    _obj_properties(obj_props), library(NULL), _bytecode_size(0), _bytecode(0),
    _bytecode_static(true) {
}

/***********************
 * meat::Class::~Class *
 ***********************/

meat::Class::~Class () throw () {
  if (!_bytecode_static && _bytecode != NULL) delete[] _bytecode;
}

/*******************************
 * meat::Class::obj_properties *
 *******************************/

meat::uint8_t meat::Class::obj_properties() const {
  meat::uint8_t total = _obj_properties;
  if (!_super.is_null())
    total += cast<const Class>(_super).obj_properties();
  return total;
}

/***************************
 * meat::Class::new_object *
 ***************************/

meat::Reference meat::Class::new_object() {
  Reference result;

  if (vtable.constructor)
    return vtable.constructor((Reference &)(*this), obj_properties());
  else {
    /*  If we don't have a native constructor then lets see if our super
     * does.
     */
    Reference csuper = _super;
    while (not csuper.is_null()) {
      if (cast<Class>(csuper).vtable.constructor) {
        return cast<Class>(csuper).vtable.constructor((Reference &)(*this),
                                                      obj_properties());
      }
      csuper = cast<Class>(csuper)._super;
    }
  }

  throw Exception(std::string("Unable to create new object for ") + name());
}

/********************************
 * meat::Class::set_constructor *
 ********************************/

void meat::Class::set_constructor(constructor_t constructor) {
  vtable.set(constructor);
}

/***************************
 * meat::Class::set_vtable *
 ***************************/

void meat::Class::set_vtable(uint8_t entries, vtable_entry_t table[],
                             alloc_t table_alloc) {
  vtable.set_vtable(entries, table, table_alloc);
}

/*********************************
 * meat::Class::set_class_vtable *
 *********************************/

void meat::Class::set_class_vtable(uint8_t entries, vtable_entry_t table[],
                                   alloc_t table_alloc) {
  vtable.set_class_vtable(entries, table, table_alloc);
}

/*************************
 * meat::Class::bytecode *
 *************************/

void meat::Class::bytecode(uint16_t size, uint8_t *code, alloc_t code_alloc) {
  //_bytecode.set(size, code, code_alloc);
  if (size) {
    _bytecode_size = size;
    switch (code_alloc) {
    case STATIC:
      _bytecode = code;
      _bytecode_static = true;
      break;
    case DYNAMIC:
      _bytecode = code;
      _bytecode_static = false;
      break;
    case COPY:
      _bytecode = new uint8_t[_bytecode_size];
      std::memcpy(_bytecode, code, _bytecode_size);
      _bytecode_static = false;
      break;
    }
  }
}

const meat::uint8_t *meat::Class::bytecode() const {
  return _bytecode;
}

/***************************
 * meat::Class::get_vtable *
 ***************************/

const meat::vtable_entry_t *meat::Class::get_vtable(uint8_t &count) const {
  count = vtable.no_entries;
  return vtable.entries;
}

/*********************************
 * meat::Class::get_class_vtable *
 *********************************/

const meat::vtable_entry_t *
meat::Class::get_class_vtable(uint8_t &count) const {
  count = vtable.no_centries;
  return vtable.centries;
}

/**************************
 * meat::Class::serialize *
 **************************/

void meat::Class::serialize(data::Archive &store,
                            std::ostream &data_stream) const {
  throw Exception("Classes cannot be serialized");
}

/**********************
 * meat::Class::write *
 **********************/

void meat::Class::write(std::ostream &lib_file) const {

  if (_super.is_null()) {
    throw Exception("Attempting to write an incomplete class with no super.");
  }

  // Write my own hash id.
  meat::uint32_t cooked_id = endian::write_be(_hash_id);
  lib_file.write((const char *)&cooked_id, 4);

  // Write our super's hash_id.
  cooked_id = endian::write_be(meat::cast<const meat::Class>(_super)._hash_id);
  lib_file.write((const char *)&cooked_id, 4);

  /*  We only record the number of class properties there are. When the class
   * is unserialized all the class properties are initialize to Null.
   */
  lib_file.put((char)properties());

  // Record the number of properties for the objects.
  lib_file.put((char)obj_properties());

  // Write the virtual method table.
  vtable.write(lib_file);

  // Write the bytecode
  // Write the size of the bytecode.
  meat::uint16_t size_be = endian::write_be(_bytecode_size);
  lib_file.write((const char *)&size_be, 2);

  // Write the bytecode to the file.
  lib_file.write((const char *)_bytecode, _bytecode_size);
}

/***********************
 * meat::Class::import *
 ***********************/

meat::Class *meat::Class::import(std::istream &lib_file) {
  meat::uint32_t class_id;
  lib_file.read((char *)&class_id, 4);
  class_id = endian::read_be(class_id);

#ifdef DEBUG
  std::cout << "CLASS: Importing class id " << std::hex << class_id
            << std::dec << std::endl;
#endif /* DEBUG */

  meat::uint32_t super_id;
  lib_file.read((char *)&super_id, 4);
  Reference super = resolve(endian::read_be(super_id));

  meat::uint8_t class_props = lib_file.get();
  meat::uint8_t obj_props = lib_file.get();

  Class *cls = new Class(super, class_props, obj_props);
  cls->_hash_id = class_id;

  cls->vtable.read(lib_file);

  // Read in the size of the bytecode.
  lib_file.read((char *)&cls->_bytecode_size, 2);
  cls->_bytecode_size = endian::read_be(cls->_bytecode_size);

  // Allocate space for the bytecode and read it in.
  cls->_bytecode = new uint8_t[cls->_bytecode_size];
  lib_file.read((char *)cls->_bytecode, cls->_bytecode_size);

  cls->_bytecode_static = false;

  class_registry_t &classes = class_registry();
  if (classes.find(class_id) != classes.end()) {
    throw Exception("Class import conflict.");
  }

  return cls;
}

/***********************
 * meat::Class::lookup *
 ***********************/

std::string meat::Class::lookup(uint32_t hash_id) const {
  if (library) return library->lookup(hash_id);
  return itohex(hash_id);
}

/***********************
 * meat::Class::record *
 ***********************/

void meat::Class::record(Reference &cls, bool replace) {
  meat::uint32_t hash_id = cast<Class>(cls)._hash_id;

#ifdef DEBUG
  std::cout << "CLASS: Recording class as " << std::hex
            << std::showbase << hash_id << std::endl;
#endif

  class_registry_t &classes = class_registry();
  if (not replace && classes.find(hash_id) != classes.end()) {
    throw Exception((std::string("Class ") + cast<Class>(cls).name()) +
										" already registered.");
  }

  classes[hash_id] = cls;
  if (!cast<Class>(cls)._super.is_null())
    cast<Class>(cls).vtable.link(cast<Class>(cast<Class>(cls)._super));
}

void meat::Class::record(Class *cls, const char *id, bool replace) {
  meat::uint32_t hash_id = hash(id);

#ifdef DEBUG
  std::cout << "CLASS: Recording class " << id << " as id " << std::hex
            << std::showbase << hash_id << std::endl;
#endif

  class_registry_t &classes = class_registry();
  if (not replace && classes.find(hash_id) != classes.end()) {
    throw Exception((std::string("Class ") + cls->name()) +
										" already registered.");
  }

  classes[hash_id] = cls;
  cls->_hash_id = hash_id;
  if (!cls->_super.is_null())
    cls->vtable.link(cast<Class>(cls->_super));
}

void meat::Class::record(Reference &cls, const char *id, bool replace) {
  meat::uint32_t hash_id = hash(id);
  Class &thecls = cast<Class>(cls);

#ifdef DEBUG
  std::cout << "CLASS: Recording class " << id << " as id " << std::hex
            << std::showbase << hash_id << std::endl;
#endif

  class_registry_t &classes = class_registry();
  if (not replace && classes.find(hash_id) != classes.end()) {
    throw Exception((std::string("Class ") + cast<Class>(cls).name()) +
										" already registered.");
  }

  classes[hash_id] = cls;
  thecls._hash_id = hash_id;
  if (!thecls._super.is_null())
    thecls.vtable.link(cast<Class>(thecls._super));
}

/*************************
 * meat::Class::unrecord *`
 *************************/

void meat::Class::unrecord(Reference &cls) {

#ifdef DEBUG
  std::cout << "CLASS: Unrecording class " << cast<Class>(cls).name()
            << std::endl;
#endif
  class_registry().erase(cast<Class>(cls)._hash_id);
}

/***********************
 * meat::Class::relink *
 ***********************/

void meat::Class::relink() {
#ifdef DEBUG
  std::cout << "CLASS: Relinking class " << name() << std::endl;
#endif
  if (!_super.is_null())
    vtable.link(cast<Class>(_super));
}

/************************
 * meat::Class::resolve *
 ************************/

meat::Reference &meat::Class::resolve(const std::string &id) {
  meat::uint32_t hash_id = hash(id.c_str());

  class_registry_t &classes = class_registry();
  if (classes.find(hash_id) == classes.end()) {
    throw Exception((std::string("Class ") + id) +
                     " was not found.");
  }

  return classes[hash_id];
}

meat::Reference &meat::Class::resolve(meat::uint32_t hash_id) {
  class_registry_t &classes = class_registry();
  std::stringstream msg;

  msg << "Resolving class 0x";
  msg << std::setw(8) << std::setfill('0') << std::setbase(16);
  msg << hash_id << " failed.";
  if (classes.find(hash_id) == classes.end()) {
    throw Exception(msg.str());
  }

  return classes[hash_id];
}

/***************************
 * meat::Class::have_class *
 ***************************/

bool meat::Class::have_class(const std::string &id) {
  class_registry_t &classes = class_registry();
  return (classes.find(hash(id.c_str())) != classes.end());
}

/*********************
 * meat::Class::find *
 *********************/

const meat::vtable_entry_t *
meat::Class::find(meat::uint32_t hash_id) const {
  return vtable.find(hash_id);
}

const meat::vtable_entry_t *
meat::Class::class_find(meat::uint32_t hash_id) const {
  return vtable.class_find(hash_id);
}

/*********************************************
 * meat::Class::operator meat::Reference &() *
 *********************************************/

meat::Class::operator meat::Reference &() const {
  return resolve(_hash_id);
}

/******************************************************************************
 * meat::Class::VTable Class
 */

/*******************************
 * meat::Class::VTable::VTable *
 *******************************/

meat::Class::VTable::VTable() {
  no_entries = 0;
  entries = (vtable_entry_t *)0;
  e_is_static = true;
  no_centries = 0;
  centries = (vtable_entry_t *)0;
  ce_is_static = true;
  constructor = 0;
}

/********************************
 * meat::Class::VTable::~VTable *
 ********************************/

meat::Class::VTable::~VTable() throw() {
  if (!e_is_static && no_entries > 0)
    delete[] entries;
  if (!ce_is_static && no_centries > 0)
    delete[] centries;
}

/***********************************
 * meat::Class::Vtable::set_vtable *
 ***********************************/

void meat::Class::VTable::set_vtable(uint8_t entries, vtable_entry_t table[],
                                     alloc_t table_alloc) {

#ifdef TESTING
  // All we can really test here is the entries are sorted by the hash value.
  meat::test::test("Class virtual table validation", false);
  for (uint8_t c = 1; c < entries; c++) {
    if (table[c - 1].hash_id >= table[c].hash_id) {
      meat::test::failed("Class virtual table validation", false);
      break;
    }
  }
#endif

  no_entries = entries;
  switch (table_alloc) {
  case STATIC: // The table is a part of the code and not deallocation is needed.
    this->entries = table;
    e_is_static = true;
    break;
  case DYNAMIC: // The table deallocation is now managed by us.
    this->entries = table;
    e_is_static = false;
    break;
  case COPY: // Make a copy of the table and manage deallocation.
    this->entries = new vtable_entry_t[no_entries];
    std::memcpy(this->entries, table, sizeof(vtable_entry_t) * no_entries);
    e_is_static = false;
    break;
  }
}

/*****************************************
 * meat::Class::Vtable::set_class_vtable *
 *****************************************/

void meat::Class::VTable::set_class_vtable(uint8_t entries,
                                           vtable_entry_t table[],
                                           alloc_t table_alloc) {
#ifdef TESTING
  // All we can really test here is the entries are sorted by the hash value.
  meat::test::test("Class class virtual table validation", false);
  for (uint8_t c = 1; c < entries; c++) {
    if (table[c - 1].hash_id >= table[c].hash_id) {
      meat::test::failed("Class class virtual table validation", false);
      break;
    }
  }
#endif

  no_centries = entries;
  switch (table_alloc) {
  case STATIC:
    // The table is a part of the code and deallocation is not needed.
    this->centries = table;
    ce_is_static = true;
    break;
  case DYNAMIC: // The table deallocation is now managed by us.
    this->centries = table;
    ce_is_static = false;
    break;
  case COPY: // Make a copy of the table and manage deallocation.
    this->centries = new vtable_entry_t[no_centries];
    std::memcpy(this->centries, table, sizeof(vtable_entry_t) * no_centries);
    ce_is_static = false;
    break;
  }
}

/****************************
 * meat::Class::Vtable::set *
 ****************************/

void meat::Class::VTable::set(constructor_t constructor) {
#ifdef TESTING
  meat::test::test("Setting vtable constructor procedure", false);
  if (constructor == 0)
    meat::test::failed("Setting vtable constructor procedure", false);
#endif /* TESTING */
  this->constructor = constructor;
}

/*****************************
 * meat::Class::Vtable::link *
 *****************************/

void meat::Class::VTable::link(Class &super) {
#ifdef DEBUG
  std::cout << "VTABLE: Linking" << std::endl;
#endif
  for (unsigned int c = 0; c < no_entries; ++c) {
    if (entries[c].flags & VTM_SUPER) {
      vtable_entry_t *other = super.vtable.find(entries[c].hash_id);
      if (other) {
        std::memcpy(&entries[c], other, sizeof(vtable_entry_t));
        entries[c].flags |= VTM_SUPER;
      }
#ifdef DEBUG
      else
        std::cout << "VTABLE: Linking error for "
                  << std::hex << entries[c].hash_id << std::dec
                  << std::endl;
#endif
    }
  }
  for (unsigned int c = 0; c < no_centries; ++c) {
    if (centries[c].flags & VTM_SUPER) {
      vtable_entry_t *other = super.vtable.class_find(centries[c].hash_id);
      if (other) {
        std::memcpy(&centries[c], other, sizeof(vtable_entry_t));
        centries[c].flags |= VTM_SUPER;
      }
#ifdef DEBUG
      else
        std::cout << "VTABLE: Class linking error for "
                  << std::hex << centries[c].hash_id << std::dec
                  << std::endl;
#endif
    }
  }
}

/*****************************
 * meat::Class::Vtable::find *
 *****************************/

meat::vtable_entry_t *
meat::Class::VTable::find(uint32_t hash_id) {
  register meat::int16_t lo = 0, hi = no_entries - 1, pivot;

  /*  Simple binary search for entries. Note that all the entries must be
   * sorted.
   */
  if (no_entries > 0) {
    while (lo <= hi) {
      pivot = (hi + lo) >> 1;
      meat::int32_t res = hash_id - entries[pivot].hash_id;
      if (res == 0) return &entries[pivot];
      else if (res < 0) hi = pivot - 1;
      else if (res > 0) lo = pivot + 1;
    }
  }
  return 0;
}

const meat::vtable_entry_t *
meat::Class::VTable::find(uint32_t hash_id) const {
  register meat::int16_t lo = 0, hi = no_entries - 1, pivot;

  /*  Simple binary search for entries. Note that all the entries must be
   * sorted.
   */
  if (no_entries > 0) {
    while (lo <= hi) {
      pivot = (hi + lo) >> 1;
      meat::int32_t res = hash_id - entries[pivot].hash_id;
      if (res == 0) return &entries[pivot];
      else if (res < 0) hi = pivot - 1;
      else if (res > 0) lo = pivot + 1;
    }
  }
  return 0;
}

/***********************************
 * meat::Class::Vtable::class_find *
 ***********************************/

meat::vtable_entry_t *
meat::Class::VTable::class_find(meat::uint32_t hash_id) {
  register int16_t lo = 0, hi = no_centries, pivot;

  /*  Simple binary search for class entries. Note that all the class
   * entries must be sorted.
   */
  if (no_centries > 0) {
    while (lo <= hi) {
      pivot = (hi + lo) >> 1;
      int32_t res = hash_id - centries[pivot].hash_id;
      if (res == 0) return &centries[pivot];
      else if (res < 0) hi = pivot - 1;
      else if (res > 0) lo = pivot + 1;
    }
  }
  return 0;
}

const meat::vtable_entry_t *
meat::Class::VTable::class_find(meat::uint32_t hash_id) const {
  register int16_t lo = 0, hi = no_centries, pivot;

  /*  Simple binary search for class entries. Note that all the class
   * entries must be sorted.
   */
  if (no_centries > 0) {
    while (lo <= hi) {
      pivot = (hi + lo) >> 1;
      int32_t res = hash_id - centries[pivot].hash_id;
      if (res == 0) return &centries[pivot];
      else if (res < 0) hi = pivot - 1;
      else if (res > 0) lo = pivot + 1;
    }
  }
  return 0;
}

/******************************
 * meat::Class::Vtable::write *
 ******************************/

void meat::Class::VTable::write(std::ostream &lib_file) const {
  static unsigned int zero = 0;

  lib_file.put(no_entries);
  lib_file.put(no_centries);

  for (meat::uint8_t c = 0; c < no_entries; c++) {
    if ((entries[c].flags & VTM_BYTECODE) == 0 &&
        !(entries[c].flags & VTM_SUPER)) {
      throw Exception("I can't serialize native classes.");
    }

    meat::uint32_t cooked_hash = endian::write_be(entries[c].hash_id);
    lib_file.write((const char *)&cooked_hash, 4);
    cooked_hash = endian::write_be(entries[c].class_id);
    lib_file.write((const char *)&cooked_hash, 4);

    if (entries[c].flags & VTM_SUPER) {
      lib_file.put(VTM_SUPER);
      lib_file.write((const char *)&zero, 3);
    } else {
      lib_file.put(entries[c].flags);
      lib_file.put(entries[c].locals);

      meat::uint16_t cooked_offset =
        endian::write_be((uint16_t)entries[c].method.offset);
      lib_file.write((const char *)&cooked_offset, 2);
    }
  }

  // Write the class methods virtual table.
  for (meat::uint8_t c = 0; c < no_centries; c++) {
    if ((centries[c].flags & VTM_BYTECODE) == 0 &&
        !(centries[c].flags & VTM_SUPER)) {
      throw Exception("I can't serialize native classes.");
    }

    meat::uint32_t cooked_hash = endian::write_be(centries[c].hash_id);
    lib_file.write((const char *)&cooked_hash, 4);
    cooked_hash = endian::write_be(centries[c].class_id);
    lib_file.write((const char *)&cooked_hash, 4);

    if (centries[c].flags & VTM_SUPER) {
      lib_file.put(VTM_SUPER);
      lib_file.write((const char *)&zero, 3);
    } else {
      lib_file.put(centries[c].flags);
      lib_file.put(centries[c].locals);

      meat::uint16_t cooked_offset =
        endian::write_be((uint16_t)centries[c].method.offset);
      lib_file.write((const char *)&cooked_offset, 2);
    }
  }
}

/*****************************
 * meat::Class::Vtable::read *
 *****************************/

void meat::Class::VTable::read(std::istream &lib_file) {
  // TODO Should make sure old tables are deallocated first.

  // Read in the virtual table sizes.
  no_entries = lib_file.get();
  no_centries = lib_file.get();

  // Read in the object method virtual table.
  entries = new vtable_entry_t[no_entries];
  for (meat::uint8_t c = 0; c < no_entries; c++) {
    meat::uint32_t hash_id, class_id;
    lib_file.read((char *)&hash_id, 4);
    entries[c].hash_id = endian::read_be(hash_id);
    lib_file.read((char *)&class_id, 4);
    entries[c].class_id = endian::read_be(class_id);

    entries[c].flags = lib_file.get();
    entries[c].locals = lib_file.get();

    meat::uint16_t offset;
    lib_file.read((char *)&offset, 2);
    entries[c].method.offset = endian::read_be(offset);
  }
  e_is_static = false;

  // Read in the class methods virtual table.
  centries = new vtable_entry_t[no_centries];
  for (meat::uint8_t c = 0; c < no_centries; c++) {
    meat::uint32_t hash_id, class_id;
    lib_file.read((char *)&hash_id, 4);
    centries[c].hash_id = endian::read_be(hash_id);
    lib_file.read((char *)&class_id, 4);
    centries[c].class_id = endian::read_be(class_id);

    centries[c].flags = lib_file.get();
    centries[c].locals = lib_file.get();

    meat::uint16_t offset;
    lib_file.read((char *)&offset, 2);
    centries[c].method.offset = endian::read_be(offset);
  }
  ce_is_static = false;
}

/******************************************************************************
 * meat::Context Class
 */

static const meat::uint8_t CFL_BYTECODE = 0x00;
static const meat::uint8_t CFL_NATIVE   = 0x01;

/**************************
 * meat::Context::Context *
 **************************/

meat::Context::Context(meat::uint8_t locals)
  : Object(Class::resolve("Context")), _result_index(0), done(false) {
  num_of_locals = locals + 4;
  this->_locals = new Reference[num_of_locals];
}

meat::Context::Context(Reference context, meat::uint8_t locals)
  : Object(Class::resolve("Context")), _messenger(context), _result_index(0),
    done(false) {
  num_of_locals = locals + 4;
  this->_locals = new Reference[num_of_locals];
}

meat::Context::Context(Reference cls,
                       Reference context,
                       meat::uint8_t locals)
  : Object(cls), _messenger(context), _result_index(0), done(false) {
  num_of_locals = locals + 4;
  this->_locals = new Reference[num_of_locals];
}

/***************************
 * meat::Context::~Context *
 ***************************/

meat::Context::~Context() throw() {
  delete [] _locals;
}

/***********************
 * meat::Context::self *
 ***********************/

meat::Reference meat::Context::self() const {
  return _locals[0];
}

void meat::Context::self(Reference new_self) {
  _locals[0] = new_self;
}

/****************************
 * meat::Context::messenger *
 ****************************/

meat::Reference meat::Context::messenger() const {
  return _messenger;
}

void meat::Context::messenger(Reference context) {
  _messenger = context;
}

/****************************
 * meat::Context::parameter *
 ****************************/

meat::Reference meat::Context::parameter(uint8_t index) const {
  return _locals[index + 4];
}

void meat::Context::parameter(uint8_t index, Reference value) {
  _locals[index + 4] = value;
}

/************************
 * meat::Context::local *
 ************************/

void meat::Context::local(uint8_t index, Reference value) {
  if (index < num_of_locals) _locals[index] = value;
  else {
    std::stringstream msg;
    msg << "Setting local variable " << (unsigned int)index
        << " out of range (0-" << (unsigned int)(num_of_locals - 1) << ")";
    throw Exception(msg.str());
  }
}

meat::Reference meat::Context::local(uint8_t index) const {
  if (index >= 0 and index < num_of_locals) return _locals[index];
  else {
    std::stringstream msg;
    msg << "Getting local variable " << (unsigned int)index
        << " out of range (0-" << (unsigned int)(num_of_locals - 1) << ")";
    throw Exception(msg.str());
  }
}

/*******************************
 * meat::Context::result_index *
 *******************************/

void meat::Context::result_index(uint8_t local_parent_index) {
  _result_index = local_parent_index;
  cast<Context>(_messenger).local(_result_index, meat::Null());
}

/*************************
 * meat::Context::result *
 *************************/

void meat::Context::result(Reference value) {
  _result = value;
  if (_result_index)
    cast<Context>(_messenger).local(_result_index, _result);
}

meat::Reference meat::Context::result() const {
  return _result;
}

/******************************************************************************
 * meat::BlockContext Class
 */

#define BCFL_BREAK (0x01)
#define BCFL_CONTINUE (0x02)
#define BCFL_BREAK_TRAP (0x04)
#define BCFL_CONT_TRAP (0x08)

/************************************
 * meat::BlockContext::BlockContext *
 ************************************/

meat::BlockContext::BlockContext(Reference context, meat::uint8_t locals,
                                 uint16_t ip)
  : Context(Class::resolve("BlockContext"), context, locals),
    _origin(context), bc_flags(0), start_ip(ip) {
  flags = meat::Context::BYTECODE;
  this->_ip = ip;
}

/*************************************
 * meat::BlockContext::~BlockContext *
 *************************************/

meat::BlockContext::~BlockContext() throw() {
}

/*****************************
 * meat::BlockContext::local *
 *****************************/

void meat::BlockContext::local(meat::uint8_t index,
															 meat::Reference value) {
  meat::uint8_t local_cnt = cast<Context>(_origin).locals();

  if (index < local_cnt)
    cast<Context>(_origin).local(index, value);
  else
    Context::local(index - local_cnt, value);
}

meat::Reference meat::BlockContext::local(meat::uint8_t index) const {
  meat::uint8_t local_cnt = cast<const Context>(_origin).locals();

  //if (index == 3) return self

  if (index < local_cnt)
    return cast<const Context>(_origin).local(index);
  else
    return Context::local(index - local_cnt);
}

/******************************
 * meat::BlockContext::locals *
 ******************************/

meat::uint8_t meat::BlockContext::locals() const {
  return cast<const Context>(_origin).locals() + Context::locals();
}

/*******************************
 * meat::BlockContext::c_break *
 *******************************/

void meat::BlockContext::c_break() {
  bc_flags |= BCFL_BREAK;
  finish();
}

void meat::BlockContext::c_continue() {
  bc_flags |= BCFL_CONTINUE;
  finish();
}

void meat::BlockContext::set_break_trap() {
  bc_flags |= BCFL_BREAK_TRAP;
}

void meat::BlockContext::set_continue_trap() {
  bc_flags |= BCFL_CONT_TRAP;
}

bool meat::BlockContext::break_trap_set() const {
  return bc_flags & BCFL_BREAK_TRAP;
}

bool meat::BlockContext::continue_trap_set() const {
  return bc_flags & BCFL_CONT_TRAP;
}

bool meat::BlockContext::break_called() const {
  return bc_flags & BCFL_BREAK;
}

bool meat::BlockContext::continue_called() const {
  return bc_flags & BCFL_CONTINUE;
}

void meat::BlockContext::reset() {
  bc_flags = 0;
  _ip = start_ip;
  unfinish();
}

/******************************************************************************
 * meat::Exception Class
 */

/******************************
 * meat::Exception::Exception *
 ******************************/

meat::Exception::Exception()
  : Object(Class::resolve("Exception"), 2) {
  // Empty Exception
}

meat::Exception::Exception(const Exception &other)
  : Object(Class::resolve("Exception"), 2) {
  this->property(0) = other.property(0);
  this->property(1) = other.property(1);
}

meat::Exception::Exception(Reference cls, meat::uint8_t properties)
  : Object(cls, properties) {
  // Empty Exception constructor used for class inheritance.
}

meat::Exception::Exception(const std::string &message)
  : Object(Class::resolve("Exception"), 2) {
  // New exception with a meat Text message.
  this->property(0) = new Text(message);
}

meat::Exception::Exception(const std::string &message, Reference context)
  : Object(Class::resolve("Exception"), 2) {
  this->property(0) = new Text(message);
  this->property(1) = context;
}

meat::Exception::Exception(Reference message, Reference context)
  : Object(Class::resolve("Exception"), 2) {
  this->property(0) = message;
  this->property(1) = context;
}

/*************************
 * meat::Exception::what *
 *************************/

const char* meat::Exception::what() const throw() {
  try {
    // Attempt to get the error message from property 0.
    if (this->property(0) == Null())
      return "";
    else
      return cast<const Text>(this->property(0)).c_str();
  } catch (...) {
    // If property 0 cannot be converted to a string then return nothing.
    return "";
  }
}

/******************************************************************************
 * Value Class Implemenation
 */

/**********************
 * meat::Value::Value *
 **********************/

meat::Value::Value(Reference cls, uint8_t properties)
  : Object(cls, properties), data_type(INTEGER) {
  data.i = 0;
}

meat::Value::Value(int32_t value)
  : Object(Class::resolve("Integer")), data_type(INTEGER) {
  data.i = value;
}

meat::Value::Value(float_t value)
  : Object(Class::resolve("Number")), data_type(FLOAT) {
  data.f = value;
}

meat::Value::Value(bool value)
  : Object(Class::resolve("Boolean")), data_type(BOOLEAN) {
  data.b = value;
}

/**************************
 * meat::Value::serialize *
 **************************/

void meat::Value::serialize(data::Archive &store,
                            std::ostream &data_stream) const {
  switch (data_type) {
  case BOOLEAN:
    store << (uint8_t)data.b;
    break;
  case INTEGER:
    store << data.i;
    break;
  case FLOAT:
    store << data.f;
    break;
  default:
    break;
  }
}

/****************************
 * meat::Value::unserialize *
 ****************************/

void meat::Value::unserialize(data::Archive &store,
                              std::istream &data_stream) {
  if (this->is_type("Integer")) {
    data_type = INTEGER;
    store >> data.i;
  } else if (this->is_type("Number")) {
    data_type = FLOAT;
    store >> data.f;
  }
}

meat::Value::operator meat::int32_t() const {
  switch (data_type) {
  case INTEGER:
    return data.i;
  case FLOAT:
    return data.f;
  default:
    throw Exception("Invalid conversion of an Object to an integer.");
  }
}

meat::Value::operator meat::float_t() const {
  switch (data_type) {
  case INTEGER:
    return data.i;
  case FLOAT:
    return data.f;
  default:
    throw Exception("Invalid conversion of an Object to a float.");
  }
}

/******************************************************************************
 * Text Class Implemenation
 */

/********************
 * meat::Text::Text *
 ********************/

meat::Text::Text() : Object(Class::resolve("Text")) {
}

meat::Text::Text(const Text &other)
  : std::string(other), Object(Class::resolve("Text")) {
}

meat::Text::Text(Reference cls, uint8_t properties)
  : Object(cls, properties) {
}

meat::Text::Text(const std::string &value)
  : std::string(value), Object(Class::resolve("Text")) {
}

meat::Text::Text(const char *value)
  : std::string(value), Object(Class::resolve("Text")) {
}

/*************************
 * meat::Text::serialize *
 *************************/

void meat::Text::serialize(data::Archive &store,
                           std::ostream &data_stream) const {
  data_stream.write(c_str(), length());
  data_stream.put(0);
}

/***************************
 * meat::Text::unserialize *
 ***************************/

void meat::Text::unserialize(data::Archive &store,
                             std::istream &data_stream) {
  clear();
  unsigned char ch;
  while ((ch = data_stream.get()) != 0)
    *this += ch;
}

/******************************************************************************
 * List Class Implemenation
 */

/********************
 * meat::List::List *
 ********************/

meat::List::List() : Object(Class::resolve("List")) {
}

meat::List::List(Reference cls, meat::uint8_t properties)
  : Object(cls, properties) {
}

/*************************
 * meat::List::serialize *
 *************************/

void meat::List::serialize(data::Archive &store,
                           std::ostream &data_stream) const {
  // Record the size of the list.
  store << (uint32_t)size();

  // Add all the list entries to the store.
  for (unsigned int c = 0; c < size(); c++) {
    Reference obj = at(c);
    store << store.add_property(obj);
  }
}

/***************************
 * meat::List::unserialize *
 ***************************/

void meat::List::unserialize(data::Archive &store,
                             std::istream &data_stream) {
  // Read in the size of the list
  uint32_t elements;
  store >> elements;

  // Read in all the Objects that are in the list.
  meat::uint32_t index;
  for (unsigned int c = 0; c < elements; c++) {
    store >> index;
    push_back(store.get_object(index));
  }
}

/******************************************************************************
 * Set Class Implemenation
 */

/******************
 * meat::Set::Set *
 ******************/

meat::Set::Set() : Object(Class::resolve("Set")) {
}

meat::Set::Set(Reference cls, meat::uint8_t properties)
  : Object(cls, properties) {
}

/************************
 * meat::Set::serialize *
 ************************/

void meat::Set::serialize(data::Archive &store,
                          std::ostream &data_stream) const {
  // Record the size of the list.
  store << (uint32_t)size();

  // Add all the list entries to the store.
  meat::Set::const_iterator it = begin();
  for (; it != end(); ++it)
    store << store.add_property(*it);
}

/**************************
 * meat::Set::unserialize *
 **************************/

void meat::Set::unserialize(data::Archive &store,
                            std::istream &data_stream) {
  // Read in the size of the list
  uint32_t elements;
  store >> elements;

  // Read in all the Objects that are in the list.
  meat::uint32_t index;
  for (unsigned int c = 0; c < elements; c++) {
    store >> index;
    insert(store.get_object(index));
  }
}

/******************************************************************************
 * Index Class Implemenation
 */

bool meat::obj_less::operator()(const Reference &first,
                                const Reference &second) const {
  Reference ctx = message((Reference)first, "<", meat::Null());
  cast<Context>(ctx).parameter(0, (Reference)second);
  Reference result = execute(ctx);
  if (result == meat::BTrue())
    return true;
  return false;
}

/**********************
 * meat::Index::Index *
 **********************/

meat::Index::Index() : Object(Class::resolve("Index")) {
}

meat::Index::Index(Reference cls, meat::uint8_t properties)
  : Object(cls, properties) {
}

/**************************
 * meat::Index::serialize *
 **************************/

void meat::Index::serialize(data::Archive &store,
                            std::ostream &data_stream) const {
  // Record the size of the list.
  store << (uint32_t)size();

  // Add all the index entries to the store.
  for (const_iterator it = begin(); it != end(); it++) {
    Reference key = it->first;
    Reference value = it->second;

    store << store.add_property(key);
    store << store.add_property(value);
  }
}

/****************************
 * meat::Index::unserialize *
 ****************************/

void meat::Index::unserialize(data::Archive &store,
                              std::istream &data_stream) {
  // Read in the size of the list
  uint32_t elements;
  store >> elements;

  // Read in all the Objects that are in the list.
  meat::uint32_t key_index, val_index;
  for (unsigned int c = 0; c < elements; c++) {
    store >> key_index;
    store >> val_index;

    (*this)[store.get_object(key_index)] = store.get_object(val_index);
  }
}

/******************************************************************************
 */

/*****************
 * meat::message *
 *****************/

meat::Reference meat::message(meat::Reference object,
                              meat::uint32_t hash_id,
                              meat::Reference context) {

  Reference cls, obj_cls;
  const vtable_entry_t *m_entry = 0;

#ifdef TESTING
  meat::test::test("Messaging Object", false);
  if (object.is_null()) {
    meat::test::failed("Messaging object", false);
    throw Exception("Unable to message a null Reference");
  }
#endif

  // Resolve the message with the hash_id.
  if (object->is_class()) {
    obj_cls = cls = object;

    /*  If the method is inherited then start looking through the parent
     * classes to find the actual method entry.
     */
    while (m_entry == 0 and !cls.is_null()) {
      m_entry = ((Class &)(*cls)).class_find(hash_id);
      if (m_entry == 0) cls = cast<Class>(cls).super();
    }

  } else {
    obj_cls = cls = object->type();

    /*  If the method is inherited then start looking through the parent
     * classes to find the actual method entry.
     */
    while (m_entry == 0 and !cls.is_null()) {
      m_entry = cast<Class>(cls).find(hash_id);
      if (m_entry == 0) cls = cast<Class>(cls).super();
    }
  }

  // Raise an error if we couldn't resolve the method name.
  if (m_entry == 0)
    throw Exception(std::string("Message ") +
                    cast<Class>(obj_cls).lookup(hash_id) +
                    " is unresolved", context);

  // Now create the new context.
  Context *ctx = new Context(context, m_entry->locals);
  Reference new_context(ctx);

  // Populate the mandatory local objects.
  ctx->_locals[0] = object;                                  // self
  ctx->_locals[1] = meat::Class::resolve(m_entry->class_id); // class
  ctx->_locals[3] = meat::Null();                            // null

  if ((m_entry->flags & VTM_BYTECODE) == 0) {
    // Flag for native method.
    ctx->flags = meat::Context::PRIMATIVE;
    ctx->pointer = m_entry->method.pointer;
  } else {
    // Flag for bytecode method.
    ctx->flags = meat::Context::BYTECODE;
    ctx->_ip = m_entry->method.offset;
  }

  // Return the new context.
  return new_context;
}

meat::Reference meat::message(meat::Reference object,
                              const char *method,
                              meat::Reference context) {
  return message(object, hash(method), context);
}

/***********************
 * meat::message_super *
 ***********************/

meat::Reference meat::message_super(meat::Reference object,
                                    meat::uint32_t hash_id,
                                    meat::Reference context) {

  Reference cls;
  const vtable_entry_t *m_entry = 0;

  // Resolve the message with the hash_id.
  if (object->is_class()) {
    cls = cast<Context>(context).super();

    /*  If the method is inherited then start looking through the parent
     * classes to find the actual method entry.
     */
    while (m_entry == 0 and !cls.is_null()) {
      m_entry = cast<Class>(cls).class_find(hash_id);
      if (m_entry == 0) cls = cast<Class>(cls).super();
    }

  } else {
    cls = cast<Context>(context).super();

    /*  If the method is inherited then start looking through the parent
     * classes to find the actual method entry.
     */
    while (m_entry == 0 and !cls.is_null()) {
      m_entry = cast<Class>(cls).find(hash_id);
      if (m_entry == 0) cls = cast<Class>(cls).super();
    }
  }

  // Raise an error if we couldn't resolve the method name.
  if (m_entry == 0)
    throw Exception(std::string("Message is unresolved"), context);

  // Now create the new context.
  Context *ctx = new Context(context, m_entry->locals);
  Reference new_context(ctx);

  // Populate the mandatory local objects.
  ctx->_locals[0] = object;                                  // self
  ctx->_locals[1] = meat::Class::resolve(m_entry->class_id); // class
  ctx->_locals[3] = meat::Null();                            // null

  if ((m_entry->flags & VTM_BYTECODE) == 0) {
    // Flag for native method.
    ctx->flags = meat::Context::PRIMATIVE;
    ctx->pointer = m_entry->method.pointer;
  } else {
    // Flag for bytecode method.
    ctx->flags = meat::Context::BYTECODE;
    ctx->_ip = m_entry->method.offset;
  }

  // Return the new context.
  return new_context;
}

#ifdef DEBUG
std::ostream &meat::operator <<(std::ostream &out, Class &cls) {
  return (out << "CLASS<" << cls.name() << ">");
}
#endif

/********************
 * meat::ClassClass *
 ********************/

meat::Reference meat::ClassClass(bool initializing) {
  static Reference cls;

  if (cls.is_null() and not initializing) {
    cls = Class::resolve("Class");
  }

  return cls;
}

/**************
 * meat::True *
 **************/

meat::Reference meat::BTrue() {
  static Reference true_object;

  if (true_object.is_null()) {
    true_object = Class::resolve("Boolean")->property(0);
  }

  return true_object;
}

/***************
 * meat::False *
 ***************/

meat::Reference meat::BFalse() {
  static Reference false_object;

  if (false_object.is_null()) {
    false_object = Class::resolve("Boolean")->property(1);
  }

  return false_object;
}

/*****************
 * meat::Boolean *
 *****************/

meat::Reference meat::Boolean(bool value) {
  static Reference true_object;
  static Reference false_object;

  if (true_object.is_null()) {
    true_object = Class::resolve("Boolean")->property(0);
  }
  if (false_object.is_null()) {
    false_object = Class::resolve("Boolean")->property(1);
  }

  return (value ? true_object : false_object);
}

/**************
 * meat::Null *
 **************/

meat::Reference meat::Null() {
  static Reference null_object;

  if (null_object.is_null()) {
    /*  We need to be a little more hands on here. As the initial object and
     * Class class objects are created they need some kind of null value before
     * the Null object and class are created. The normal way will cause
     * Exceptions before there is even a meat Exception class. So if
     * initially someone wants a Null object before there is a Null object we
     * just return a null reference.
     */
    meat::uint32_t hash_id = hash("Null");
    class_registry_t &classes = class_registry();

    if (classes.find(hash_id) != classes.end()) {
      null_object = classes[hash_id]->property(0);
    }
  }

  return null_object;
}
