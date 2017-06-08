/*                                                                  -*- c++ -*-
 * Library and Archive data stores.
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

#include <meat.h>
#include <string>
#include <fstream>

#ifndef _MEAT_DATASTORE_H
#define _MEAT_DATASTORE_H

#if defined (__linux__)
typedef void *nativelib_t;
#elif defined (_WIN32) || defined (_WIN64)
typedef HMODULE nativelib_t;
#endif

/** @ns */
namespace meat {
  /** @ns */
  namespace data {

#if defined (__WIN__)
    typedef HMODULE dlhandle;
#else
    typedef void* dlhandle;
#endif

    /** Internal command to initialize the Meat classes.
     */
    DECLSPEC void initialize();

    extern void (*compiler_import)(const char *name);

    static const int FL_ARCHIVE = 1;
    static const int FL_LIBRARY = 2;
    static const int FL_UNKNOWN = -1;
    DECLSPEC int fl_type(const std::string &fname);

    /** Access to class library files.
     */
    class DECLSPEC Library {
    public:
      static Library *create(const char *name);
      static Library *import(const char *name);
      /**
       */
      static Reference execute(const char *name);
      static void include(const std::string &includes);
      static const std::string &include();
      static void unload(const char *name);

      virtual ~Library() throw();

      void import();

      static void add_path(const char *path);

      void add_import(const char *name);

      void add(Class *cls, const char *id);

      void set_application(const std::string &name);

      const char *get_name() { return name.c_str(); }

      /** Creates and writes a new Meat library file if the library was
       * created with the create() method.
       */
      void write();

      const std::deque<std::string> &get_imports() const { return imports; }

      const std::deque<Reference> &get_classes() const { return classes; }

    protected:
      /* These are only used when compiling a new class library file. */
      std::deque<std::string> imports;
      std::deque<Reference> classes;
      std::string includes;

    private:
      std::string name;
      bool is_new;

      bool is_native;
      nativelib_t dlhandle;
      Reference application;

      Library(const char *name);

      void import_from_archive(const char *name);
      void import_from_native(const char *filename, const char *name);
    };

    /** The meat object store.
     */
    class DECLSPEC Archive : public Object {
    public:
      Archive(const char *filename, bool create = false);
      virtual ~Archive() throw();

      void add_import(const char *name);

      /** Set the Object to be serialized into the store.
       * @param Object The Object to serialize.
       * @see Script::Object
       */
      void set_object(Reference &object);

      /** Retrieves an Object from the data store.
       */
      Reference get_object();
      Reference get_object(uint32_t index);

      void sync();

      /** Adds a property to the data for the object being serialized.
       * @param property reference to the parameter to add to the file.
       * @return The index offset of the object with the file.
       */
      uint32_t add_property(Reference property);

      Archive &operator>>(meat::uint8_t &value);
      Archive &operator>>(meat::int8_t &value);
      Archive &operator>>(meat::uint32_t &value);
      Archive &operator>>(meat::int32_t &value);
      Archive &operator>>(meat::float_t &value);

      friend Archive &operator<<(Archive &archive, meat::uint8_t value);
      friend Archive &operator<<(Archive &archive, meat::int8_t value);
      friend Archive &operator<<(Archive &archive, meat::uint16_t value);
      friend Archive &operator<<(Archive &archive, meat::int16_t value);
      friend Archive &operator<<(Archive &archive, meat::uint32_t value);
      friend Archive &operator<<(Archive &archive, meat::int32_t value);
      friend Archive &operator<<(Archive &archive, meat::float_t value);

    private:
      /* Within the file
       *  Header
       *  Object Data
       *   key
       *   object
       *  Import Data
       *  Index Data
       *
       */

      class Index {
      public:
        Index() : obj_offset(0) {};

        Reference object; /** Cached object. */
        uint32_t cls_id;
        uint32_t obj_offset;
      };

      std::fstream data_stream;

      std::string name;
      bool create;
      Reference object;

      std::vector<std::string> imports;
      std::vector<Index> index;

      bool synced;
    };

    Archive &operator<<(Archive &archive, meat::uint8_t value);
    Archive &operator<<(Archive &archive, meat::int8_t value);
    Archive &operator<<(Archive &archive, meat::uint16_t value);
    Archive &operator<<(Archive &archive, meat::int16_t value);
    Archive &operator<<(Archive &archive, meat::uint32_t value);
    Archive &operator<<(Archive &archive, meat::int32_t value);
    Archive &operator<<(Archive &archive, meat::float_t value);
  };
};

typedef void *(*init_library_proc_t)(meat::data::Library &library);

#endif /* _MEAT_DATASTORE_H */
