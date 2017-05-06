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

meat::Object::Object(Reference &type) {
#ifdef TESTING
	meat::Test::test("Object type setting", false);
	if (!type.is_null()) {
		if (!type->is_class())
			throw Exception("An Object type must be a class.");
	} else {
		meat::Test::failed("Object type setting", false);
	}
#endif
	o_type = type;
	num_of_props = CLASS(type).get_obj_properties();
	if (num_of_props > 0) {
		// Initialize the properties and set them all to Null.
		properties = new Reference[num_of_props];
		for (unsigned int c = 0; c < num_of_props; c++)
			this->properties[c] = Null();
	} else
		properties = 0;
	data_type = PROPSONLY;
}

meat::Object::Object(Reference &type, meat::uint8_t properties) {
#ifdef TESTING
	static bool first_fail = true;

	meat::Test::test("Object type setting with properties", false);
	if (!type.is_null()) {
		if (!type->is_class())
			throw Exception("An Object type must be a class.");
	} else {
		if (not first_fail)
			meat::Test::failed("Object type setting with properties", false);
		first_fail = false;
	}
#endif
	o_type = type;
	num_of_props = properties;
	if (properties > 0) {
		// Initialize the properties and set them all to Null.
		this->properties = new Reference[num_of_props];
		for (unsigned int c = 0; c < num_of_props; c++)
			this->properties[c] = Null();
	} else
		this->properties = 0;
	data_type = PROPSONLY;
}

meat::Object::Object(bool value) {
	// Create a new boolean Object.
	o_type = Class::resolve("Boolean");
	num_of_props = 0;
	properties = 0;
	data_type = BOOLEAN;
	data.b = value;
}

meat::Object::Object(meat::int32_t value) {
	// Create a new integer Object.
	o_type = Class::resolve("Integer");
	num_of_props = 0;
	properties = 0;
	data_type = INTEGER;
	data.i = value;
}

meat::Object::Object(meat::float_t value) {
	// Create a new float Object
	o_type = Class::resolve("Number");
	num_of_props = 0;
	properties = 0;
	data_type = FLOAT;
	data.f = value;
}

meat::Object::Object(const char *value) {
	// Create a new text Object
	o_type = Class::resolve("Text");
	num_of_props = 0;
	properties = 0;
	data_type = STRING;
	data.s = new char[strlen(value) + 1];
	std::strcpy(data.s, value);
}

meat::Object::Object(const char *value, const char *value2) {
	// Create a new text Object conjugating the two strings.
	o_type = Class::resolve("Text");
	num_of_props = 0;
	properties = 0;
	data_type = STRING;
	data.s = new char[strlen(value) + strlen(value2) + 1];
	std::strcpy(data.s, value);
	std::strcat(data.s, value2);
}

/*************************
 * meat::Object::~Object *
 *************************/

meat::Object::~Object() throw () {
	// Clean up all the properties
	if (properties)
		delete [] properties;

	// Clean up any text if the Object was a text type.
	if (data_type == STRING)
		delete [] data.s;
}

/*************************
 * meat::Object::is_type *
 *************************/

bool meat::Object::is_type(Reference &cls) const {
	Reference my_type = o_type;

#ifdef TESTING
	meat::Test::test("Type testing against a class", false);
	if (cls.is_null() or not cls->is_class())
		meat::Test::failed("Type testing against a class", false);
#endif

	while (!my_type.is_null()) {
		if (my_type == cls)
			return true;
		my_type = CLASS(my_type).get_super();
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
	// Serialize the Objects data
	store << (uint8_t)data_type;

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
	case STRING:
		data_stream.write(data.s, strlen(data.s));
		data_stream.put(0);
		break;
	default:
		break;
	}
}

/*****************************
 * meat::Object::unserialize *
 *****************************/

void meat::Object::unserialize(data::Archive &store,
															 std::istream &data_stream) {
	uint8_t temp;
	store >> temp;
	data_type = (data_type_t)temp;

	switch (data_type) {
	case BOOLEAN:
		store >> temp;
		data.b = (bool)temp;
		break;
	case INTEGER:
		store >> data.i;
		break;
	case FLOAT:
		store >> data.f;
		break;
	case STRING: {
		std::string temp;
		char ch;
		while ((ch = data_stream.get()) != 0)
			temp += ch;
		data.s = new char[temp.length() + 1];
		std::strcpy(data.s, temp.c_str());
		break;
	}
	default:
		break;
	}
}

/**************************
 * meat::Object::property *
 **************************/

meat::Reference &meat::Object::property(meat::uint8_t index) {
	if (index < num_of_props)
		return properties[index];
	throw Exception("Invalid property index.");
}


meat::Reference &meat::Object::property(meat::uint8_t index) const {
	if (index < num_of_props)
		return properties[index];
	throw Exception("Invalid property index.");
}

/*****************************
 * meat::Object::operator == *
 *****************************/

bool meat::Object::operator ==(const char *value) const {
	return std::strcmp(to_string(), value) == 0;
}

/****************************************
 * meat::Object::operator meat::Class & *
 ****************************************/

meat::Object::operator meat::Class &() {
	throw Exception("Invalid conversion of an Object to a Class.");
}

/******************************************
 * meat::Object::operator meat::Context & *
 ******************************************/

meat::Object::operator meat::Context &() {
	throw Exception("Invalid conversion of an Object to a Context.");
}

/************************
 * meat::Object::to_int *
 ************************/

meat::int32_t meat::Object::to_int() const {
	switch (data_type) {
	case INTEGER:
		return data.i;
	case FLOAT:
		return data.f;
	default:
		throw Exception("Invalid conversion of an Object to an integer.");
	}
}

/**************************
 * meat::Object::to_float *
 **************************/

meat::float_t meat::Object::to_float() const {
	switch (data_type) {
	case INTEGER:
		return data.i;
	case FLOAT:
		return data.f;
	default:
		throw Exception("Invalid conversion of an Object to a float.");
	}
}

/***************************
 * meat::Object::to_string *
 ***************************/

const char *meat::Object::to_string() const {
	if (data_type == STRING)
		return data.s;

	throw Exception("Invalid conversion of an Object to a string.");
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
	: Object(meat::ClassClass(), 0) {

	this->hash_id = 0;
	this->super = resolve(parent);
	this->obj_properties = obj_props;
}

meat::Class::Class(const char *parent, uint8_t cls_props, uint8_t obj_props)
	: Object(meat::ClassClass(), cls_props) {

	this->hash_id = 0;
	this->super = resolve(parent);
	this->obj_properties = obj_props;
}

meat::Class::Class(meat::Reference &parent, uint8_t obj_props)
	: Object(meat::ClassClass(true), 0) {

	this->hash_id = 0;
	this->super = parent;
	this->obj_properties = obj_props;
}

meat::Class::Class(meat::Reference &parent,
									 uint8_t cls_props,
									 uint8_t obj_props)
	: Object(meat::ClassClass(), cls_props) {

	this->hash_id = 0;
	this->super = parent;
	this->obj_properties = obj_props;
}

/***********************
 * meat::Class::~Class *
 ***********************/

meat::Class::~Class () throw () {
}

/**************************
 * meat::Class::get_super *
 **************************/

meat::Reference meat::Class::get_super() const {
	return super;
}

/***********************************
 * meat::Class::get_obj_properties *
 ***********************************/

meat::uint8_t meat::Class::get_obj_properties() const {
	meat::uint8_t total = obj_properties;
	if (!super.is_null())
		total += CLASS(super).get_obj_properties();
	return total;
}

/***************************
 * meat::Class::new_object *
 ***************************/

meat::Reference meat::Class::new_object() {
	Reference result;

	if (vtable.constructor)
		return vtable.constructor((Reference &)(*this), get_obj_properties());
	else {
		/*  If we don't have a native constructor then lets see if our super
		 * does.
		 */
		Reference csuper = this->super;
		while (!csuper.is_null()) {
			if (CLASS(csuper).vtable.constructor) {
        return CLASS(csuper).vtable.constructor((Reference &)(*this),
                                                get_obj_properties());
			}
			csuper = CLASS(csuper).super;
		}
	}

	throw Exception("Unable to create new Object");
}

/********************************
 * meat::Class::set_constructor *
 ********************************/

void meat::Class::set_constructor(constructor_t constructor) {
	vtable.set(constructor);
};

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

/*****************************
 * meat::Class::set_bytecode *
 *****************************/

void meat::Class::set_bytecode(uint16_t size, uint8_t *code,
															 alloc_t code_alloc) {
	bytecode.set(size, code, code_alloc);
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

const meat::vtable_entry_t *meat::Class::get_class_vtable(uint8_t &count) const {
	count = vtable.no_centries;
	return vtable.centries;
}

/*****************************
 * meat::Class::get_bytecode *
 *****************************/

const meat::uint8_t *meat::Class::get_bytecode() const {
	return bytecode.get();
}

/**********************
 * meat::Class::write *
 **********************/

void meat::Class::write(std::ostream &lib_file) const {

	if (super.is_null()) {
		throw Exception("Attempting to write an incomplete class with no super.");
	}

	// Write my own hash id.
	meat::uint32_t cooked_id = endian::write_be(hash_id);
	lib_file.write((const char *)&cooked_id, 4);

	// Write our super's hash_id.
	cooked_id = endian::write_be(((Class &)(*super)).hash_id);
	lib_file.write((const char *)&cooked_id, 4);

	/*  We only record the number of class properties there are. When the class
	 * is unserialized all the class properties are initialize to Null.
	 */
	lib_file.put((char)get_num_of_props());

	// Record the number of properties for the objects.
	lib_file.put((char)get_obj_properties());

	// Write the virtual method table.
	vtable.write(lib_file);

	// Write the bytecode
	bytecode.write(lib_file);
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
	cls->hash_id = class_id;

	cls->vtable.read(lib_file);

	cls->bytecode.read(lib_file);

	class_registry_t &classes = class_registry();
	if (classes.find(class_id) != classes.end()) {
		throw Exception("Class import conflict.");
	}

	return cls;
}

/***********************
 * meat::Class::record *
 ***********************/

void meat::Class::record(Reference &cls, bool replace) {
	meat::uint32_t hash_id = CLASS(cls).hash_id;

#ifdef DEBUG
	std::cout << "CLASS: Recording class as " << std::hex
						<< std::showbase << hash_id << std::endl;
#endif

	class_registry_t &classes = class_registry();
	if (not replace && classes.find(hash_id) != classes.end()) {
		throw Exception(((std::string("Class ")) +
										 " already registered.").c_str());
	}

	classes[hash_id] = cls;
	if (!CLASS(cls).super.is_null())
		CLASS(cls).vtable.link(CLASS(CLASS(cls).super));

#ifdef TESTING
	if (!CLASS(cls).vtable.constructor) {
#ifdef DEBUG
		std::cout << "VTABLE: No constructor";
		std::cout << " for " << CLASS(cls);
		std::cout << std::endl;
#endif
	}
#endif /* TESTING */
}

void meat::Class::record(Class *cls, const char *id, bool replace) {
	meat::uint32_t hash_id = hash(id);

#ifdef DEBUG
	std::cout << "CLASS: Recording class " << id << " as id " << std::hex
						<< std::showbase << hash_id << std::endl;
#endif

	class_registry_t &classes = class_registry();
	if (not replace && classes.find(hash_id) != classes.end()) {
		throw Exception(((std::string("Class ") + id) +
										 " already registered.").c_str());
	}

	classes[hash_id] = cls;
	CLASS(cls).hash_id = hash_id;
#ifdef DEBUG
	CLASS(cls).name = id;
#endif /* DEBUG */
	if (!CLASS(cls).super.is_null())
		CLASS(cls).vtable.link(CLASS(CLASS(cls).super));

#ifdef TESTING
	if (!CLASS(cls).vtable.constructor) {
#ifdef DEBUG
		std::cout << "VTABLE: No constructor";
		std::cout << " for " << CLASS(cls);
		std::cout << std::endl;
#endif
	}
#endif /* TESTING */
}

void meat::Class::record(Reference &cls, const char *id, bool replace) {
	meat::uint32_t hash_id = hash(id);

#ifdef DEBUG
	std::cout << "CLASS: Recording class " << id << " as id " << std::hex
						<< std::showbase << hash_id << std::endl;
#endif

	class_registry_t &classes = class_registry();
	if (not replace && classes.find(hash_id) != classes.end()) {
		throw Exception(((std::string("Class ") + id) +
										 " already registered.").c_str());
	}

	classes[hash_id] = cls;
	CLASS(cls).hash_id = hash_id;
#ifdef DEBUG
	CLASS(cls).name = id;
#endif /* DEBUG */
	if (!CLASS(cls).super.is_null())
		CLASS(cls).vtable.link(CLASS(CLASS(cls).super));

#ifdef TESTING
	if (!CLASS(cls).vtable.constructor) {
#ifdef DEBUG
		std::cout << "VTABLE: No constructor";
		std::cout << " for " << CLASS(cls);
		std::cout << std::endl;
#endif
	}
#endif /* TESTING */
}

/*************************
 * meat::Class::unrecord *
 *************************/

void meat::Class::unrecord(Reference &cls) {

#ifdef DEBUG
	std::cout << "CLASS: Unrecording class as " << std::hex
						<< std::showbase << CLASS(cls).hash_id << std::endl;
#endif
	class_registry().erase(CLASS(cls).hash_id);
}

void meat::Class::relink() {
#ifdef DEBUG
	std::cout << "CLASS: Relinking class " << std::hex
						<< std::showbase << hash_id << std::endl;
#endif
	if (!super.is_null())
		vtable.link(CLASS(super));
}

/***************************
 * meat::Class::have_class *
 ***************************/

bool meat::Class::have_class(const char *id) {
	class_registry_t &classes = class_registry();
	return (classes.find(hash(id)) != classes.end());
}

/************************
 * meat::Class::resolve *
 ************************/

meat::Reference &meat::Class::resolve(const char *id) {
	meat::uint32_t hash_id = hash(id);

	class_registry_t &classes = class_registry();
	if (classes.find(hash_id) == classes.end()) {
		throw Exception(((std::string("Class ") + id) + " was not found.").c_str());
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
		throw Exception(msg.str().c_str());
	}

	return classes[hash_id];
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
	return resolve(hash_id);
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
	meat::Test::test("Class virtual table validation", false);
	for (uint8_t c = 1; c < entries; c++) {
		if (table[c - 1].hash_id >= table[c].hash_id) {
			meat::Test::failed("Class virtual table validation", false);
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
	meat::Test::test("Class class virtual table validation", false);
	for (uint8_t c = 1; c < entries; c++) {
		if (table[c - 1].hash_id >= table[c].hash_id) {
			meat::Test::failed("Class class virtual table validation", false);
			break;
		}
	}
#endif

	no_centries = entries;
  switch (table_alloc) {
	case STATIC: // The table is a part of the code and not deallocation is needed.
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
	meat::Test::test("Setting vtable constructor procedure", false);
	if (constructor == 0)
		meat::Test::failed("Setting vtable constructor procedure", false);
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
		if (entries[c].flags & VTM_NATIVE && !(entries[c].flags & VTM_SUPER)) {
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
		if (centries[c].flags & VTM_NATIVE && !(centries[c].flags & VTM_SUPER)) {
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
 * meat::Class::ByteCode Class
 */

/***********************************
 * meat::Class::ByteCode::ByteCode *
 ***********************************/

meat::Class::ByteCode::ByteCode() : size(0), code(NULL), is_static(true) {
}

meat::Class::ByteCode::~ByteCode() throw() {
	// Deallocate the bytecode.
	if (!is_static && code != NULL) delete[] code;
}

/********************************
 * meat::Class::ByteCode::write *
 ********************************/

void meat::Class::ByteCode::write(std::ostream &lib_file) const {
	// Write the size of the bytecode.
	meat::uint16_t size_be = endian::write_be((meat::uint16_t)size);
	lib_file.write((const char *)&size_be, 2);

	// Write the bytecode to the file.
	lib_file.write((const char *)code, size);
}

/*******************************
 * meat::Class::ByteCode::read *
 *******************************/

void meat::Class::ByteCode::read(std::istream &lib_file) {
	// Read in the size of the bytecode.
	lib_file.read((char *)&size, 2);
	size = endian::read_be(size);

	// Allocate space for the bytecode and read it in.
	code = new uint8_t[size];
	lib_file.read((char *)code, size);

	is_static = false;
}

/******************************
 * meat::Class::ByteCode::set *
 ******************************/

void meat::Class::ByteCode::set(unsigned int size, uint8_t *code,
																alloc_t code_alloc) {
	if (size) {
		this->size = size;
    switch (code_alloc) {
		case STATIC:
			this->code = code;
			is_static = true;
			break;
		case DYNAMIC:
			this->code = code;
			is_static = false;
			break;
		case COPY:
			this->code = new uint8_t[this->size];
			std::memcpy(this->code, code, this->size);
			is_static = false;
			break;
    }
	}
}

/******************************
 * meat::Class::ByteCode::get *
 ******************************/

const meat::uint8_t *meat::Class::ByteCode::get() const {
	return code;
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
	: Object(Class::resolve("Context")), result_index(0), done(false) {
	num_of_locals = locals + 4;
	this->locals = new Reference[num_of_locals];
}

meat::Context::Context(Reference &context, meat::uint8_t locals)
	: Object(Class::resolve("Context")), up_level(context), result_index(0),
		done(false) {
	num_of_locals = locals + 4;
	this->locals = new Reference[num_of_locals];
}

meat::Context::Context(Reference &cls,
											 Reference &context,
											 meat::uint8_t locals)
	: Object(cls), up_level(context), result_index(0), done(false) {
	num_of_locals = locals + 4;
	this->locals = new Reference[num_of_locals];
}

/***************************
 * meat::Context::~Context *
 ***************************/

meat::Context::~Context() throw() {
	delete [] locals;
}

/***************************
 * meat::Context::get_self *
 ***************************/

meat::Reference meat::Context::get_self() const {
	return locals[0];
}

/*****************************
 * meat::Context::reset_self *
 *****************************/

void meat::Context::reset_self(Reference &new_self) {
	locals[0] = new_self;
}

/******************************
 * meat::Context::get_uplevel *
 ******************************/

meat::Reference meat::Context::get_uplevel() const {
	return up_level;
}

/******************************
 * meat::Context::set_uplevel *
 ******************************/

void meat::Context::set_uplevel(Reference &context) {
	up_level = context;
}

/****************************
 * meat::Context::get_param *
 ****************************/

meat::Reference meat::Context::get_param(uint8_t index) const {
	return locals[index + 4];
}

/****************************
 * meat::Context::set_param *
 ****************************/

void meat::Context::set_param(uint8_t index, Reference &value) {
	locals[index + 4] = value;
}

/****************************
 * meat::Context::set_local *
 ****************************/

void meat::Context::set_local(uint8_t index, Reference &value) {
	if (index < num_of_locals) locals[index] = value;
	else {
		std::stringstream msg;
		msg << "Setting local variable " << (unsigned int)index
				<< " out of range (0-" << (unsigned int)(num_of_locals - 1) << ")";
		throw Exception(msg.str());
	}
}

/****************************
 * meat::Context::get_local *
 ****************************/

meat::Reference meat::Context::get_local(uint8_t index) const {
	if (index < num_of_locals) return locals[index];
	else {
		std::stringstream msg;
		msg << "Getting local variable " << (unsigned int)index
				<< " out of range (0-" << (unsigned int)(num_of_locals - 1) << ")";
		throw Exception(msg.str());
	}
}

/***********************************
 * meat::Context::set_result_index *
 ***********************************/

void meat::Context::set_result_index(uint8_t local_parent_index) {
	result_index = local_parent_index;
	CONTEXT(up_level).set_local(result_index, meat::Null());
}

/*****************************
 * meat::Context::set_result *
 *****************************/

void meat::Context::set_result(Reference &value) {
	result = value;
	if (result_index) {
		CONTEXT(up_level).set_local(result_index, result);
	}
}

/*****************************
 * meat::Context::get_result *
 *****************************/

meat::Reference meat::Context::get_result() const {
	return result;
}

/******************************************************************************
 * meat::BlockContext Class
 */

/************************************
 * meat::BlockContext::BlockContext *
 ************************************/

meat::BlockContext::BlockContext(uint8_t locals)
	: Context(Class::resolve("BlockContext"), meat::Null(), locals),
		context(meat::Null()) {
	flags = meat::Context::NOMETHOD;
}

meat::BlockContext::BlockContext(Reference &context, meat::uint8_t locals,
																 uint16_t ip)
	: Context(Class::resolve("BlockContext"), context, locals),
		context(context) {
	flags = meat::Context::BYTECODE;
	this->ip = ip;
}

/*************************************
 * meat::BlockContext::~BlockContext *
 *************************************/

meat::BlockContext::~BlockContext() throw() {
}

/*********************************
 * meat::BlockContext::set_local *
 *********************************/

void meat::BlockContext::set_local(meat::uint8_t index,
																	 meat::Reference &value) {
	meat::uint8_t local_cnt = CONTEXT(context).get_num_of_locals();

	if (index < local_cnt)
		CONTEXT(context).set_local(index, value);
	else
		Context::set_local(index - local_cnt, value);
}

/*********************************
 * meat::BlockContext::get_local *
 *********************************/

meat::Reference meat::BlockContext::get_local(meat::uint8_t index) const {
	meat::uint8_t local_cnt = CONTEXT(context).get_num_of_locals();

	if (index < local_cnt)
		return CONTEXT(context).get_local(index);
	else
		return Context::get_local(index - local_cnt);
}

/*****************************************
 * meat::BlockContext::get_num_of_locals *
 *****************************************/

meat::uint8_t meat::BlockContext::get_num_of_locals() const {
	return CONTEXT(context).get_num_of_locals() + Context::get_num_of_locals();
}

/*******************************
 * meat::BlockContext::is_done *
 *******************************/

bool meat::BlockContext::is_done() {
	return (Context::is_done() or CONTEXT(context).is_done());
};

/******************************************************************************
 * meat::Exception Class
 */

meat::Exception::Exception()
	: Object(Class::resolve("Exception"), 2) {
	// Empty Exception
}

meat::Exception::Exception(const Exception &other)
	: Object(Class::resolve("Exception"), 2) {
	this->property(0) = other.property(0);
	this->property(1) = other.property(1);
}

meat::Exception::Exception(Reference &cls, meat::uint8_t properties)
	: Object(cls, properties) {
	// Empty Exception constructor used for class inheritance.
}

meat::Exception::Exception(const char *message)
	: Object(Class::resolve("Exception"), 2) {
	// New exception with a meat Text message.
	this->property(0) = new Object(message);
}

meat::Exception::Exception(std::string message)
	: Object(Class::resolve("Exception"), 2) {
	// New exception with a meat Text message.
	this->property(0) = new Object(message.c_str());
}

meat::Exception::Exception(const char *message, Reference &context)
	: Object(Class::resolve("Exception"), 2) {
	this->property(0) = new Object(message);
	this->property(1) = context;
}

meat::Exception::Exception(std::string message, Reference &context)
	: Object(Class::resolve("Exception"), 2) {
	this->property(0) = new Object(message.c_str());
	this->property(1) = context;
}

meat::Exception::Exception(Reference &message, Reference &context)
	: Object(Class::resolve("Exception"), 2) {
	this->property(0) = message;
	this->property(1) = context;
}

const char* meat::Exception::what() const throw() {
	try {
		// Attempt to get the error message from property 0.
		if (this->property(0) == Null())
			return "";
		else
			return this->property(0)->to_string();
	} catch (...) {
		// If property 0 cannot be converted to a string then return nothing.
		return "";
	}
}

/******************************************************************************
 * List Class Implemenation
 */

meat::List::List() : Object(Class::resolve("List")) {
}

meat::List::List(Reference &cls, meat::uint8_t properties)
	: Object(cls, properties) {
}

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
 * Index Class Implemenation
 */

bool meat::obj_less::operator()(const Reference &first,
																const Reference &second) const {
	Reference ctx = message((Reference &)first, "<", meat::Null());
	CONTEXT(ctx).set_param(0, (Reference &)second);
	Reference result = execute(ctx);
	if (result == meat::True())
		return true;
	return false;
}

meat::Index::Index() : Object(Class::resolve("Index")) {
}

meat::Index::Index(Reference &cls, meat::uint8_t properties)
	: Object(cls, properties) {
}

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

/**********************
 * Index::unserialize *
 **********************/

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

/******************
 * meat::message *
 ******************/

meat::Reference meat::message(meat::Reference &object,
															meat::uint32_t hash_id,
															meat::Reference &context) {

	Reference cls;
	const vtable_entry_t *m_entry = 0;

#ifdef TESTING
	meat::Test::test("Messaging Object", false);
	if (object.is_null()) {
		meat::Test::failed("Messaging object", false);
		throw Exception("Unable to message a null Reference");
	}
#endif

	// Resolve the message with the hash_id.
	if (object->is_class()) {
    cls = object;

		/*  If the method is inherited then start looking through the parent
		 * classes to find the actual method entry.
		 */
		while (m_entry == 0 and !cls.is_null()) {
			m_entry = ((Class &)(*cls)).class_find(hash_id);
			if (m_entry == 0) cls = CLASS(cls).get_super();
		}

	} else {
		cls = object->get_type();

		/*  If the method is inherited then start looking through the parent
		 * classes to find the actual method entry.
		 */
		while (m_entry == 0 and !cls.is_null()) {
			m_entry = CLASS(cls).find(hash_id);
			if (m_entry == 0) cls = CLASS(cls).get_super();
		}
	}

	// Raise an error if we couldn't resolve the method name.
	if (m_entry == 0) throw Exception("Message is unresolved", context);

  // Now create the new context.
  Context *ctx = new Context(context, m_entry->locals);
  Reference new_context(ctx);

  // Populate the mandatory local objects.
  ctx->locals[0] = object;             // self
  ctx->locals[1] = meat::Class::resolve(m_entry->class_id); // class
  ctx->locals[2] = new_context.weak(); // context
  ctx->locals[3] = meat::Null();      // null

  if (m_entry->flags & VTM_NATIVE) {
    // Flag for native method.
    ctx->flags = meat::Context::PRIMATIVE;
    ctx->pointer = m_entry->method.pointer;
  } else {
    // Flag for bytecode method.
    ctx->flags = meat::Context::BYTECODE;
    ctx->ip = m_entry->method.offset;
  }

  // Return the new context.
  return new_context;
}

/******************
 * meat::message *
 ******************/

meat::Reference meat::message(meat::Reference &object,
															const char *method,
															meat::Reference &context) {
	return message(object, hash(method), context);
}

/************************
 * meat::message_super *
 ************************/

meat::Reference meat::message_super(meat::Reference &object,
																		meat::uint32_t hash_id,
																		meat::Reference &context) {

	Reference cls;
	const vtable_entry_t *m_entry = 0;

	// Resolve the message with the hash_id.
	if (object->is_class()) {
		cls = CONTEXT(context).get_super();

		/*  If the method is inherited then start looking through the parent
		 * classes to find the actual method entry.
		 */
		while (m_entry == 0 and !cls.is_null()) {
			m_entry = CLASS(cls).class_find(hash_id);
			if (m_entry == 0) cls = CLASS(cls).get_super();
		}

	} else {
		cls = CONTEXT(context).get_super();

		/*  If the method is inherited then start looking through the parent
		 * classes to find the actual method entry.
		 */
		while (m_entry == 0 and !cls.is_null()) {
			m_entry = CLASS(cls).find(hash_id);
			if (m_entry == 0) cls = CLASS(cls).get_super();
		}
	}

	// Raise an error if we couldn't resolve the method name.
	if (m_entry == 0) throw Exception("Message is unresolved", context);

	// Now create the new context.
	Context *ctx = new Context(context, m_entry->locals);
	Reference new_context(ctx);

	// Populate the mandatory local objects.
	ctx->locals[0] = object;                                  // self
	ctx->locals[1] = meat::Class::resolve(m_entry->class_id); // class
	ctx->locals[2] = new_context.weak();                      // context
	ctx->locals[3] = meat::Null();                            // null

	if (m_entry->flags & VTM_NATIVE) {
		// Flag for native method.
		ctx->flags = meat::Context::PRIMATIVE;
		ctx->pointer = m_entry->method.pointer;
	} else {
		// Flag for bytecode method.
		ctx->flags = meat::Context::BYTECODE;
		ctx->ip = m_entry->method.offset;
	}

	// Return the new context.
	return new_context;
}

#ifdef DEBUG
std::ostream &meat::operator <<(std::ostream &out, Class &cls) {
	return (out << "CLASS<" << cls.name << ">");
}
#endif

/*********************
 * meat::ClassClass *
 *********************/

meat::Reference &meat::ClassClass(bool initializing) {
	static Reference cls;

	if (cls.is_null() and not initializing) {
		cls = Class::resolve("Class");
	}

	return cls;
}

/***************
 * meat::True *
 ***************/

meat::Reference &meat::True() {
	static Reference true_object;

	if (true_object.is_null()) {
		true_object = Class::resolve("Boolean")->property(0);
	}

	return true_object;
}

/****************
 * meat::False *
 ****************/

meat::Reference &meat::False() {
	static Reference false_object;

	if (false_object.is_null()) {
		false_object = Class::resolve("Boolean")->property(1);
	}

	return false_object;
}

/***************
 * meat::Null *
 ***************/

meat::Reference &meat::Null() {
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
