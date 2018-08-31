/*                                                                  -*- c++ -*-
 * A simple Meat compiler for building Libraries and Archives.
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
 */

#include <iostream>
#include <fstream>
#include <meat.h>
#include <meat/utilities.h>
#include <meat/bytecode.h>
#include <cstring>
#include <sstream>
#include <list>
#include <map>
#include <cmath>
#include "dump.h"

extern "C" {
  typedef struct _meat_header_s {
   char magic[4];            // File type magic
   std::uint8_t major_ver;
   std::uint8_t minor_ver;
  } meat_header_t;
}

typedef struct _vtable_entry_s {
  std::uint32_t hash_id; // All entries must be sorted by this id.
  std::uint32_t class_id;
  std::uint8_t flags;
  std::uint8_t locals;   // This includes parameters with local variables
  std::uint16_t offset;
} vtable_entry_t;

class Class {
public:
  uint32_t hash_id;
  uint32_t super_hash_id;

  uint8_t object_properties;
  uint8_t class_properties;

  std::vector<vtable_entry_t> vtable;
  std::vector<vtable_entry_t> cvtable;

  std::vector<uint8_t> bytecode;
};

static std::list<Class> classes;
static std::map<std::uint32_t, std::string> symbol_table;

/**************
 * read_class *
 **************/

static void read_class(std::istream &in) {
  Class cls;

  // Class and super hash ids.
  in.read((char *)&cls.hash_id, 4);
  cls.hash_id = endian::read_be(cls.hash_id);
  in.read((char *)&cls.super_hash_id, 4);
  cls.super_hash_id = endian::read_be(cls.super_hash_id);

  // Property counts.
  cls.object_properties = in.get();
  cls.class_properties = in.get();

  // Read in the virtual tables sizes.
  uint8_t no_entries = in.get();
  uint8_t no_centries = in.get();

  cls.vtable.resize(no_entries);
  cls.cvtable.resize(no_centries);

  // Read in the object virtual table.
  for (unsigned int c = 0; c < no_entries; c++) {
    std::uint32_t hash_id, class_id;
    in.read((char *)&hash_id, 4);
    cls.vtable[c].hash_id = endian::read_be(hash_id);
    in.read((char *)&class_id, 4);
    cls.vtable[c].class_id = endian::read_be(class_id);

    cls.vtable[c].flags = in.get();
    cls.vtable[c].locals = in.get();

    std::uint16_t offset;
    in.read((char *)&offset, 2);
    cls.vtable[c].offset = endian::read_be(offset);
  }

  // Read in the class virtual table.
  for (unsigned int c = 0; c < no_centries; c++) {
    std::uint32_t hash_id, class_id;
    in.read((char *)&hash_id, 4);
    cls.cvtable[c].hash_id = endian::read_be(hash_id);
    in.read((char *)&class_id, 4);
    cls.cvtable[c].class_id = endian::read_be(class_id);

    cls.cvtable[c].flags = in.get();
    cls.cvtable[c].locals = in.get();

    std::uint16_t offset;
    in.read((char *)&offset, 2);
    cls.cvtable[c].offset = endian::read_be(offset);
  }

  // Get the bytecode size.
  std::uint16_t bytecode_size;
  in.read((char *)&bytecode_size, 2);
  bytecode_size = endian::read_be(bytecode_size);
  cls.bytecode.resize(bytecode_size);

  // Read in the bytecode.
  in.read((char *)&cls.bytecode[0], bytecode_size);

  classes.push_back(cls);
}

/**********
 * lookup *
 **********/

static std::string lookup(uint32_t hash_id) {
  // Attempt to convert a hash id into a human readable symbol.
  auto res = symbol_table.find(hash_id);
  if (res != symbol_table.end()) {
    return res->second;
  } else {
    return itohex(hash_id);
  }
}

/*********
 * local *
 *********/

static std::string local(std::uint8_t id) {
  switch (id) {
  case 0:
    return "self";
  case 1:
    return "class";
  case 2:
    return "context";
  case 3:
    return "null";
  default: {
    std::stringstream ss;
    ss << "L" << (unsigned int)id;
    return ss.str();
  }
  }
}

/***************
 * disassemble *
 ***************/

static void disassemble(uint8_t *ip, const std::string &pre = "    ") {
  for (;;) {
    meat::bytecode::bytecode_t *bc = (meat::bytecode::bytecode_t *)ip;

    switch (bc->code) {
    case meat::bytecode::NOOP:
      std::cout << pre << "NOOP\n";
      ip += 1;
      break;

    case meat::bytecode::MESG_SUPER:
    case meat::bytecode::MESSAGE:
      switch (bc->code) {
      case meat::bytecode::MESSAGE:
        std::cout << pre << "MESSAGE "; break;
      case meat::bytecode::MESG_SUPER:
        std::cout << pre << "MESSAGE SUPER "; break;
      default: break;
      }
      std::cout << local(bc->o.m.object) << " "
                << lookup(endian::read_be(bc->o.m.message_id));
      if (bc->o.m.parameters) {
        for (uint8_t c = 0; c < bc->o.m.parameters; c++) {
          std::cout << " " << local(bc->o.m.parameter[c]);
        }
      }
      std::cout << "\n";
      ip += 7 + bc->o.m.parameters;
      break;

    case meat::bytecode::MESG_SUPER_RESULT:
    case meat::bytecode::MESG_RESULT:
      std::cout << pre << local(bc->o.mr.result) << " = ";
      switch (bc->code) {
      case meat::bytecode::MESG_RESULT:
        std::cout << "[MESSAGE " << local(bc->o.mr.object); break;
      case meat::bytecode::MESG_SUPER_RESULT:
        std::cout << "[MESSAGE super"; break;
      default: break;
      }
      std::cout << " " << lookup(endian::read_be(bc->o.mr.message_id));
      if (bc->o.mr.parameters) {
        for (uint8_t c = 0; c < bc->o.mr.parameters; c++) {
          std::cout << " " << local(bc->o.mr.parameter[c]);
        }
      }
      std::cout << "]\n";
      ip += 8 + bc->o.mr.parameters;
      break;

    case meat::bytecode::BLOCK:
      std::cout << pre << local(bc->o.bc.result) << " = {\n"
                << pre << "  # " << (unsigned int)bc->o.bc.locals
                << " block variables\n";
      disassemble(ip + 5, pre + "  ");
      std::cout << pre << "}\n";
      ip += endian::read_be(bc->o.bc.code_size) + 5;
      break;

    case meat::bytecode::CONTEXT_END:
      std::cout << pre << "CONTEXT END" << std::endl;
      return;

    case meat::bytecode::ASSIGN_PROP:
    case meat::bytecode::ASSIGN_CLASS_PROP:
      std::cout << pre << local(bc->o.ap.destination) << " = ";
      switch (bc->code) {
      case meat::bytecode::ASSIGN_PROP:
        std::cout << "PROPERTY "; break;
      case meat::bytecode::ASSIGN_CLASS_PROP:
        std::cout << "CLASS PROPERTY "; break;
      default: break;
      }
      std::cout << (unsigned int)(bc->o.ap.property_id + 1) << "\n";
      ip += 3;
      break;

    case meat::bytecode::ASSIGN:
      std::cout << pre << local(bc->o.a.destination) << " = "
                << local(bc->o.a.source) << "\n";
      ip += 3;
      break;

    case meat::bytecode::ASSIGN_CLASS:
      std::cout << pre << local(bc->o.c.destination) << " = "
                << lookup(endian::read_be(bc->o.c.class_id)) << "\n";
      ip += 6;
      break;

    case meat::bytecode::ASSIGN_CONST_TXT:
      std::cout << pre << local(bc->o.ct.destination) << " = "
                << "\"" << bc->o.ct.value << "\"\n";
      ip += strlen((const char *)bc->o.ct.value) + 3;
      break;

    case meat::bytecode::ASSIGN_CONST_INT:
      std::cout << pre << local(bc->o.ci.destination) << " = "
                << (int)endian::read_be(bc->o.ci.value) << "\n";
      ip += 6;
      break;

    case meat::bytecode::ASSIGN_CONST_NUM: {
      std::cout << pre << local(bc->o.cn.destination) << " = ";
      double mantissa = ldexp(endian::read_be(bc->o.cn.mantissa), -53);
      double value = ldexp(mantissa, endian::read_be(bc->o.cn.exponent));
      std::cout << value << "\n";
      ip += 13;
      break;
    }

    case meat::bytecode::ASSIGN_BLOCK_PARAM: {
      std::cout << pre << local(bc->o.ct.destination) << " = BLOCK PARAMETER "
                << local(bc->o.bp.local_index) << "\n";
      ip += 3;
      break;
    }

    case meat::bytecode::SET_PROP:
    case meat::bytecode::SET_CLASS_PROP:
      switch (bc->code) {
      case meat::bytecode::SET_PROP:
        std::cout << pre << "PROPERTY "; break;
      case meat::bytecode::SET_CLASS_PROP:
        std::cout << pre << "CLASS PROPERTY "; break;
      default: break;
      }
      std::cout << (unsigned int)(bc->o.sp.destination + 1) << " = "
                << local(bc->o.sp.source) << "\n";
      ip += 3;
      break;

    default:
      std::cout << pre << std::hex << "0x" << (unsigned int)(bc->code)
                << " Unknown bytecode\n";
      return;
    }
  }
}

/***************
 * dump_libary *
 ***************/

static void dump_libary(std::istream &in) {
  in.get(); // Unused flags at the moment.

  // Application class index.
  std::uint32_t app_id;
  in.read((char *)&app_id, 4);
  app_id = endian::read_be(app_id);

  // Read and import all the necessary libraries.
  int import_cnt = in.get();
  for (int ic = 0; ic < import_cnt; ic++) {
    std::string import_name;

    // Read in the null terminated library name.
    char ch;
    while ((ch = in.get()) != 0)
      import_name += ch;
    std::cout << "Library requires: " << import_name << "\n";
  }
  std::cout << std::flush;

  /* Import all the classes from the file. */
  unsigned int class_cnt = in.get();
  for (unsigned int cc = 0; cc < class_cnt; cc++) {
    read_class(in);
  }

  // Read in the symbols table.
  std::uint32_t sz;
  in.read((char *)&sz, 4);
  sz = endian::read_be(sz);
  std::uint8_t *syms = NULL;
  if (sz) {
    syms = new std::uint8_t[sz];
    in.read((char *)syms, sz);
  }

  const char *sym = (const char *)syms;
  while (sz and *sym != 0) {
    symbol_table[hash(sym)] = sym;
    sym += std::strlen(sym) + 1;
  }

  for (auto &cls: classes) {
    std::cout << "\n" << lookup(cls.super_hash_id)
              << " subclass: " << lookup(cls.hash_id) << " {\n";
    if (cls.object_properties)
      std::cout << "  " << (unsigned int)cls.object_properties
                << " properties\n";
    if (cls.class_properties)
      std::cout << "  " << (unsigned int)cls.class_properties
                << " class properties\n";

    for (auto &entry: cls.vtable) {
      if (entry.class_id == cls.hash_id) {
        std::cout << "\n  method " << lookup(entry.hash_id)
                  << " {\n    # " << (unsigned int)entry.locals
                  << " context variables\n";
        disassemble(&cls.bytecode[entry.offset]);
        std::cout << "  }\n";
      }
    }
    std::cout << "}" << std::endl;
  }
}

/****************
 * dump_archive *
 ****************/

static void dump_archive(std::istream &in) {
  std::uint32_t import_offset;
  std::uint32_t index_offset;

  in.read((char *)&import_offset, 4);
  in.read((char *)&index_offset, 4);

  import_offset = endian::read_be(import_offset);
  index_offset = endian::read_be(index_offset);

  in.seekg(import_offset);
  std::uint8_t import_count = in.get();

  /* Read in all the library dependancy names and import those libraries.
   */
  for (unsigned int ic = 0; ic < import_count; ic++) {
    std::string import_name;

    char ch;
    while ((ch = in.get()) != 0) {
      import_name += ch;
    }

    std::cout << "Library requires: " << import_name << '\n';
  }

  in.seekg(index_offset);
  std::uint32_t index_count;
  in.read((char *)&index_count, 4);
  index_count = endian::read_be(index_count);

  std::cout << (unsigned int)index_count << " objects\n";
}

/**************
 * dump::dump *
 **************/

void dump::dump(const std::string &library) {
  meat_header_t header;

  std::ifstream meat_file(library, std::ios::in | std::ios::binary);

  //if (not meat_file.is_open())
  //  throw std::exception(std::string("Unable to open file ") + name);

  // Read in the file header and make sure the it's the right type of file.
  meat_file.read((char *)&header, sizeof(meat_header_t));
  if (strncmp(header.magic, "MLIB", 4) == 0) {
    std::cout << "Meat Library File V" << (unsigned int)header.major_ver << "."
              << (unsigned int)header.minor_ver << "\n" << std::endl;
    dump_libary(meat_file);
  } else if (strncmp(header.magic, "MARC", 4) == 0) {
    std::cout << "Meat Archive File V" << (unsigned int)header.major_ver << "."
              << (unsigned int)header.minor_ver << "\n" << std::endl;
    dump_archive(meat_file);
  }
}
