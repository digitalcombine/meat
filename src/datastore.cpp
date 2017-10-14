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

#include <meat/datastore.h>
#include <meat/utilities.h>

#include <algorithm>
#include <cstring>
#include <fstream>
#include <unistd.h>

#ifdef TESTING
#include <testing.h>
#include <iostream>
#endif

#ifdef DEBUG
#include <iostream>
#endif

#if defined (__WIN__)
#include <windows.h>
#define DLLEXT ".dll"
#else
#include <dlfcn.h>
#define DLLEXT ".so"
#endif

#define null (meat::Null())

extern "C" {
  typedef struct _sgelib_header_t {
    char magic[4];            // File type magic
    meat::uint8_t major_ver;
    meat::uint8_t minor_ver;
  } sgelib_header_t;
}

/******************************************************************************
 * Dynamic Library importing abstract interface
 */

static bool fexists(const char *filename) {
  return (access(filename, F_OK) == 0);
}

static nativelib_t dl_open(const char *filename) {
#if defined (__linux__)
  void *handle = dlopen(filename, RTLD_LAZY);
  if (handle == NULL)
    throw meat::Exception(dlerror());
  return handle;
#endif
}

static void dl_close(nativelib_t handle) {
#if defined (__linux__)
  if (dlclose(handle) != 0)
    std::cerr << "WARNING: dlclose " << dlerror() << std::endl;
#endif
}

static void *dl_symbol(nativelib_t handle, const char *funcname) {
#if defined (__linux__)
  return dlsym(handle, funcname);
#endif
}


/***********************
 * meat::data::fl_type *
 ***********************/

int meat::data::fl_type(const std::string &fname) {
	std::ifstream fd(fname.c_str());
	char magic[4] = {'X', 'X', 'X', 'X'};

	fd.read(magic, 4);
	if (fd) {
		fd.close();
		if (std::strncmp(magic, "MARC", 4) == 0) return FL_ARCHIVE;
		if (std::strncmp(magic, "MLIB", 4) == 0) return FL_LIBRARY;
	} else
		fd.close();
	return FL_UNKNOWN;
}

/******************************************************************************
 * class meat:data::Library
 */

/** Contains the library search path.
 */
static std::deque<std::string> &get_path() {
  static std::deque<std::string> path;
  return path;
}

/**
 */
static std::map<std::string, meat::data::Library *> &get_libraries() {
  static std::map<std::string, meat::data::Library *> imported;
  return imported;
}

/********************************
 * meat::data::Library::Library *
 ********************************/

meat::data::Library::Library(const std::string &name)
	: imports(new meat::List()), syms_free(false), syms_size(0), symbols(NULL),
		dlhandle(0) {
  this->_name = name;
  this->is_new = false;
}

/*********************************
 * meat::data::Library::~Library *
 *********************************/

meat::data::Library::~Library() throw() {

#ifdef DEBUG
  std::cout << "LIBRARY: Final unloading of " << name << std::endl;
#endif

  /*  Cleanup all the classes in the libray. Note if any object is still using
   * any of these classes the references will keep the class until no one is
   * using them any more.
   */
	if (_name != "__builtin__") {
		std::deque<Reference>::iterator it;
		for (it = classes.begin(); it != classes.end(); it++)
			Class::unrecord(*it);
	}

	clear_symbols();

  if (is_native) dl_close(dlhandle);
}

/*******************************
 * meat::data::Library::create *
 *******************************/

meat::data::Library *meat::data::Library::create(const std::string &name) {
  Library *new_lib = new Library(name);
  new_lib->_name = name;

	// The __builtin__ library is special ;)
	if (new_lib->_name == "__builtin__")
		new_lib->is_new = false;
	else
		new_lib->is_new = true;

  get_libraries()[name] = new_lib;

  return new_lib;
}

/*******************************
 * meat::data::Library::import *
 *******************************/

meat::data::Library *meat::data::Library::import(const std::string &name) {
  // Check to see if we have already imported the library.
  std::map<std::string, Library *>::iterator lib = get_libraries().find(name);
  if (lib != get_libraries().end())
    return lib->second;

	// Load the library.
  Library *imported_lib = new Library(name);
  imported_lib->import();

	// Register the library.
  get_libraries()[name] = imported_lib;

  return imported_lib;
}

/********************************
 * meat::data::Library::execute *
 ********************************/

meat::Reference meat::data::Library::execute(const std::string &name) {
	Library *imported_lib = new Library(name);
  imported_lib->import_from_archive(name);

	if (not imported_lib->application.is_null()) {
		get_libraries()[name] = imported_lib;

		// Message the application and get things rolling.
		meat::Reference context = meat::message(imported_lib->application,
																						"entry", meat::Null());
		return meat::execute(context);
	}
	throw meat::Exception(std::string("Library ") + name +
												" is not executable.");
}

/****************************
 * meat::data::Library::get *
 ****************************/

meat::data::Library *
meat::data::Library::get(const std::string &name) {
	std::map<std::string, meat::data::Library *>::iterator it =
    get_libraries().find(name);
  if (it != get_libraries().end()) {
		return it->second;
  }
	return NULL;
}

/*******************************
 * meat::data::Library::unload *
 *******************************/

void meat::data::Library::unload(const std::string &name) {

#ifdef DEBUG
  std::cout << "LIBRARY: Unloading " << name << std::endl;
#endif

  std::map<std::string, meat::data::Library *>::iterator it =
    get_libraries().find(name);
  if (it != get_libraries().end()) {
    delete it->second;
    get_libraries().erase(it);
  }
}

/*******************************
 * meat::data::Library::import *
 *******************************/

void meat::data::Library::import() {

#ifdef DEBUG
  std::cout << "LIBRARY: Importing " << name << std::endl;
#endif

	// First see if the library is already loaded.
  std::map<std::string, Library *>::iterator lib = get_libraries().find(_name);
  if (lib != get_libraries().end()) {
    return;
  }

  /*  Search the path for the library. This code needs a serious review and
   * may be prone to many unexpected errors.
   */
  std::ifstream lib_file;
  std::deque<std::string>::iterator it = get_path().begin();

	bool found = false;
  for (; it != get_path().end(); it++) {
    if (fexists(((*it) + (this->_name + ".mlib")).c_str())) {
      import_from_archive((*it) + (this->_name + ".mlib"));
			found = true;
			break;
    } else if (fexists(((*it) + (this->_name + DLLEXT)).c_str())) {
      // The .mlib failed, now try try to open a native library file.
      import_from_native((*it) + (this->_name + DLLEXT), this->_name);
			found = true;
			break;
    }
  }
	if (!found) {
		/* OMG, we couldn't find the library. Must be a vegetarian :P */
		throw meat::Exception(std::string("Unable to find library ") +
													this->_name);
	}

	// Initialize all the imported classes.
	init_classes();
}

/*************************************
 * meat::data::Library::init_classes *
 *************************************/

void meat::data::Library::init_classes() {
	std::deque<Reference>::iterator class_it = classes.begin();
	for (; class_it != classes.end(); ++class_it) {
		Reference context = message(*class_it, "initialize", meat::Null());
		meat::execute(context);
	}
}

/*********************************
 * meat::data::Library::add_path *
 *********************************/

void meat::data::Library::add_path(const std::string &name) {
  get_path().push_front(name);
}

/****************************
 * meat::data::Library::add *
 ****************************/

void meat::data::Library::add(Class *cls, const std::string &id) {
  if (!cls->is_class()) {
    throw meat::Exception("Only classes can be added to library files.");
  }

#ifdef DEBUG
  std::cout << "LIBRARY: Adding class " << id << std::endl;
#endif /* DEBUG */
  Reference newcls = cls;
  classes.push_back(newcls);
	if (_name != "__builtin__")
		Class::record(newcls, id.c_str());
	cls->library = this;
}

/******************************
 * meat::data::Library::clear *
 ******************************/

void meat::data::Library::clear() {
	/*  Cleanup all the classes in the libray. Note if any object is still using
   * any of these classes the references will keep the class until no one is
   * using them any more.
   */
	if (_name != "__builtin__") {
		std::deque<Reference>::iterator it;
		for (it = classes.begin(); it != classes.end(); it++)
			Class::unrecord(*it);
	}
	classes.clear();

	clear_symbols();
}

/****************************************
 * meat::data::Library::set_application *
 ****************************************/

/** @todo This is a rather dumb method. It should check to make sure
 *        the class exists in this library, not just in any of the
 *        loaded libraries.
 */
void meat::data::Library::set_application(const std::string &name) {
	application = Class::resolve(_name);
}

/******************************
 * meat::data::Library::dlsym *
 ******************************/

void *meat::data::Library::dlsymbol(const std::string &name) {
	if (dlhandle) {
		void *result = dl_symbol(dlhandle, name.c_str());
		if (!result) {
			throw Exception(std::string("Unable to load DL symbol ") + name +
											" from library " + this->_name);
		}
		return result;
	}
	throw Exception(std::string("Unable to load DL symbol from non-native "
															"library ") + this->_name);
}

/************************************
 * meat::data::Library::set_symbols *
 ************************************/

void meat::data::Library::set_symbols(meat::uint8_t *symbols,
																			meat::alloc_t sym_alloc) {
	if (this->symbols and syms_free) delete[] symbols;

	// Build symbol index table.
	this->symbols = symbols;
	const char *sym = (const char *)this->symbols;
	while (*sym != 0) {
		syms_table[hash(sym)] = sym;
		sym += std::strlen(sym) + 1;
	}

	// Record the size of the symbols table.
	syms_size = (unsigned int)(sym - (const char *)this->symbols) + 1;

	// Deal with memory management.
	switch (sym_alloc) {
	case meat::STATIC:
		syms_free = false;
		break;
	case meat::DYNAMIC:
		syms_free = true;
		break;
	case meat::COPY:
		syms_free = true;
		this->symbols = new meat::uint8_t[syms_size];
		std::memcpy(this->symbols, symbols, syms_size);
		break;
	}
}

/**************************************
 * meat::data::Library::clear_symbols *
 **************************************/

void meat::data::Library::clear_symbols() {
	if (syms_free) delete[] symbols;
	symbols = NULL;
	syms_size = 0;
	syms_free = false;
	syms_table.clear();
}

/*******************************
 * meat::data::Library::lookup *
 *******************************/

std::string meat::data::Library::lookup(meat::uint32_t hash_id) const {
	std::map<meat::uint32_t, const char *>::const_iterator it =
		syms_table.find(hash_id);
	if (it != syms_table.end()) {
		return it->second;
	}
	return itohex(hash_id);
}

/********************************************
 * meat::data::Library::import_from_archive *
 ********************************************/

void meat::data::Library::import_from_archive(const std::string &name) {
  sgelib_header_t header;

  std::ifstream lib_file(name.c_str(), std::ios::in | std::ios::binary);

  if (not lib_file.is_open())
    throw meat::Exception(std::string("Unable to open file ") + name);

  // Read in the file header and make sure the it's the right type of file.
  lib_file.read((char *)&header, sizeof(sgelib_header_t));
	if (strncmp(header.magic, "MLIB", 4) != 0) {
		throw meat::Exception(std::string("Attempting to import a non-library"
																			" file ") + name);
	}

	// Library flags.
	lib_file.get();

	// Application class index.
	uint32_t app_id;
	lib_file.read((char *)&app_id, 4);
	app_id = endian::read_be(app_id);

  // Read and import all the necessary libraries.
  int import_cnt = lib_file.get();
#ifdef DEBUG
  std::cout << "LIBRARY: Importing " << std::dec << import_cnt
            << " required libraries." << std::endl;
#endif
  for (int ic = 0; ic < import_cnt; ic++) {
    std::string import_name;

    // Read in the null terminated library name.
    char ch;
    while ((ch = lib_file.get()) != 0)
      import_name += ch;

#ifdef DEBUG
    std::cout << "LIBRARY: Importing requirement "
              << import_name << ".mlib" << std::endl;
#endif

    // Import the library.
    meat::data::Library::import(import_name.c_str());
  }

  /* Import all the classes from the file. */
  unsigned int class_cnt = lib_file.get();
#ifdef DEBUG
  std::cout << "LIBRARY: Importing " << std::dec
            << (int)class_cnt << " classes" << std::endl;
#endif
  for (unsigned int cc = 0; cc < class_cnt; cc++) {
    Reference cls = meat::Class::import(lib_file);
    classes.push_back(cls);
    Class::record(cls);
		cast<Class>(cls).library = this;
		if (app_id && cast<Class>(cls).hash_id() == app_id)
			application = cls;
  }

	// Read in the symbols table.
	uint32_t sz;
	lib_file.read((char *)&sz, 4);
	sz = endian::read_be(sz);
	if (sz) {
		meat::uint8_t *syms = new meat::uint8_t[sz];
		lib_file.read((char *)syms, sz);
		set_symbols(syms, meat::DYNAMIC);
	}

  lib_file.close();

	/*  Now that we have loaded all the class we can resolve the application
	 * class if it was set.
	 */
	if (app_id)
		application = Class::resolve(app_id);
}

/*******************************************
 * meat::data::Library::import_from_native *
 *******************************************/

void meat::data::Library::import_from_native(const std::string &filename,
																						 const std::string &name) {
  typedef void *(*init_proc_t)(Library &library);

#ifdef DEBUG
  std::cout << "LIBRARY: Importing from native file " << filename
            << std::endl;
#endif

  is_native = true;
  dlhandle = dl_open(filename.c_str());

  init_proc_t init_proc =
    (init_proc_t)dl_symbol(dlhandle, (std::string("init_") + name).c_str());
  if (init_proc != NULL) {
    init_proc(*this);
  } else
    throw meat::Exception(std::string("Unable to initialize library ") +
													dlerror());
}

/******************************************************************************
 * Library Class
 */

static meat::vtable_entry_t LibraryMethods[] = {
  {0x0000043c, 0x00000000, VTM_SUPER, 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER, 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER, 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER, 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER, 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER, 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER, 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER, 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER, 0, {(meat::method_ptr_t)0}}
};

// class method import:
static meat::Reference Library_cm_import_(meat::Reference context) {
  //meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference filename = meat::cast<meat::Context>(context).parameter(0);

#ifdef DEBUG
  std::cout << "DEBUG: Importing library " << meat::cast<meat::Text>(filename)
            << std::endl;
#endif /* DEBUG */

  if (meat::grinder_impl() != NULL) {
		meat::grinder_impl()->import(meat::cast<meat::Text>(filename), context);
    //compiler_import()(meat::cast<meat::Text>(filename), context);
  } else {
    meat::data::Library::import(meat::cast<meat::Text>(filename));
  }
  return null;
}

// class method include:
static meat::Reference Library_cm_include_(meat::Reference context) {
  //meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference cpp_includes =
		meat::cast<meat::Context>(context).parameter(0);

  if (meat::grinder_impl() != NULL) {
		meat::grinder_impl()->include(meat::cast<meat::Text>(cpp_includes));
  } else {
    throw meat::Exception("Method Library include is only with the compiler");
	}

  return null;
}

// class method setApplication:
static meat::Reference Library_cm_setApplication_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  //meat::Reference klass = meat::cast<meat::Context>(context).get_class();
  meat::Reference class_name = meat::cast<meat::Context>(context).parameter(0);
	return null;
}

static meat::vtable_entry_t LibraryCMethods[] = {
  {0x0000043c, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x05614602, 0x6d20bcbb, VTM_NATIVE, 1, Library_cm_include_},
  {0x068b6f7b, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
	{0x1c461870, 0x6d20bcbb, VTM_NATIVE, 1, Library_cm_setApplication_},
  {0x2c296348, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x54aa30e6, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x72cd0161, 0x6d20bcbb, VTM_NATIVE, 1, Library_cm_import_},
  {0x7a8e569a, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}}
};

/******************************************************************************
 * class meat::data::Archive
 */

#define OBJECT_PROP      0x00
#define OBJECT_PROP_WEAK 0x01

typedef struct _sgedat_header_t {
  char magic[4];     // Should be MARC
  meat::uint8_t major_ver;
  meat::uint8_t minor_ver;
  meat::uint32_t import_offset;
  meat::uint32_t index_offset;
} sgedat_header_t;

/********************************
 * meat::data::Archive::Archive *
 ********************************/

meat::data::Archive::Archive(const char *filename, bool create)
  : Object(Class::resolve("Archive")), name(filename), create(create)  {
  synced = false;
  if (!this->create) {

#ifdef DEBUG
    std::cout << "ARCHIVE: Openning file " << name << std::endl;
#endif

    std::ifstream dat_file((this->name).c_str(),
                           std::ios::in | std::ios::binary);

    if (dat_file.is_open()) {
      sgedat_header_t header;

      // Read the header and file magic ID.
      dat_file.read((char *)&header, sizeof(sgedat_header_t));
      if (strncmp(header.magic, "MARC", 4) != 0) {
        throw meat::Exception("Archive attempting to open a non-archive"
                               " file.");
      }

#ifdef DEBUG
      std::cout << std::dec;
      std::cout << "       Version " << (unsigned int)header.major_ver << "."
                << (unsigned int)header.minor_ver << std::endl;
      std::cout << "       Import offset at " << std::dec
                << (unsigned int)endian::read_be(header.import_offset)
                << std::endl;
#endif

      /* Read and import all the necessary libraries. */
      dat_file.seekg(endian::read_be(header.import_offset));
      unsigned int import_cnt = dat_file.get();

#ifdef DEBUG
      std::cout << "       Importing " << (unsigned int)import_cnt
                << " libraries" << std::endl;
#endif

      /* Read in all the library dependancy names and import those libraries.
       */
      for (unsigned int ic = 0; ic < import_cnt; ic++) {
        std::string import_name;

        char ch;
        while ((ch = dat_file.get()) != 0) {
          import_name += ch;
        }

        Library::import(import_name.c_str());
      }

      /* Read in the object index size. */
      dat_file.seekg(endian::read_be(header.index_offset));
      meat::uint32_t index_count;
      dat_file.read((char *)&index_count, 4);
      index_count = endian::read_be(index_count);
#ifdef DEBUG
      std::cout << "       Reading index "
                << "at " << endian::read_be(header.index_offset)
                << " size = "
                << index_count << std::endl;
#endif
      index.resize(index_count);

      /* Read in the object index entries. */
      for (meat::uint32_t c = 0; c < index_count; c++) {
        meat::uint32_t hash_id;
        dat_file.read((char *)&hash_id, 4);
        index[c].cls_id = endian::read_be(hash_id);

        meat::uint32_t offset;
        dat_file.read((char *)&offset, 4);
        index[c].obj_offset = endian::read_be(offset);
      }
    } else {
      throw meat::Exception((std::string("Unable to open data file ") +
                              filename) + " for reading.");
    }
  }
}

/*********************************
 * meat::data::Archive::~Archive *
 *********************************/

meat::data::Archive::~Archive() throw() {
}

/***********************************
 * meat::data::Archive::add_import *
 ***********************************/

void meat::data::Archive::add_import(const char *name) {
  // We can only add library imports when creating a new file.
  if (create)
    imports.push_back(name);
}

/***********************************
 * meat::data::Archive::set_object *
 ***********************************/

void meat::data::Archive::set_object(Reference &object) {
  if (create) {
#ifdef DEBUG
    std::cout << "ARCHIVE: Setting new object " << std::endl;
#endif

    Index new_index;
    new_index.object = object;
    new_index.cls_id = cast<Class>(object->type()).hash_id();

    index.clear();
    index.push_back(new_index);

    synced = false;
  } else {
    throw meat::Exception("Cannot write to a readonly object store.");
  }
}

/*************************************
 * meat::data::Archive::add_property *
 *************************************/

meat::uint32_t meat::data::Archive::add_property(Reference property) {
  if (create) {
    /* Search the index to see if the object already exists. If it does then
     * just return the index offset.
     */
    for (size_t c = 0; c < index.size(); c++) {
      if (index[c].object == property) return c;
    }

    if (property.is_null()) {
      /* Add the object to the index and return the index offset. */
#ifdef TESTING
      std::clog << "TEST WARNING: Serializing an object with a null reference"
        " property. This should no longer be happening, fix the problem."
                << std::endl;
#endif
      Index new_index;
      new_index.object = meat::Null();
      index.push_back(new_index);
      return index.size() - 1;
    } else {
      /* Add the object to the index and return the index offset. */
      Index new_index;
      new_index.object = property.weak();
      new_index.cls_id = cast<Class>(property->type()).hash_id();
      index.push_back(new_index);
      return index.size() - 1;
    }
  } else {
    throw meat::Exception("Cannot write to a readonly archive.");
  }
}

/***********************************
 * meat::data::Archive::get_object *
 ***********************************/

meat::Reference meat::data::Archive::get_object() {
  if (!create) {

#ifdef DEBUG
    std::cout << "ARCHIVE: Reading "
              << this->name << std::endl;
#endif /* DEBUG */

    data_stream.open((this->name).c_str(),
                     std::ios::in | std::ios::binary);
    if (data_stream.is_open()) {
      return get_object(0);
      data_stream.close();
    } else {
      throw meat::Exception((std::string("Unable to open archive ") +
                              this->name).c_str());
    }
  } else {
    throw meat::Exception((std::string("Cannot read from a newly "
                                        "created archive") +
                            this->name).c_str());
  }
}

/***********************************
 * meat::data::Archive::get_object *
 ***********************************/

typedef struct _props_index_s {
	meat::uint32_t offset;
	meat::uint8_t flags;
} props_index_t;

meat::Reference meat::data::Archive::get_object(uint32_t index) {

  /* Make sure the stream is open before we get started.
   */
  if (not data_stream.is_open())
    throw meat::Exception((std::string("Internal read error on archive ") +
                            this->name).c_str());

#ifdef DEBUG
  std::cout << "ARCHIVE: get_object(" << std::dec
            << (unsigned int)index << ")" << std::endl;
#endif /* DEBUG */

  /* See if we have the object already loaded in out cache.
   */
  if (this->index.at(index).cls_id == 0x002539a7 ||
      this->index.at(index).obj_offset == 0) {
#ifdef DEBUG
    std::cout << "         -> Null" << std::endl;
#endif /* DEBUG */
    return meat::Null();
  } else if (this->index.at(index).cls_id == 0x67140424) {
    /*  Unserialize a boolean object. This seriously sucks, but it works for
     * now...
     */
#ifdef DEBUG
    std::cout << "         -> Boolean" << std::endl;
#endif /* DEBUG */
    std::streampos save_pos = data_stream.tellp();
    data_stream.seekg(this->index.at(index).obj_offset);

    meat::uint8_t value;
    //data_stream >> value; // Object data value type
    data_stream >> value;

    data_stream.seekg(save_pos);
    switch (value) {
    case 0:
      return meat::BFalse();
    default:
      return meat::BTrue();
    }
  } else if (not this->index.at(index).object.is_null()) {
    return this->index.at(index).object;
  }

  /* The objects are written to the file as follows:
   *  internal binary data
   *  properties offsets
   */
  std::streampos save_pos = data_stream.tellp();
  data_stream.seekg(this->index.at(index).obj_offset);

#ifdef DEBUG
  std::cout << "         -> Class id " << std::hex
            << this->index.at(index).cls_id << std::endl;
#endif /* DEBUG */

  Reference obj_class = Class::resolve(this->index.at(index).cls_id);

#ifdef DEBUG
  std::cout << "         -> " << cast<Class>(obj_class) << std::endl;
#endif /* DEBUG */

  Reference obj = cast<Class>(obj_class).new_object();

  // Read in all the index offsets for the object's properties.
  meat::uint8_t num_of_props = cast<Class>(obj_class).obj_properties();
	std::vector<props_index_t> prop_idxs(num_of_props);
  /*struct _props_index_s {
    meat::uint32_t offset;
    meat::uint8_t flags;
		} prop_idxs[num_of_props];*/

#ifdef DEBUG
  std::cout << "         -> " << std::dec << (unsigned int)num_of_props
            << " properties " << std::endl;
#endif /* DEBUG */

  // Read in the index table for the objects properties
  for (int c = 0; c < num_of_props; c++) {
    data_stream.read((char *)&prop_idxs[c].offset, sizeof(uint32_t));
    prop_idxs[c].offset = endian::read_be(prop_idxs[c].offset);
    prop_idxs[c].flags = data_stream.get();
  }

  /* Record the object before loading the properties incase one of the
   * properties references this object. If we record it after loading the
   * properties it creates an endless loop.
   */
  this->index.at(index).object = obj.weak();

  // Load the objects properties.
  for (int c = 0; c < num_of_props; c++) {
    if (prop_idxs[c].flags and OBJECT_PROP_WEAK)
      obj->property(c) = (this->get_object(prop_idxs[c].offset)).weak();
    else
      obj->property(c) = this->get_object(prop_idxs[c].offset);
  }

	obj->unserialize(*this, data_stream);

  data_stream.seekg(save_pos);
  return obj;
}

/*****************************
 * meat::data::Archive::sync *
 *****************************/

void meat::data::Archive::sync() {
  /* If we are not in create mode then do nothing. We shouldn't need to throw
   * an error here, I cannot really see the point.
   */
  if (create == false) return;
  if (synced) return;

#ifdef DEBUG
  std::cout << "ARCHIVE: Writing archive " << name << std::endl;
#endif /* DEBUG */

  data_stream.open(this->name.c_str(),
                   std::ios::out | std::ios::binary);

  if (data_stream.is_open()) {
    sgedat_header_t header = {
      {'M', 'A', 'R', 'C'},
      1, 0,
      0, 0  // These are offsets in the file we don't have yet.
    };

    /*  We initially write the header to the file here to make sure that all
     * data after is in the right places. We have to rewrite this header with
     * all the offsets filled in later.
     */
    data_stream.write((char *)&header, sizeof(sgedat_header_t));

    /* Write the objects to the file. */
    for (size_t c = 0; c < index.size(); c++) {
      // Get the offset into the file of the object
      index[c].obj_offset = data_stream.tellp();

      // Now serialize any object data to the file
      if (!(index[c].object.is_null())) {
        uint8_t props = index[c].object->properties();

#ifdef TESTING
        meat::test::test("Serializing number of properties", false);
        if (props != cast<Class>(index[c].object->type()).obj_properties()) {
          meat::test::failed("Serializing number of properties", false);
#ifdef DEBUG
          std::cout << "      " << std::dec << (unsigned int)props << " != "
                    << (unsigned int)cast<Class>(index[c].object->type()).obj_properties()
                    << " for type " << cast<Class>(index[c].object->type())
                    << std::endl;
#endif
        }
#endif /* TESTING */

        // Now add the properties to the index and record the index offset.
        for (uint8_t i = 0; i < props; i++) {
          uint32_t idx =
            endian::write_be(add_property((index[c].object)->property(i)));
          data_stream.write((char *)&idx, 4);

          if ((index[c].object)->property(i).is_weak())
            data_stream.put(OBJECT_PROP_WEAK);
          else
            data_stream.put(OBJECT_PROP);
        }

				// Now add any binary object data
				index[c].object->serialize(*this, data_stream);
      } else
        index[c].obj_offset = 0;
    }

    header.import_offset = endian::write_be((uint32_t)data_stream.tellp());
#ifdef DEBUG
    std::cout << "       Import offset at " << std::dec
              << (unsigned int)data_stream.tellp() << std::endl;
#endif /* DEBUG */

    /* Write all dependant library names to the object file next.
     */
    meat::uint8_t import_cnt = imports.size();
    data_stream.put(import_cnt);
    for (std::vector<std::string>::iterator it = imports.begin();
         it != imports.end();
         it++) {
      data_stream.write(it->data(), it->length());
      data_stream.put('\0');
    }

    header.index_offset = endian::write_be((uint32_t)data_stream.tellp());

    /* Write the object index to the file next. */
    meat::uint32_t index_cnt = endian::write_be((uint32_t)index.size());
    data_stream.write((char *)&index_cnt, 4);
    for (std::vector<Index>::iterator it = index.begin();
         it != index.end(); it++) {
      meat::uint32_t hash_id = endian::write_be(it->cls_id);
      data_stream.write((char *)&hash_id, 4);

      meat::uint32_t offset = endian::write_be(it->obj_offset);
      data_stream.write((char *)&offset, 4);
    }

    /* Rewrite the header with the new offset information. */
    data_stream.seekp(0);
    data_stream.write((char *)&header, sizeof(sgedat_header_t));

    data_stream.close();

    synced = true;
  }
}

/************************************
 * meat::data::Archive::operator >> *
 ************************************/

meat::data::Archive &
meat::data::Archive::operator >>(meat::uint8_t &value) {
  value = (meat::uint8_t)data_stream.get();
  return *this;
}

/************************************
 * meat::data::Archive::operator >> *
 ************************************/

meat::data::Archive &meat::data::Archive::operator >>(meat::int8_t &value) {
  value = (meat::int8_t)data_stream.get();
  return *this;
}

/************************************
 * meat::data::Archive::operator >> *
 ************************************/

meat::data::Archive &meat::data::Archive::operator >>(meat::uint32_t &value)
{
  data_stream.read((char *)&value, sizeof(meat::uint32_t));
  value = endian::read_be(value);
  return *this;
}

/************************************
 * meat::data::Archive::operator >> *
 ************************************/

meat::data::Archive &meat::data::Archive::operator >>(meat::int32_t &value) {
  data_stream.read((char *)&value, sizeof(meat::uint32_t));
  value = endian::read_be(value);
  return *this;
}

/************************************
 * meat::data::Archive::operator >> *
 ************************************/

meat::data::Archive &meat::data::Archive::operator >>(meat::float_t &value) {
  data_stream.read((char *)&value, sizeof(meat::float_t));
  value = endian::read_be(value);
  return *this;
}

/************************************
 * meat::data::Archive::operator << *
 ************************************/

meat::data::Archive &meat::data::operator <<(meat::data::Archive &archive,
                                               meat::uint8_t value) {
  archive.data_stream.put(value);
  return archive;
}

/************************************
 * meat::data::Archive::operator << *
 ************************************/

meat::data::Archive &meat::data::operator <<(meat::data::Archive &archive,
                                               meat::int8_t value) {
  archive.data_stream.put(value);
  return archive;
}

/************************************
 * meat::data::Archive::operator << *
 ************************************/

meat::data::Archive &meat::data::operator <<(meat::data::Archive &archive,
                                               meat::uint32_t value) {
  meat::uint32_t temp = endian::write_be(value);
  archive.data_stream.write((char *)&temp, sizeof(meat::uint32_t));
  return archive;
}

/************************************
 * meat::data::Archive::operator << *
 ************************************/

meat::data::Archive &meat::data::operator <<(meat::data::Archive &archive,
                                               meat::int16_t value) {
  meat::int16_t temp = endian::write_be(value);
  archive.data_stream.write((char *)&temp, sizeof(meat::int16_t));
  return archive;
}

/************************************
 * meat::data::Archive::operator << *
 ************************************/

meat::data::Archive &meat::data::operator <<(meat::data::Archive &archive,
                                               meat::uint16_t value) {
  meat::uint16_t temp = endian::write_be(value);
  archive.data_stream.write((char *)&temp, sizeof(meat::uint16_t));
  return archive;
}

/************************************
 * meat::data::Archive::operator << *
 ************************************/

meat::data::Archive &meat::data::operator <<(meat::data::Archive &archive,
                                               meat::int32_t value) {
  meat::int32_t temp = endian::write_be(value);
  archive.data_stream.write((char *)&temp, sizeof(meat::int32_t));
  return archive;
}

/************************************
 * meat::data::Archive::operator << *
 ************************************/

meat::data::Archive &meat::data::operator <<(meat::data::Archive &archive,
                                               meat::float_t value) {
  meat::float_t temp = endian::write_be(value);
  archive.data_stream.write((char *)&temp, sizeof(meat::float_t));
  return archive;
}

/******************************************************************************
 * Archive Class
 */

// class method create:
static meat::Reference Archive_cm_create_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference filename = meat::cast<meat::Context>(context).parameter(0);

  return new meat::data::Archive(meat::cast<meat::Text>(filename).c_str(), true);
}

// class method open:
static meat::Reference Archive_cm_open_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference filename = meat::cast<meat::Context>(context).parameter(0);

  return new meat::data::Archive(meat::cast<meat::Text>(filename).c_str());
}

static meat::vtable_entry_t ArchiveCMethods[] = {
  {0x0000043c, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x0650a330, 0x36a178be, VTM_NATIVE, 1, Archive_cm_open_},
  {0x068b6f7b, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x2c296348, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x3d4e7ee8, 0x36a178be, VTM_NATIVE, 1, Archive_cm_create_},
  {0x54aa30e6, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}}
};

// method getObject
static meat::Reference Archive_om_getObject(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();

  meat::data::Archive &store_obj = (meat::data::Archive &)(*self);
  return store_obj.get_object();
}

// method import:
static meat::Reference Archive_om_import_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference filename = meat::cast<meat::Context>(context).parameter(0);

  meat::data::Archive &archive_obj = (meat::data::Archive &)(*self);
  meat::data::Library::import(meat::cast<meat::Text>(filename).c_str());
  archive_obj.add_import(meat::cast<meat::Text>(filename).c_str());
  return null;
}

// method setObject:
static meat::Reference Archive_om_setObject_(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();
  meat::Reference value = meat::cast<meat::Context>(context).parameter(0);

  meat::data::Archive &archive_obj = (meat::data::Archive &)(*self);
  archive_obj.set_object(value);
  return null;
}

// method sync
static meat::Reference Archive_om_sync(meat::Reference context) {
  meat::Reference self = meat::cast<meat::Context>(context).self();

  ((meat::data::Archive &)(*self)).sync();
  return null;
}

static meat::vtable_entry_t ArchiveMethods[] = {
  {0x0000043c, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x00361a9b, 0x36a178be, VTM_NATIVE, 0, Archive_om_sync},
  {0x00368f3a, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x28186b3f, 0x36a178be, VTM_NATIVE, 0, Archive_om_getObject},
  {0x34003578, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x4e688b77, 0x36a178be, VTM_NATIVE, 1, Archive_om_setObject_},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x72cd0161, 0x36a178be, VTM_NATIVE, 1, Archive_om_import_},
  {0x7a8e569a, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER,  0, {(meat::method_ptr_t)0}}
};

/******************************************************************************
 * Public Interface
 */

void meat::data::initialize() {
  meat::Class *cls;

  Library::add_path("");

  /* Create the Library class. */
  cls = new Class(Class::resolve("Object"));
	cls->set_vtable(9, LibraryMethods, STATIC);
  cls->set_class_vtable(12, LibraryCMethods, STATIC);
  Class::record(cls, "Library");

  /* Create the Archive class */
  cls = new Class(Class::resolve("Object"), 2);
  cls->set_vtable(13, ArchiveMethods, STATIC);
  cls->set_class_vtable(11, ArchiveCMethods, STATIC);
  Class::record(cls, "Archive");
}
