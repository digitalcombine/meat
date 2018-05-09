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

#if defined (__linux__) || defined(__FreeBSD__)
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

    static const int FL_ARCHIVE = 1;
    static const int FL_LIBRARY = 2;
    static const int FL_UNKNOWN = -1;
    DECLSPEC int fl_type(const std::string &fname);

    /** Access to class library files.
     */
    class DECLSPEC Library : public Object {
    public:
      /**
       */
      static Reference import(const std::string &name);

      /** If the library has an application
       */
      static Reference execute(const std::string &name);

      static Reference get(const std::string &name);

      Library(const std::string &name);
      Library(Reference type, std::uint8_t properties);

      virtual ~Library() throw();

      /** Adds a directory path to the list of library paths.
       */
      static void add_path(const std::string &path);

      void requires(const std::string &path);

      /**
       */
      void import();

      /** Manually trigger all the initialize class methods for all the
       * classes in the library.
       *
       * @see create
       */
      void init_classes();

      /**
       */
      void add(Class *cls, const std::string &id);

      /** Get a native dynamic symbol from the library, if it was a native
       * library.
       */
      void *dlsymbol(const std::string &name);

      const std::string &name() const;

      /**
       */
      void set_symbols(std::uint8_t *symbols);

      /** Attempt to resolve the hash_id to
       */
      std::string lookup(std::uint32_t hash_id) const;

      const meat::List &get_classes() const;

    private:
      bool is_native;
      nativelib_t dlhandle;
      Reference application;

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
