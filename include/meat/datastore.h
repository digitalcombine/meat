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

    static const int FL_ARCHIVE = 1;
    static const int FL_LIBRARY = 2;
    static const int FL_UNKNOWN = -1;
    DECLSPEC int fl_type(const std::string &fname);

    /** Access to class library files.
     */
    class DECLSPEC Library {
    public:
      static Library *create(const std::string &name);
      static Library *import(const std::string &name);

      /** If the library has an application
       */
      static Reference execute(const std::string &name);

      static Library *get(const std::string &name);
      static void unload(const std::string &name);

      /** Unload all libraries.
       */
      static void unload();

      virtual ~Library() throw();

      /** Adds a directory path to the list of library paths.
       */
      static void add_path(const std::string &path);

      /** Manually trigger all the initialize class methods for all the
       * classes in the library.
       *
       * @see create
       */
      void init_classes();

      /**
       */
      void add(Class *cls, const std::string &id);

      /** Clears the library of all it's classes and symbols.
       */
      void clear();

      void set_application(const std::string &name);

      /** Get a native dynamic symbol from the library, if it was a native
       * library.
       */
      void *dlsymbol(const std::string &name);

      const std::string &name() { return _name; }

      void set_symbols(std::uint8_t *symbols,
                       meat::alloc_t sym_alloc);
      void clear_symbols();
      std::string lookup(std::uint32_t hash_id) const;

      const std::deque<Reference> &get_classes() const { return classes; }

    protected:
      /**
       * @todo Imports and includes should be moved to the Grinder.Library
       *       class.
       */
      std::deque<Reference> classes;

      /**
       */
      void import();

    private:
      std::string _name;
      bool is_new;

      std::map<std::uint32_t, const char *> syms_table;
      bool syms_free;
      std::uint32_t syms_size;
      std::uint8_t *symbols;

      bool is_native;
      nativelib_t dlhandle;
      Reference application;

      Library(const std::string &name);

      void import_from_archive(const std::string &name);
      void import_from_native(const std::string &filename,
                              const std::string &name);
    };

    /** The meat object store.
     */
    class DECLSPEC Archive : public Object {
    public:
      Archive(const std::string &filename, bool create = false);
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
      Reference get_object(std::uint32_t index);

      void sync();

      /** Adds a property to the data for the object being serialized.
       * @param property reference to the parameter to add to the file.
       * @return The index offset of the object with the file.
       */
      std::uint32_t add_property(Reference property);

      Archive &operator>>(std::uint8_t &value);
      Archive &operator>>(std::int8_t &value);
      Archive &operator>>(std::uint32_t &value);
      Archive &operator>>(std::int32_t &value);
      Archive &operator>>(double &value);

      friend Archive &operator<<(Archive &archive, std::uint8_t value);
      friend Archive &operator<<(Archive &archive, std::int8_t value);
      friend Archive &operator<<(Archive &archive, std::uint16_t value);
      friend Archive &operator<<(Archive &archive, std::int16_t value);
      friend Archive &operator<<(Archive &archive, std::uint32_t value);
      friend Archive &operator<<(Archive &archive, std::int32_t value);
      friend Archive &operator<<(Archive &archive, double value);

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
        std::uint32_t cls_id;
        std::uint32_t obj_offset;
      };

      std::fstream data_stream;

      std::string name;
      bool create;
      Reference object;

      std::vector<std::string> imports;
      std::vector<Index> index;

      bool synced;
    };

    Archive &operator<<(Archive &archive, std::uint8_t value);
    Archive &operator<<(Archive &archive, std::int8_t value);
    Archive &operator<<(Archive &archive, std::uint16_t value);
    Archive &operator<<(Archive &archive, std::int16_t value);
    Archive &operator<<(Archive &archive, std::uint32_t value);
    Archive &operator<<(Archive &archive, std::int32_t value);
    Archive &operator<<(Archive &archive, double value);
  }
}

typedef void *(*init_library_proc_t)(meat::data::Library &library);

#endif /* _MEAT_DATASTORE_H */
