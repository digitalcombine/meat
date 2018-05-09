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

#include <cstring>
#include <fstream>
#include <cmath>
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

using namespace meat;

#define null (meat::Null())

extern "C" {
  typedef struct __attribute__((packed)) _meat_header_s {
    char magic[4];            // File type magic
    std::uint8_t major_ver;
    std::uint8_t minor_ver;
  } meat_header_t;
}

/******************************************************************************
 * Dynamic Library importing abstract interface
 */

static bool fexists(const char *filename) {
  return (access(filename, F_OK) == 0);
}

// XXX This should be turned into a class

static nativelib_t dl_open(const std::string &filename) {
#if defined (__linux__) || defined (__FreeBSD__)
  void *handle = dlopen(filename.c_str(), RTLD_LAZY);
  if (handle == NULL)
    throw meat::Exception(dlerror());
  return handle;
#endif
}

static void dl_close(nativelib_t handle) {
#if defined (__linux__) || defined (__FreeBSD__)
  if (dlclose(handle) != 0)
    std::cerr << "WARNING: dlclose " << dlerror() << std::endl;
#endif
}

static void *dl_symbol(nativelib_t handle, const std::string &funcname) {
#if defined (__linux__) || defined (__FreeBSD__)
  return dlsym(handle, funcname.c_str());
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

 #define _name (property(0))
 #define _requirements (property(1))
 #define _classes (property(2))
 #define _symbols (property(3))

 #define _registry (type()->property(0))
 #define _paths (type()->property(1))

/********************************
 * meat::data::Library::Library *
 ********************************/

meat::data::Library::Library(const std::string &name)
  : Object(Class::resolve("Library")), is_native(false), dlhandle(nullptr) {

  _requirements = new List;
  _classes = new List;
  _symbols = new Index;

  Reference key = new Text(name);
  auto library = cast<Index>(_registry).find(key);
  if (library != cast<Index>(_registry).end())
    throw meat::Exception(std::string("Library ") + name +
                          " is already loaded.");

  _name = new Text(name);
}

meat::data::Library::Library(Reference type, std::uint8_t properties)
  : Object(type, properties), is_native(false), dlhandle(nullptr) {
}

/*********************************
 * meat::data::Library::~Library *
 *********************************/

meat::data::Library::~Library() throw() {
#ifdef DEBUG
  std::cout << "LIBRARY: Final unloading of " << cast<Text>(_name) << std::endl;
#endif
  _requirements = nullptr;
  for (auto &cls: cast<List>(_classes)) Class::unrecord(cls);

  if (is_native) dl_close(dlhandle);
}

/*******************************
 * meat::data::Library::import *
 *******************************/

meat::Reference meat::data::Library::import(const std::string &name) {
  // Check to see if we have already imported the library.
  Reference context = message(Class::resolve("Library"), "import:",
                              meat::Null());
  cast<Context>(context).parameter(0, new Text(name));
  return meat::execute(context);
}

/********************************
 * meat::data::Library::execute *
 ********************************/

meat::Reference meat::data::Library::execute(const std::string &name) {
  Library *imported_lib = new Library(name);
  imported_lib->import_from_archive(name);

  if (not imported_lib->application.is_null()) {
    // Message the application and get things rolling.
    auto context = meat::message(imported_lib->application,
                                 "entry", meat::Null());
    return meat::execute(context);
  }
  throw meat::Exception(std::string("Library ") + name +
                        " is not executable.");
}

/****************************
 * meat::data::Library::get *
 ****************************/

meat::Reference meat::data::Library::get(const std::string &name) {
  Reference library = Class::resolve("Library");
  auto it = cast<Index>(library->property(0)).find(new Text(name));
  if (it != cast<Index>(library->property(0)).end()) {
    return it->second;
  }
  return NULL;
}

/*******************************
 * meat::data::Library::import *
 *******************************/

void meat::data::Library::import() {

#ifdef DEBUG
  std::cout << "LIBRARY: Importing " << cast<Text>(_name) << std::endl;
#endif

  // First see if the library is already loaded.
  /*std::map<std::string, Library *>::iterator lib = get_libraries().find(_name);
  if (lib != get_libraries().end()) {
    return;
  }*/

  /*  Search the path for the library. This code needs a serious review and
   * may be prone to many unexpected errors.
   */
  std::ifstream lib_file;

  bool found = false;
  for (auto &path: cast<List>(_paths)) {
  if (fexists((cast<Text>(path) + (cast<Text>(_name) + ".mlib")).c_str())) {
      import_from_archive(cast<Text>(path) + (cast<Text>(_name) + ".mlib"));
      found = true;
      break;
    } else if (fexists((cast<Text>(path) + (cast<Text>(_name) + DLLEXT)).c_str())) {
      // The .mlib failed, now try try to open a native library file.
      import_from_native(cast<Text>(path) + (cast<Text>(_name) + DLLEXT),
                         cast<Text>(_name));
      found = true;
      break;
    }
  }
  if (!found) {
    /* OMG, we couldn't find the library. Must be a vegetarian :P */
    throw meat::Exception(std::string("Unable to find library ") +
                          cast<Text>(_name));
  }

  // Initialize all the imported classes.
  init_classes();
}

/*************************************
 * meat::data::Library::init_classes *
 *************************************/

void meat::data::Library::init_classes() {
  for (auto &cls: cast<List>(_classes))
    meat::execute(message(cls, "initialize", meat::Null()));
}

/*********************************
 * meat::data::Library::add_path *
 *********************************/

void meat::data::Library::add_path(const std::string &name) {
  Reference library = Class::resolve("Library");
  cast<List>(library->property(1)).push_front(new Text(name));
}

/*********************************
 * meat::data::Library::requires *
 *********************************/

void meat::data::Library::requires(const std::string &import_name) {
  cast<List>(_requirements).push_back(
    meat::data::Library::import(import_name));
}

/****************************
 * meat::data::Library::add *
 ****************************/

void meat::data::Library::add(Class *cls, const std::string &id) {
  if (not cls->is_class())
    throw meat::Exception("Only classes can be added to library files.");

#ifdef DEBUG
  std::cout << "LIBRARY: Adding class " << id << std::endl;
#endif /* DEBUG */

  if (name() != "__builtin__") {
    cls->name(id);
    Reference newcls = cls;
    cast<List>(_classes).push_back(newcls);
    Class::record(newcls);
  } else {
    cast<List>(_classes).push_back(Class::resolve(id));
  }

  cls->library = this;
}

/******************************
 * meat::data::Library::dlsym *
 ******************************/

void *meat::data::Library::dlsymbol(const std::string &name) {
  if (dlhandle) {
    void *result = dl_symbol(dlhandle, name.c_str());
    if (!result) {
      throw Exception(std::string("Unable to load DL symbol \"") + name +
                      "\" from library " + cast<Text>(_name));
    }
    return result;
  }
  throw Exception(std::string("Unable to load DL symbol \"") + name +
                              "\" from non-native library " +
                              cast<Text>(_name));
}

const std::string &meat::data::Library::name() const {
  return cast<Text>(_name);
}

/************************************
 * meat::data::Library::set_symbols *
 ************************************/

void meat::data::Library::set_symbols(std::uint8_t *symbols) {
  cast<Index>(_symbols).clear();

  // Build symbol index table.
  const char *sym = (const char *)symbols;
  while (*sym != 0) {
    Reference hash_id = new Value((int32_t)hash(sym));
    cast<Index>(_symbols)[hash_id] = new Text(sym);
    sym += std::strlen(sym) + 1;
  }
}

/*******************************
 * meat::data::Library::lookup *
 *******************************/

std::string meat::data::Library::lookup(std::uint32_t hash_id) const {
  Reference _hash_id = new Value((int32_t)hash_id);
  auto it = cast<Index>(_symbols).find(_hash_id);
  if (it != cast<Index>(_symbols).end()) return cast<Text>(it->second);
  return itohex(hash_id);
}

const meat::List &meat::data::Library::get_classes() const {
  return cast<List>(_classes);
}

/********************************************
 * meat::data::Library::import_from_archive *
 ********************************************/

void meat::data::Library::import_from_archive(const std::string &name) {
  meat_header_t header;

  std::ifstream lib_file(name.c_str(), std::ios::in | std::ios::binary);

  if (not lib_file.is_open())
    throw meat::Exception(std::string("Unable to open file ") + name);

  // Read in the file header and make sure the it's the right type of file.
  lib_file.read((char *)&header, sizeof(meat_header_t));
  if (strncmp(header.magic, "MLIB", 4) != 0) {
    throw meat::Exception(std::string("Attempting to import a non-library"
                                      " file ") + name);
  }

  // Library flags.
  lib_file.get();

  // Application class index.
  std::uint32_t app_id;
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
    requires(import_name);
  }

  /* Import all the classes from the file. */
  unsigned int class_cnt = lib_file.get();
#ifdef DEBUG
  std::cout << "LIBRARY: Importing " << std::dec
            << (int)class_cnt << " classes" << std::endl;
#endif
  for (unsigned int cc = 0; cc < class_cnt; cc++) {
    Reference cls = meat::Class::import(lib_file);
    cast<List>(_classes).push_back(cls);
    Class::record(cls);
    cast<Class>(cls).library = this;
    if (app_id && cast<Class>(cls).hash_id() == app_id)
      application = cls;
  }

  // Read in the symbols table.
  std::uint32_t sz;
  lib_file.read((char *)&sz, 4);
  sz = endian::read_be(sz);
  if (sz) {
    std::uint8_t *syms = new std::uint8_t[sz];
    lib_file.read((char *)syms, sz);
    set_symbols(syms);
    delete []syms;
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
    (init_proc_t)dl_symbol(dlhandle, std::string("init_") + name);
  if (init_proc != NULL) {
    init_proc(*this);
  } else
    throw meat::Exception(std::string("Unable to initialize library ") +
                          name + ": " + dlerror());
}

/******************************************************************************
 * class meat::data::Archive
 */

#define OBJECT_PROP      0x00
#define OBJECT_PROP_WEAK 0x01

typedef struct __attribute__((packed)) _meat_arch_header_s {
  char magic[4];     // Should be MARC
  std::uint8_t major_ver;
  std::uint8_t minor_ver;
  std::uint32_t import_offset;
  std::uint32_t index_offset;
} meat_arch_header_t;

/********************************
 * meat::data::Archive::Archive *
 ********************************/

meat::data::Archive::Archive(const std::string &filename, bool create)
  : Object(Class::resolve("Archive")), name(filename), create(create)  {
  synced = false;
  if (!this->create) {

#ifdef DEBUG
    std::cout << "ARCHIVE: Openning file " << name << std::endl;
#endif

    std::ifstream dat_file((this->name).c_str(),
                           std::ios::in | std::ios::binary);

    if (dat_file.is_open()) {
      meat_arch_header_t header;

      // Read the header and file magic ID.
      dat_file.read((char *)&header, sizeof(meat_arch_header_t));
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
      std::uint32_t index_count;
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
      for (std::uint_fast32_t c = 0; c < index_count; c++) {
        std::uint32_t hash_id;
        dat_file.read((char *)&hash_id, 4);
        index[c].cls_id = endian::read_be(hash_id);

        std::uint32_t offset;
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

std::uint32_t meat::data::Archive::add_property(Reference property) {
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
  std::uint32_t offset;
  std::uint8_t flags;
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

    std::uint8_t value;
    data_stream >> value; // Object data value type

    data_stream.seekg(save_pos);
    return meat::Boolean(value != 0);
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
  std::uint8_t num_of_props = cast<Class>(obj_class).obj_properties();
  std::vector<props_index_t> prop_idxs(num_of_props);

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
    meat_arch_header_t header = {
      {'M', 'A', 'R', 'C'},
      1, 0,
      0, 0  // These are offsets in the file we don't have yet.
    };

    /*  We initially write the header to the file here to make sure that all
     * data after is in the right places. We have to rewrite this header with
     * all the offsets filled in later.
     */
    data_stream.write((char *)&header, sizeof(meat_arch_header_t));

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

    header.import_offset = endian::write_be((std::uint32_t)data_stream.tellp());
#ifdef DEBUG
    std::cout << "       Import offset at " << std::dec
              << (unsigned int)data_stream.tellp() << std::endl;
#endif /* DEBUG */

    /* Write all dependant library names to the object file next.
     */
    std::uint8_t import_cnt = imports.size();
    data_stream.put(import_cnt);
    for (std::vector<std::string>::iterator it = imports.begin();
         it != imports.end();
         it++) {
      data_stream.write(it->data(), it->length());
      data_stream.put('\0');
    }

    header.index_offset = endian::write_be((uint32_t)data_stream.tellp());

    /* Write the object index to the file next. */
    std::uint32_t index_cnt = endian::write_be((uint32_t)index.size());
    data_stream.write((char *)&index_cnt, 4);
    for (std::vector<Index>::iterator it = index.begin();
         it != index.end(); it++) {
      std::uint32_t hash_id = endian::write_be(it->cls_id);
      data_stream.write((char *)&hash_id, 4);

      std::uint32_t offset = endian::write_be(it->obj_offset);
      data_stream.write((char *)&offset, 4);
    }

    /* Rewrite the header with the new offset information. */
    data_stream.seekp(0);
    data_stream.write((char *)&header, sizeof(meat_arch_header_t));

    data_stream.close();

    synced = true;
  }
}

/************************************
 * meat::data::Archive::operator >> *
 ************************************/

meat::data::Archive &
meat::data::Archive::operator >>(std::uint8_t &value) {
  value = (std::uint8_t)data_stream.get();
  return *this;
}

/************************************
 * meat::data::Archive::operator >> *
 ************************************/

meat::data::Archive &meat::data::Archive::operator >>(std::int8_t &value) {
  value = (std::int8_t)data_stream.get();
  return *this;
}

/************************************
 * meat::data::Archive::operator >> *
 ************************************/

meat::data::Archive &meat::data::Archive::operator >>(std::uint32_t &value)
{
  data_stream.read((char *)&value, sizeof(std::uint32_t));
  value = endian::read_be(value);
  return *this;
}

/************************************
 * meat::data::Archive::operator >> *
 ************************************/

meat::data::Archive &meat::data::Archive::operator >>(std::int32_t &value) {
  data_stream.read((char *)&value, sizeof(std::uint32_t));
  value = endian::read_be(value);
  return *this;
}

/************************************
 * meat::data::Archive::operator >> *
 ************************************/

meat::data::Archive &meat::data::Archive::operator >>(double &value) {
  std::int64_t mi;
  std::int32_t exponent;

  // Read in the mantissa and exponent of the float point value.
  data_stream.read((char *)&mi, sizeof(std::int64_t));
  data_stream.read((char *)&exponent, sizeof(std::int32_t));

  // Convert to the machine endian.
  mi = endian::read_be(mi);
  exponent = endian::read_be(exponent);

  // Return the mantissa from an integer value to a double.
  double mantissa = ldexp(mi, -53);

  // Put the value back together.
  value = ldexp(mantissa, exponent);
  return *this;
}

/************************************
 * meat::data::Archive::operator << *
 ************************************/

meat::data::Archive &meat::data::operator <<(meat::data::Archive &archive,
                                             std::uint8_t value) {
  archive.data_stream.put(value);
  return archive;
}

/************************************
 * meat::data::Archive::operator << *
 ************************************/

meat::data::Archive &meat::data::operator <<(meat::data::Archive &archive,
                                             std::int8_t value) {
  archive.data_stream.put(value);
  return archive;
}

/************************************
 * meat::data::Archive::operator << *
 ************************************/

meat::data::Archive &meat::data::operator <<(meat::data::Archive &archive,
                                             std::uint32_t value) {
  std::uint32_t temp = endian::write_be(value);
  archive.data_stream.write((char *)&temp, sizeof(std::uint32_t));
  return archive;
}

/************************************
 * meat::data::Archive::operator << *
 ************************************/

meat::data::Archive &meat::data::operator <<(meat::data::Archive &archive,
                                             std::int16_t value) {
  std::int16_t temp = endian::write_be(value);
  archive.data_stream.write((char *)&temp, sizeof(std::int16_t));
  return archive;
}

/************************************
 * meat::data::Archive::operator << *
 ************************************/

meat::data::Archive &meat::data::operator <<(meat::data::Archive &archive,
                                             std::uint16_t value) {
  std::uint16_t temp = endian::write_be(value);
  archive.data_stream.write((char *)&temp, sizeof(std::uint16_t));
  return archive;
}

/************************************
 * meat::data::Archive::operator << *
 ************************************/

meat::data::Archive &meat::data::operator <<(meat::data::Archive &archive,
                                             std::int32_t value) {
  std::int32_t temp = endian::write_be(value);
  archive.data_stream.write((char *)&temp, sizeof(std::int32_t));
  return archive;
}

/************************************
 * meat::data::Archive::operator << *
 ************************************/

meat::data::Archive &meat::data::operator <<(meat::data::Archive &archive,
                                             double value) {
  // Get the mantissa and expontent of the float point value.
  std::int32_t exponent;
  double mantissa = frexp(value, &exponent);

  // Convert the mantissa to an integer value.
  std::int64_t mi = trunc(ldexp(mantissa, 53));

  // Convert to big endian.
  exponent = endian::write_be(exponent);
  mi = endian::write_be(mi);

  // Write it to the stream.
  archive.data_stream.write((char *)&mi, sizeof(std::int64_t));
  archive.data_stream.write((char *)&exponent, sizeof(std::int32_t));

  return archive;
}
