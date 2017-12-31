/*                                                                  -*- c++ -*-
 * The Library Compiler
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


#include <meat/utilities.h>

#include "compiler.h"
#include "ast.h"

#include <algorithm>
#include <string>
#include <sstream>
#include <iomanip>
#include <iterator>

#ifdef DEBUG
#include <iostream>
#endif

#ifdef TESTING
#include <testing.h>
#endif

using namespace meat;

struct _c_vtable_entry_s {
  std::uint32_t hash_id;
  std::string str_hash;
  std::string str_class_hash;
  std::string flags;
  std::uint8_t locals;
  std::string func_name;
};

/** Converts an integer value to a string.
 */
static std::string to_string(unsigned int value) {
  std::ostringstream convert;
  convert << value;
  return convert.str();
}

/** Changes the meat function name to a name more suitable for c++.
 */
static std::string cook_c_name(const std::string &value) {
  std::string method_name(value);
  for (unsigned int c = 0; c < method_name.size(); c++) {
    if (method_name[c] == ':' || method_name[c] == '.')
      method_name[c] = '_';
  }
  return method_name;
}

static bool c_vtable_comp(struct _c_vtable_entry_s left,
                          struct _c_vtable_entry_s right) {
  return left.hash_id < right.hash_id;
}

static bool vtable_comp(struct _vtable_entry_s left,
                        struct _vtable_entry_s right) {
  return left.hash_id < right.hash_id;
}

/******************************************************************************
 *  Exposed internal to give the grinder application access to what it needs
 * to compile text files.
 */

extern "C" {
  DECLSPEC void exec_library(meat::Reference, std::istream &);
  DECLSPEC void exec_class(meat::Reference, std::istream &);
}

void exec_library(meat::Reference library, std::istream &code) {
  cast<grinder::Library>(library).execute(code);
}

void exec_class(meat::Reference klass, std::istream &code) {
  cast<grinder::Class>(klass).execute(code);
  cast<grinder::Class>(klass).create_class();
}

/******************************************************************************
 * Class VtableBuilder
 */

template <class Ty>
class VtableBuilder {
private:
  std::vector<Ty> vtable;

public:
  typedef typename std::vector<Ty>::const_iterator const_iterator;
  typedef typename std::vector<Ty>::iterator iterator;

  VtableBuilder() {}

  void set(const struct _vtable_entry_s *table, uint8_t count) {
    static Ty foo;
    vtable.clear();
    for (unsigned int c = 0; c < count; ++c) {
      vtable.push_back(foo);
      vtable.back().hash_id = table[c].hash_id;
      vtable.back().flags = VTM_SUPER;
    }
  }

  void add_entry(const Ty &entry) {
    iterator it = vtable.begin();
    for (; it != vtable.end(); ++it) {
      if (it->hash_id == entry.hash_id) {
        vtable.erase(it);
        break;
      }
    }

    vtable.push_back(entry);
  }

  std::uint8_t size() { return vtable.size(); }

  const_iterator begin() const { return vtable.begin(); }
  iterator begin() { return vtable.begin(); }
  const_iterator end() const { return vtable.end(); }
  iterator end() { return vtable.end(); }

  void sort() {
    std::sort(vtable.begin(), vtable.end(), vtable_comp);
  }

  struct _vtable_entry_s *entries() {
    return &vtable[0];
  }

  void clear() { vtable.clear(); }
};

/******************************************************************************
 * meat::grinder::Library Class
 */

#define _name (property(0))
#define classes (property(1))
#define applicationClass (property(2))
#define requiredLibraries (property(3))
#define cppInclude (property(4))

/***********************************
 * meat::grinder::Library::Library *
 ***********************************/

grinder::Library::Library(Reference klass,
                          uint8_t properties)
  : Object(klass, properties), library(NULL), to_cpp(false) {

  classes = new List();
  requiredLibraries = new Set();
}

/************************************
 * meat::grinder::Library::~Library *
 ************************************/

grinder::Library::~Library() throw() {
  if (library)
    data::Library::unload(library->name());
}

/***************************************
 * meat::grinder::Library::register_as *
 ***************************************/

void grinder::Library::register_as(const std::string &name) {
  _name = new Text(name);
  library = data::Library::create(name);
}

/*************************************
 * meat::grinder::Library::add_class *
 *************************************/

void grinder::Library::add_class(Reference klass) {
  cast<List>(classes).push_back(klass);
  cast<Class>(klass).library = this;
}

/***********************************
 * meat::grinder::Library::compile *
 ***********************************/

void grinder::Library::compile() {
  library->clear();

  List::const_iterator cit = cast<List>(classes).begin();
  for (; cit != cast<List>(classes).end(); cit++) {
    cast<const Class>(*cit).create_class();
  }

  write();
}

/*********************************
 * meat::grinder::Library::write *
 *********************************/

void grinder::Library::write() {
  if (is_cpp()) {
    // Now write the code to the file stream.
    std::ofstream cpp_file;
    cpp_file.open((cast<Text>(_name) + ".cpp").c_str());
    if (cpp_file.is_open()) {
      write_cpp(cpp_file);
      cpp_file << std::flush;
      cpp_file.close();
    }

  } else {
    library->clear_symbols();

    meat::List::const_iterator cit = cast<List>(classes).begin();
    for (; cit != cast<List>(classes).end(); cit++) {
      meat::cast<const Class>(*cit).update_symbols(symbols);
    }

    std::stringstream syms_table;
    std::set<std::string>::const_iterator it = symbols.begin();
    for (; it != symbols.end(); ++it) {
      syms_table << *it << '\0';
    }
    syms_table << '\0';
    library->set_symbols((std::uint8_t *)syms_table.str().c_str(),
                         meat::COPY);

    std::ofstream cpp_file;
    cpp_file.open((cast<Text>(_name) + ".mlib").c_str());
    if (cpp_file.is_open()) {
      write_mlib(cpp_file);
      cpp_file << std::flush;
      cpp_file.close();
    }
  }
}

/***************************************
 * meat::grinder::Library::unserialize *
 ***************************************/

void meat::grinder::Library::unserialize(data::Archive &store,
                                         std::istream &data_stream) {
  library = data::Library::create(cast<Text>(_name).c_str());

  Set &imports = cast<Set>(requiredLibraries);
  Set::iterator iit = imports.begin();
  for (; iit != imports.end(); ++iit) {
    data::Library::import(cast<Text>(*iit));
  }

  List::iterator cit = cast<List>(classes).begin();
  for (; cit != cast<List>(classes).end(); cit++) {
    cast<Class>(*cit).library = this;
  }
}

/***********************************
 * meat::grinder::Library::command *
 ***********************************/

void grinder::Library::command() {
  message();
}

/**************************************
 * meat::grinder::Library::add_symbol *
 **************************************/

void grinder::Library::add_symbol(const std::string &symbol) {
  symbols.insert(symbol);
}

/*****************************************
 * meat::grinder::Library::clear_symbols *
 *****************************************/

void grinder::Library::clear_symbols() {
  symbols.clear();
}

/**********************************
 * meat::grinder::Library::import *
 **********************************/

void grinder::Library::import(const std::string &library,
                              meat::Reference context) {
  data::Library::import(library);
  cast<Set>(requiredLibraries).insert(new Text(library));
}

/***********************************
 * meat::grinder::Library::include *
 ***********************************/

void grinder::Library::include(const std::string &code) {
  cppInclude = new meat::Text(code);
}

/****************************************
 * meat::grinder::Library::create_class *
 ****************************************/

void grinder::Library::create_class(meat::Reference super,
                                    const std::string &cls_name,
                                    const std::string &cls_body,
                                    meat::Reference context) {
  meat::Reference ctx =
    meat::message(meat::Class::resolve("Grinder.Class"),
                  "from:createSubclass:",
                  context);
  meat::cast<meat::Context>(ctx).parameter(0, super);
  meat::cast<meat::Context>(ctx).parameter(1, new meat::Text(cls_name));
  meat::Reference cb = meat::execute(ctx);

  add_class(cb);

  std::string body(cls_body);
  std::istringstream is(body);

  cast<grinder::Class>(cb).execute(is);
  cast<grinder::Class>(cb).create_class();
}

/*************************************************
 * meat::grinder::Library::set_application_class *
 *************************************************/

void grinder::Library::set_application_class(meat::Reference klass) {
  applicationClass = klass;
}

/***********************************
 * meat::grinder::Library::message *
 ***********************************/

Reference grinder::Library::message() {
  Reference object;
  std::string message;
  Token mesgt = tokens[0];

  std::string class_name = (std::string)(tokens[0]);
  tokens.permit(Token::WORD);
  object = meat::Class::resolve(class_name);

  std::string method_name;
  std::deque<Reference> parameters;

  for (unsigned int c = 0; not tokens.expect(Token::EOL); ++c) {

    if (c % 2 == 0) {
      // Method name
      method_name += (std::string)(tokens[0]);
      tokens.permit(Token::WORD);
    } else {
      // Parameters
      if (tokens.expect(Token::WORD) or tokens.expect(Token::BLOCK) or
          tokens.expect(Token::LITRL_STRING) or
          tokens.expect(Token::SUBST_STRING)) {
        parameters.push_back(new meat::Text((const std::string &)tokens[0]));
        tokens.next();
      } else if (tokens.expect(Token::COMMAND)) {
        tokens.push();
        parameters.push_back(this->message());
        tokens.pop();
      } else {
        throw SyntaxException(tokens[0], "Unexpected token in message");
      }
    }
  }
  tokens.next(); // Remove EOL.

  try {
    Reference new_context = meat::message(object, method_name, context);
    for (unsigned int idx = 0; idx < parameters.size(); ++idx) {
      cast<Context>(new_context).parameter(idx, parameters[idx]);
    }
    context = new_context;
    Reference result = meat::execute(context);
  } catch (Exception err) {
    throw SyntaxException(mesgt, err.what());
  }
  context = cast<Context>(context).messenger();
  return result;
}

/**********************************
 * meat::grinder::Library::is_cpp *
 **********************************/

bool grinder::Library::is_cpp() const {

  // Check the methods to see if any of them are cpp methods.
  List::const_iterator mit = cast<List>(classes).begin();
  for (; mit != cast<List>(classes).end(); mit++) {
    if (meat::cast<Class>(*mit).is_cpp()) {
      return true;
    }
  }
  return false;
}

/**************************************
 * meat::grinder::Library::write_mlib *
 **************************************/

extern "C" {
  typedef struct __attribute__((packed)) _mlib_header_t {
    char magic[4];            // File type magic
    std::uint8_t major_ver;
    std::uint8_t minor_ver;
    std::uint8_t flags;
    std::uint32_t application_id;
  } mlib_header_t;
}

void grinder::Library::write_mlib(std::ostream &out) {
  mlib_header_t header = {
    {'M', 'L', 'I', 'B'},
    1, 0, 0, 0
  };

  // Add the application class hash ID if the library is executable.
  if (!applicationClass.is_null() and !(applicationClass == Null())) {
    header.application_id =
      endian::write_be(hash(cast<Text>(applicationClass)));
  }

  /*  We initially write the header to the file here to make sure that all
   * data after is in the right places. We have to rewrite this header with
   * all the offsets filled in later.
   */
  out.write((const char *)&header, sizeof(mlib_header_t));

  meat::Set &imports = cast<Set>(requiredLibraries);
#ifdef DEBUG
  std::cout << "LIBRARY: Adding " << (int)imports.size()
            << " imports" << std::endl;
#endif /* DEBUG */
  /* Write all import strings to the library file next. */
  std::uint8_t import_cnt = imports.size();

  out.put(import_cnt);
  for (meat::Set::const_iterator it = imports.begin();
       it != imports.end();
       it++) {
    out.write(cast<const Text>(*it).data(),
              cast<const Text>(*it).length());
    out.put('\0');
  }

  // Write the classes to the file.
  const std::deque<Reference> &klasses = library->get_classes();
#ifdef DEBUG
  std::cout << "LIBRARY: Adding " << (int)klasses.size()
            << " classes" << std::endl;
#endif /* DEBUG */
  out.put((uint8_t)klasses.size());
  for (std::deque<Reference>::const_iterator it = klasses.begin();
       it != klasses.end(); it++) {
    cast<const meat::Class>(*it).write(out);
  }

  // Write the symbols table to the file.
  std::stringstream syms_table;
  std::set<std::string>::const_iterator it = symbols.begin();
  for (; it != symbols.end(); ++it) {
    syms_table << *it << '\0';
  }
  syms_table << '\0';

  std::uint32_t sz = endian::write_be((std::uint32_t)syms_table.str().size());
  out.write((const char *)&sz, 4);
  if (syms_table.str().size())
    out.write((const char *)syms_table.str().c_str(),
              syms_table.str().size());
}

/*************************************
 * meat::grinder::Library::write_cpp *
 *************************************/

void grinder::Library::write_cpp(std::ostream &out) {
  out << "/*************************************************************"
      << "*****************\n"
      << " * Auto-generated by the Grinder Compiler Library\n"
      << " */\n\n"
      << "#include <meat.h>\n"
      << "#include <meat/datastore.h>\n"
      << cast<Text>(cppInclude)
      << "\nusing namespace meat;\n"
      << "\n#define null (meat::Null())\n\n";

  List::const_iterator cit;

  // Create all the class methods and vtables.
  for (cit = cast<List>(classes).begin(); cit != cast<List>(classes).end();
       cit++) {
    cast<Class>(*cit).cpp_methods(out);
    cast<const Class>(*cit).update_symbols(symbols);
  }

  // Create symbols table
  if (!symbols.empty()) {
    out << "static std::uint8_t Symbols[] = {\n";

    std::string line = "  \"";

    std::set<std::string>::const_iterator it = symbols.begin();
    for (; it != symbols.end(); ++it) {

      if (line.length() + it->length() + 2 < 79) {
        line += *it + "\\0";
      } else {
        out << line << "\"\n";
        line = std::string("  \"") + *it + "\\0";
      }
    }
    if (line.length() + 2 < 79)
      out << line << "\\0\"\n";
    else
      out << line << "\"\n  \"\\0\"\n";

    out << "};\n";
  }

  out << "\n";

  // Declare the init function using C conventions.
  out << "// We need to declare init_"
      << std::string(library->name()) + " as a C function.\n"
      << "extern \"C\" {\n"
      << "  DECLSPEC void init_" + std::string(library->name())
      << "(meat::data::Library &library);\n}\n\n";

  // Generate the init function.
  out << "void init_" + std::string(library->name())
      << "(meat::data::Library &library) {\n  meat::Class *cls;\n";

  // Import required libraries
  if (not (property(3) == meat::Null())) {
    out << "\n  // Import required libraries.\n";

    meat::Set &imports = cast<Set>(requiredLibraries);
    Set::const_iterator sit;
    for (sit = imports.begin(); sit != imports.end(); ++sit) {
      out << "  meat::data::Library::import(\""
          << meat::cast<const meat::Text>(*sit) << "\");\n";
    }
  }

  // Create all the class methods and vtables.
  for (cit = cast<List>(classes).begin(); cit != cast<List>(classes).end();
       cit++) {
    cast<Class>(*cit).cpp_new_class(out);
  }

  if (!symbols.empty())
    out << "\n  library.set_symbols(Symbols, meat::STATIC);\n";

  out << "}\n";
}

/******************************************************************************
 * meat::grinder::Class Class
 */

#define className (property(0))
#define superClass (property(1))
#define objectProperties (property(2))
#define classProperties (property(3))
#define objectMethods (property(4))
#define classMethods (property(5))
#define constr (property(6))

/*******************************
 * meat::grinder::Class::Class *
 *******************************/

meat::grinder::Class::Class(Reference klass, uint8_t properties)
  : Object(klass, properties), library(NULL), cpp_bytecode(0) {
}

/**************************************
 * meat::Grinder::Class::obj_property *
 **************************************/

std::uint8_t meat::grinder::Class::obj_property(const std::string &name) {
#ifdef DEBUG
  std::cout << "CLASS: Adding property " << name << std::endl;
#endif /* DEBUG */
  List &properties = cast<List>(objectProperties);

  for (uint8_t index = 0; index < properties.size(); index++) {
    if (cast<Text>(properties.at(index)) == name) return index;
  }

  properties.push_back(new Text(name));
  return properties.size() - 1;
}

/**************************************
 * meat::Grinder::Class::cls_property *
 **************************************/

std::uint8_t grinder::Class::cls_property(const std::string &name) {
#ifdef DEBUG
  std::cout << "CLASS: Adding class property " << name << std::endl;
#endif /* DEBUG */
  List &cls_properties = cast<List>(classProperties);

  for (uint8_t index = 0; index < cls_properties.size(); index++) {
    if (cast<Text>(cls_properties.at(index)) == name) return index;
  }

  cls_properties.push_back(new Text(name));
  return cls_properties.size() - 1;
}

/******************************************
 * meat::Grinder::Class::have_obj_propery *
 ******************************************/

std::int16_t grinder::Class::have_obj_property(const std::string &name) const {
  const List &properties = cast<const List>(objectProperties);

  for (uint8_t index = 0; index < properties.size(); index++) {
    if (cast<const Text>(properties.at(index)) == name) return index;
  }
  return -1;
}

/*******************************************
 * meat::Grinder::Class::have_cls_property *
 *******************************************/

std::int16_t grinder::Class::have_cls_property(const std::string &name) const {
  const List &cls_properties = cast<const List>(classProperties);

  for (uint8_t index = 0; index < cls_properties.size(); index++) {
    if (cast<const Text>(cls_properties.at(index)) == name) return index;
  }
  return -1;
}

/************************************
 * meat::Grinder::Class::add_method *
 ************************************/

void grinder::Class::add_method(Reference method) {
  cast<Index>(objectMethods)[cast<Method>(method).property(0)] = method;
  cast<Method>(method).cb = this;
}

/******************************************
 * meat::Grinder::Class::add_class_method *
 ******************************************/

void grinder::Class::add_class_method(Reference method) {
  cast<Index>(classMethods)[cast<Method>(method).property(0)] = method;
  cast<Method>(method).cb = this;
}

/**************************************
 * meat::Grinder::Class::create_class *
 **************************************/

void grinder::Class::create_class() const {

#ifdef DEBUG
  std::cout << "COMPILER: Creating class " << cast<const Text>(className)
            << std::endl;
#endif /* DEBUG */

  const List &properties = cast<const List>(objectProperties);
  const List &cls_properties = cast<const List>(classProperties);

  Reference super = meat::Class::resolve(cast<const Text>(superClass));
  //Reference super = superClass;
  meat::Class *cls =
    new meat::Class(super,
                    (uint8_t)cls_properties.size(),
                    (uint8_t)properties.size() +
                    cast<const meat::Class>(super).obj_properties());

  std::vector<uint8_t> bytecode;

  uint8_t vt_size;
  const vtable_entry_t *vt;
  Index::const_iterator mit;
  VtableBuilder<struct _vtable_entry_s> vtable;
  VtableBuilder<struct _vtable_entry_s>::iterator vtable_it;

  /* Generate the bytecode for the object methods and record it in the
   * virtual table.
   */
  vt = meat::cast<const meat::Class>(super).get_vtable(vt_size);
  vtable.set(vt, vt_size);
  for (vtable_it = vtable.begin(); vtable_it != vtable.end(); ++vtable_it)
    vtable_it->flags = VTM_SUPER;
  const Index &methods = cast<const Index>(objectMethods);

  for (mit = methods.begin(); mit != methods.end(); mit++) {
    struct _vtable_entry_s entry;
    uint16_t offset = bytecode.size();
    cast<grinder::Method>(mit->second).gen_bytecode(bytecode);
    entry.hash_id = hash(cast<const Text>(mit->first));
    entry.class_id = hash(cast<const Text>(className));
    entry.flags = VTM_BYTECODE;
    entry.locals = cast<grinder::Method>(mit->second).locals();
    entry.method.offset = offset;
#ifdef DEBUG
    std::cout << "VTABLE: method " << cast<const Text>(mit->first)
              << " bytecode @ " << offset << std::endl;
#endif
    vtable.add_entry(entry);
    library->add_symbol(cast<const Text>(mit->first));
  }
  std::sort(vtable.begin(), vtable.end(), vtable_comp);
  cls->set_vtable(vtable.size(), vtable.entries(), COPY);
  vtable.clear();

  /* Generate the bytecode for the class methods and record it in the class
   * virtual table.
   */
  vt = meat::cast<const meat::Class>(super).get_class_vtable(vt_size);
  vtable.set(vt, vt_size);
  for (vtable_it = vtable.begin(); vtable_it != vtable.end(); ++vtable_it)
    vtable_it->flags = VTM_SUPER;
  const Index &class_methods = cast<const Index>(classMethods);
  for (mit = class_methods.begin(); mit != class_methods.end(); mit++) {
    struct _vtable_entry_s entry;
    uint16_t offset = bytecode.size();
    cast<grinder::Method>(mit->second).gen_bytecode(bytecode);
    entry.hash_id = hash(cast<const Text>(mit->first));
    entry.class_id = hash(cast<const Text>(className));
    entry.flags = VTM_BYTECODE;
    entry.locals = cast<grinder::Method>(mit->second).locals();
    entry.method.offset = offset;
#ifdef DEBUG
    std::cout << "VTABLE: class method " << cast<const Text>(mit->first)
              << " bytecode @ " << offset << std::endl;
#endif
    vtable.add_entry(entry);
    library->add_symbol(cast<const Text>(mit->first));
  }
  std::sort(vtable.begin(), vtable.end(), vtable_comp);
  cls->set_class_vtable(vtable.size(), vtable.entries(), COPY);
  vtable.clear();

  // Add the bytecode to the class.
  cls->bytecode(bytecode.size(), (uint8_t *)&bytecode[0], COPY);

  // Register the new class and add it to the library.
  library->library->add(cls, cast<const Text>(className).c_str());
  library->add_symbol(cast<const Text>(className));
}

/********************************
 * meat::Grinder::Class::is_cpp *
 ********************************/

bool meat::grinder::Class::is_cpp() const {
  Index::const_iterator mit;

  // Check to see if a c++ constructor is set
  if (!(constr == meat::Null())) {
    return true;
  }

  // Check the methods to see if any of them are cpp methods.
  const Index &methods = cast<const Index>(objectMethods);
  for (mit = methods.begin(); mit != methods.end(); mit++) {
    if (((Method &)(*(mit->second))).is_cpp()) {
      return true;
    }
  }

  // Check the class methods to see if any of them are cpp methods.
  const Index &class_methods = cast<const Index>(classMethods);
  for (mit = class_methods.begin(); mit != class_methods.end(); mit++) {
    if (((Method &)(*(mit->second))).is_cpp()) {
      return true;
    }
  }
  return false;
}

/*************************************
 * meat::Grinder::Class::cpp_methods *
 *************************************/

void meat::grinder::Class::cpp_methods(std::ostream &out) {
  //std::string cppcode;
  std::string class_name = cook_c_name(cast<Text>(className));
  std::vector<uint8_t> bytecode;

  out << "/***************************************************************"
      << "***************\n"
      << " * " << cast<Text>(className) << " Class\n"
      << " */\n\n";

  if (!(constr == meat::Null())) {
    out << "static Reference " + class_name + "_constructor(\n"
        << "  Reference &klass,\n  std::uint8_t properties) {\n"
        << cast<Text>(constr)
        << "\n}\n\n";
  }

  List::const_iterator pit;
  unsigned int c;
  List &obj_props = cast<List>(objectProperties);
  List &cls_props = cast<List>(classProperties);

  if (obj_props.size() > 0) {
    for (pit = obj_props.begin(), c = 0; pit != obj_props.end(); pit++, c++) {
      out << "#define " << cast<const Text>(*pit)
          << " (self->property(" + ::to_string(c) + "))\n";
    }
    out << "\n";
  }
  if (cls_props.size() > 0) {
    for (pit = cls_props.begin(), c = 0; pit != cls_props.end(); pit++, c++) {
      out << "#define " << cast<const Text>(*pit)
          << " (klass->property(" + ::to_string(c) + "))\n";
    }
    out << "\n";
  }

  uint8_t vt_size;
  const vtable_entry_t *vt;
  VtableBuilder<struct _c_vtable_entry_s> vtable;
  VtableBuilder<struct _c_vtable_entry_s>::iterator vtable_it;

  /* Methods */
  Reference super = meat::Class::resolve(cast<const Text>(superClass));

  vt = cast<meat::Class>(super).get_vtable(vt_size);
  vtable.set(vt, vt_size);
  for (vtable_it = vtable.begin(); vtable_it != vtable.end(); ++vtable_it) {
    vtable_it->str_hash = itohex(vtable_it->hash_id);
    vtable_it->str_class_hash = "0x00000000";
    vtable_it->flags = "VTM_SUPER   ";
    vtable_it->func_name = "{(meat::method_ptr_t)0}";
  }
  if (method_count() > 0) {
    Index &methods = cast<Index>(objectMethods);

    Index::const_iterator mit;
    for (mit = methods.begin(); mit != methods.end(); mit++) {
      std::uint32_t hash_id = hash(cast<const Text>(mit->first));
      struct _c_vtable_entry_s entry;

      if (cast<Method>(mit->second).is_cpp()) {
        // Build the virtual table entry.
        entry.hash_id = hash_id;
        entry.str_hash = cast<Method>(mit->second).cpp_hash_id();
        entry.str_class_hash = cpp_hash_id();
        entry.flags = "VTM_NATIVE  ";
        entry.func_name =
          cast<Method>(mit->second).cpp_name((class_name +
                                              "_om_").c_str());

        out << "// method " << cast<const Text>(mit->first) << "\n";
        cast<Method>(mit->second).cpp_method((class_name + "_om_"), out);

        entry.locals = cast<Method>(mit->second).locals();

        vtable.add_entry(entry);
      } else {
        // Build the virtual table entry.
        entry.hash_id = hash_id;
        entry.str_hash = cast<Method>(mit->second).cpp_hash_id();
        entry.str_class_hash = cpp_hash_id();
        entry.flags = "VTM_BYTECODE";
        entry.func_name = "{(meat::method_ptr_t)" +
          ::to_string(bytecode.size()) + "}";
        cast<Method>(mit->second).gen_bytecode(bytecode);
        entry.locals = cast<Method>(mit->second).locals();
        vtable.add_entry(entry);
      }
    }

    // Sort the virtual table according to the hash ids.
    std::sort(vtable.begin(), vtable.end(), c_vtable_comp);
    m_count = vtable.size();

    out << "static meat::vtable_entry_t " << class_name << "Methods[] = {\n";

    bool first = true;
    std::vector<struct _c_vtable_entry_s>::const_iterator it;
    for (it = vtable.begin(); it != vtable.end(); it++) {
      if (!first) out << ",\n";
      else first = false;

      out << "  {" << it->str_hash << ", " << it->str_class_hash
          << ", " << it->flags << ", " << ::to_string(it->locals) << ", "
          << it->func_name << "}";
    }
    out << "\n};\n\n";
  }

  if (obj_props.size() > 0) {
    for (pit = obj_props.begin(); pit != obj_props.end(); pit++) {
      out << "#undef " << cast<const Text>(*pit) << "\n";
    }
    out << "\n";
  }

  /* Class methods */
  vt = cast<meat::Class>(super).get_class_vtable(vt_size);
  vtable.set(vt, vt_size);
  for (vtable_it = vtable.begin(); vtable_it != vtable.end(); ++vtable_it) {
    vtable_it->str_hash = itohex(vtable_it->hash_id);
    vtable_it->str_class_hash = "0x00000000";
    vtable_it->flags = "VTM_SUPER   ";
    vtable_it->func_name = "{(meat::method_ptr_t)0}";
  }
  if (class_method_count() > 0) {
    Index &methods = cast<Index>(classMethods);

    Index::const_iterator mit;
    for (mit = methods.begin(); mit != methods.end(); mit++) {
      std::uint32_t hash_id = hash(cast<const Text>(mit->first));
      struct _c_vtable_entry_s entry;

      if (cast<Method>(mit->second).is_cpp()) {
        // Build the virtual table entry.
        entry.hash_id = hash_id;
        entry.str_hash = cast<Method>(mit->second).cpp_hash_id();
        entry.str_class_hash = cpp_hash_id();
        entry.flags = "VTM_NATIVE  ";
        entry.func_name = cast<Method>(mit->second).cpp_name((class_name +
                                                              "_cm_").c_str());

        out << "// class method " << cast<const Text>(mit->first) << "\n";
        cast<Method>(mit->second).cpp_method((class_name + "_cm_"), out);

        entry.locals = cast<Method>(mit->second).locals();

        vtable.add_entry(entry);
      } else {
        // Build the virtual table entry.
        entry.hash_id = hash_id;
        entry.str_hash = cast<Method>(mit->second).cpp_hash_id();
        entry.str_class_hash = cpp_hash_id();
        entry.flags = "VTM_BYTECODE";
        entry.func_name = "{(meat::method_ptr_t)" +
          ::to_string(bytecode.size()) + "}";
        cast<Method>(mit->second).gen_bytecode(bytecode);
        entry.locals = cast<Method>(mit->second).locals();
        vtable.add_entry(entry);
      }
    }

    // Sort the virtual table according to the hash ids.
    std::sort(vtable.begin(), vtable.end(), c_vtable_comp);
    cm_count = vtable.size();

    out << "static meat::vtable_entry_t " << class_name << "CMethods[] = {\n";

    bool first = true;
    std::vector<struct _c_vtable_entry_s>::const_iterator it;
    for (it = vtable.begin(); it != vtable.end(); it++) {
      if (!first) out << ",\n";
      else first = false;

      out << "  {" << it->str_hash << ", " << it->str_class_hash
          << ", " << it->flags << ", " << ::to_string(it->locals) << ", "
          << it->func_name + "}";
    }
    out << "\n};\n\n";
  }

  if (cls_props.size() > 0) {
    for (pit = cls_props.begin(); pit != cls_props.end(); pit++) {
      out << "#undef " << cast<const Text>(*pit) << "\n";
    }
    out << "\n";
  }

  /* Bytecode generation.
   */
  cpp_bytecode = bytecode.size();
  if (!bytecode.empty()) {
    out << "static std::uint8_t " << class_name << "Bytecode[] = {\n  ";
    bool first = true;
    for (unsigned int c = 0; c < bytecode.size(); c++) {
      if (!first) {
        if (c % 13 == 0)
          out << ",\n  ";
        else
          out << ", ";
      } else first = false;

      out << itohex(bytecode[c], 2);
    }
    out << "\n};\n\n";
  }
}

/***************************************
 * meat::grinder::Class::cpp_new_class *
 ***************************************/

void meat::grinder::Class::cpp_new_class(std::ostream &out) const {
  //std::string cppcode;
  std::string class_name = cast<const Text>(className);
  std::string cooked_name = cook_c_name(class_name);

  Reference super = meat::Class::resolve(cast<const Text>(superClass));

  out << "\n  cls = new meat::Class(meat::Class::resolve("
      << itohex(cast<const meat::Class>(super).hash_id())
      << "), " << ::to_string(cast<const List>(classProperties).size())
      << ", " << ::to_string(cast<const List>(objectProperties).size() +
                             cast<const meat::Class>(super).obj_properties())
      << ");\n";

  if (!(constr == meat::Null())) {
    out << "  cls->set_constructor(" << cooked_name << "_constructor);\n";
  }

  if (method_count() > 0)
    out << "  cls->set_vtable(" << ::to_string(method_count())
        << ", " << cooked_name << "Methods, meat::STATIC);\n";

  if (class_method_count() > 0)
    out << "  cls->set_class_vtable(" << ::to_string(class_method_count())
        << ", " << cooked_name << "CMethods, meat::STATIC);\n";

  if (cpp_bytecode > 0)
    out << "  cls->bytecode(" << ::to_string(cpp_bytecode)
        << ", " << cooked_name << "Bytecode, meat::STATIC);\n";

  out << "  library.add(cls, \"" << class_name << "\");\n";
}

/*************************************
 * meat::grinder::Class::cpp_hash_id *
 *************************************/

std::string meat::grinder::Class::cpp_hash_id() const {
  return itohex(hash(cast<const Text>(this->property(0))));
}

/*********************************
 * meat::grinder::Class::command *
 *********************************/

void meat::grinder::Class::command() {

  if (tokens.expect(Token::EOL)) {
    // Empty line, just skip it.
    tokens.next();

  } else if (tokens.expect(Token::WORD, "class")) {
    // Class definitions.
    if (tokens.expect(1, Token::WORD, "property")) {
      property_def();
    } else if (tokens.expect(1, Token::WORD, "method")) {
      method_def();
    } else if (tokens.expect(1, Token::WORD, "function")) {
      method_def(true);
    } else {
      throw Exception(std::string("Invalid class definition ") +
                      (std::string)tokens[1]);
    }

  } else {
    // Object definitions.
    if (tokens.expect(Token::WORD, "constructor")) {
      tokens.next(); // Remove constructor
      constr = new Text((std::string)tokens[0]);
      tokens.permit(Token::BLOCK); // Remove the block
    } else if (tokens.expect(Token::WORD, "property")) {
      property_def();
    } else if (tokens.expect(Token::WORD, "method")) {
      method_def();
    } else if (tokens.expect(Token::WORD, "function")) {
      method_def(true);
    } else {
      throw Exception(std::string("Invalid definition ") +
                      (std::string)tokens[0]);
    }
  }
}

/****************************************
 * meat::grinder::Class::update_symbols *
 ****************************************/

void grinder::Class::update_symbols(std::set<std::string> &symbols) const {
  const Index &methods = cast<const Index>(objectMethods);

  meat::Index::const_iterator it;
  for (it = methods.begin(); it != methods.end(); ++it) {
    cast<const Method>(it->second).update_symbols(symbols);
  }

  const Index cmethods = cast<const Index>(classMethods);
  for (it = cmethods.begin(); it != cmethods.end(); ++it) {
    cast<const Method>(it->second).update_symbols(symbols);
  }
}

/*************************************
 * meat::grinder::Class::unserialize *
 *************************************/

void grinder::Class::unserialize(data::Archive &store,
                                 std::istream &data_stream) {
  Index &methods = cast<Index>(objectMethods);

  meat::Index::iterator it;
  for (it = methods.begin(); it != methods.end(); ++it) {
    cast<Method>(it->second).cb = this;
  }

  Index cmethods = cast<Index>(classMethods);
  for (it = cmethods.begin(); it != cmethods.end(); ++it) {
    cast<Method>(it->second).cb = this;
  }
}

/**************************************
 * meat::grinder::Class::property_def *
 **************************************/

void grinder::Class::property_def() {
  bool class_property = false;

  // Check if we're defining a class property.
  if (tokens.expect(Token::WORD, "class")) {
    class_property = true;
    tokens.next();
  }

  // Keyword property must be next.
  tokens.permit(Token::WORD, "property");

  // Get the property name and make sure it's a word token.
  std::string property_id = (std::string)tokens[0];
  tokens.permit(Token::WORD);

  // Add the property to the class.
  if (class_property)
    cls_property(property_id);
  else
    obj_property(property_id);

  tokens.permit(Token::EOL);
}

/************************************
 * meat::grinder::Class::method_def *
 ************************************/

void grinder::Class::method_def(bool is_native) {
  bool is_class_method = false;

#if DEBUG
  std::cout << "CLASS: method" << std::endl;
#endif

  if (tokens.expect(Token::WORD, "class")) {
    is_class_method = true;
    tokens.next();
  }
  if (is_native)
    tokens.permit(Token::WORD, "function");
  else
    tokens.permit(Token::WORD, "method");

  Method *mb = new Method(*this, is_native);
  std::string method_name;
  meat::List *method_name_list = new meat::List();

  for (int c = 0; not tokens.expect(Token::BLOCK); ++c) {
    if (c % 2 == 0) {
      method_name += (std::string)tokens[0];
      method_name_list->push_back(new Text((std::string)tokens[0]));
      tokens.permit(Token::WORD);
    } else {
      mb->add_parameter(tokens[0]);
      tokens.permit(Token::WORD);
    }
  }

  Reference name = new Text(method_name);
  if (is_class_method)
    cast<Index>(classMethods)[name] = mb;
  else
    cast<Index>(objectMethods)[name] = mb;
  mb->property(0) = method_name_list;
  mb->add_body(tokens[0]);
  tokens.permit(Token::BLOCK);

  mb->compile();
}

/**************************************
 * meat::grinder::Class::method_count *
 **************************************/

uint8_t grinder::Class::method_count() const {
  if (m_count == 0) {
    const Index &methods = cast<const Index>(objectMethods);
    return methods.size();
  } else
    return m_count;
}

/********************************************
 * meat::Grinder::Class::class_method_count *
 ********************************************/

uint8_t meat::grinder::Class::class_method_count() const {
  if (cm_count == 0) {
    const Index &methods = cast<const Index>(classMethods);
    return methods.size();
  } else
    return cm_count;
}

#undef className
#undef superClass
#undef objectProperties
#undef classProperties
#undef objectMethods
#undef classMethods
#undef constr

/******************************************************************************
 * meat::grinder::Method Class
 */

/*********************************
 * meat::grinder::Method::Method *
 *********************************/

meat::grinder::Method::Method(Reference klass, uint8_t properties)
  : Object(klass, properties), cb(NULL), _is_cpp(false) {
  this->property(0) = new List(); // Name
  this->property(1) = new List(); // Parameters
}

meat::grinder::Method::Method(Class &cb, bool is_cpp)
  : Object(meat::Class::resolve("Grinder.Method"), 3), cb(&cb),
    _is_cpp(is_cpp) {
  this->property(0) = new List(); // Name
  this->property(1) = new List(); // Parameters
}

/**********************************
 * meat::grinder::Method::compile *
 **********************************/

void meat::grinder::Method::compile() {
  if (!_is_cpp) {
    Reference super = cb->super();
    List &properties = cast<List>(cb->property(2));
    List &cls_properties = cast<List>(cb->property(3));

    ast::Method method(properties,
                       cast<meat::Class>(super).obj_properties(),
                       cls_properties, 0);
    astree = &method;

    // Add the parameters to the AST Method node.
    List &parameters = cast<List>(this->property(1));
    meat::List::iterator it;
    for (it = parameters.begin(); it != parameters.end(); it++)
      method.add_parameter(cast<Text>(*it));

    // Tokenize and send command to command() method.
    execute(cast<Text>(this->property(2)));

    method.gen_bytecode(true);  // Bytecode generation stage 1
    method.gen_bytecode(false); // Bytecode generation stage 2

    _locals = method.locals();
    method.append_bytecode(bytecode);

    astree = NULL;
  }
}

/*****************************************
 * meat::grinder::Method::update_symbols *
 *****************************************/

void
meat::grinder::Method::update_symbols(std::set<std::string> &symbols) const {
  symbols.insert(this->symbols.begin(), this->symbols.end());
}

/**************************************
 * meat::grinder::Method::cpp_hash_id *
 **************************************/

std::string meat::grinder::Method::cpp_hash_id() {
  std::string name;
  List::const_iterator cit = cast<const List>(this->property(0)).begin();
  for (; cit != cast<const List>(this->property(0)).end(); ++cit)
    name += cast<const Text>(*cit);
  return itohex(hash(name));
}

/***********************************
 * meat::grinder::Method::cpp_name *
 ***********************************/

std::string meat::grinder::Method::cpp_name(const std::string &prelim) {
  std::string orig_name;
  List::const_iterator cit = cast<const List>(this->property(0)).begin();
  for (; cit != cast<const List>(this->property(0)).end(); ++cit)
    orig_name += cast<const Text>(*cit);

  // Transform operator names
  if (orig_name == "==") orig_name = "equals";
  if (orig_name == "<>") orig_name = "nequals";
  if (orig_name == "+")  orig_name = "add";
  if (orig_name == "-")  orig_name = "sub";
  if (orig_name == "*")  orig_name = "mult";
  if (orig_name == "/")  orig_name = "div";
  if (orig_name == "%")  orig_name = "mod";
  if (orig_name == "^")  orig_name = "pow";
  if (orig_name == "<")  orig_name = "less";
  if (orig_name == "<=") orig_name = "less_equal";
  if (orig_name == ">")  orig_name = "greater";
  if (orig_name == ">=") orig_name = "greater_equal";

  // Convert all : to _
  std::string method_name = std::string(prelim) + orig_name;
  for (unsigned int c = 0; c < method_name.size(); c++) {
    if (method_name[c] == ':')
      method_name[c] = '_';
  }
  return method_name;
}

/*************************************
 * meat::grinder::Method::cpp_method *
 *************************************/

void meat::grinder::Method::cpp_method(const std::string &prelim,
                                       std::ostream &out) {
  if (_is_cpp) {
    out << "static Reference " + cpp_name(prelim) + "(Reference context) {\n"
        << "  Reference self = cast<Context>(context).self();\n"
        << "  Reference klass = cast<Context>(context).klass();\n";

    List::const_iterator it;
    int i;
    for (i = 0, it = cast<List>(this->property(1)).begin();
         it != cast<List>(this->property(1)).end();
         it++, i++) {
      std::ostringstream convert;
      convert << i;
      out << "  Reference " << cast<const Text>(*it)
          << " = cast<Context>(context).parameter(" << i
          << ");\n";
    }

    out << cast<Text>(this->property(2))
        << "}\n\n";

    _locals = cast<List>(this->property(1)).size();
  }
}

/**********************************
 * meat::grinder::Method::command *
 **********************************/

void meat::grinder::Method::command() {
  ast::Block &current = *astree;
  if (tokens.expect(Token::EOL)) {
    tokens.next();
  } else if (tokens.count() >= 2 and tokens[1] == "=") {
    current.add(assignment());
  } else {
    current.add(message());
  }
}

/****************************************
 * meat::grinder::Method::add_parameter *
 ****************************************/

void grinder::Method::add_parameter(const std::string &name) {
  List &parameters = cast<List>(this->property(1));
  parameters.push_back(new Text(name));
}

/***********************************
 * meat::grinder::Method::add_body *
 ***********************************/

void grinder::Method::add_body(const std::string &body) {
  this->property(2) = new Text(body);
}

/***************************************
 * meat::grinder::Method::gen_bytecode *
 ***************************************/

void grinder::Method::gen_bytecode(std::vector<uint8_t> &class_bc) {

#ifdef DEBUG
  std::cout << "METHODBUILDER: Bytecode size = "
            << std::dec << this->bytecode.size()
            << std::endl;
#endif /* DEBUG */

  for (uint16_t c = 0; c < this->bytecode.size(); c++)
    class_bc.push_back(this->bytecode.at(c));
}

/**********************************
 * meat::grinder::Method::message *
 **********************************/

grinder::ast::Message *grinder::Method::message() {
  meat::grinder::ast::Node *obj = NULL;
  bool super = false;

#ifdef DEBUG
  std::cout << "COMPILER: Message " << tokens.to_string() << std::flush;
#endif

  /* Determine the object that we are messaging. */
  if (tokens.expect(Token::WORD)) {
    if (tokens[0] == "super") {
      super = true;
      obj = new grinder::ast::Value("self");
    } else
      obj = new grinder::ast::Value(tokens[0]);
    tokens.next();
  } else if (tokens.expect(Token::LITRL_STRING) or
             tokens.expect(Token::SUBST_STRING)) {
    obj = text_constant();
  } else if (tokens.expect(Token::COMMAND)) {
    tokens.push();
    obj = message();
    tokens.pop();
  } else {
    throw Exception((std::string)tokens[0].position() +
                    ": Unable to determine object to message");
  }

  // Create the new message ast branch.
  ast::Message *mesg = new ast::Message(obj);

  std::string method_name;
  for (unsigned int c = 0; not tokens.expect(Token::EOL); ++c) {
    if (c % 2 == 0) {
      // Method name
      method_name += (const std::string &)(tokens[0]);
      tokens.permit(Token::WORD);
    } else {
      // Parameters
      if (tokens.expect(Token::WORD)) {
        mesg->add_param(new meat::grinder::ast::Value(tokens[0]));
        tokens.next();
      } else if (tokens.expect(Token::LITRL_STRING) or
                 tokens.expect(Token::SUBST_STRING)) {
        mesg->add_param(text_constant());
      } else if (tokens.expect(Token::COMMAND)) {
        tokens.push();
        mesg->add_param(message());
        tokens.pop();
      } else if (tokens.expect(Token::BLOCK)) {
        tokens.push();
        mesg->add_param(block());
        tokens.pop();
      } else if (tokens.expect(Token::EOL)) {
        throw Exception("Unexpected end of line in message.");
      }
    }
  }

  tokens.permit(Token::EOL);

  mesg->message_super(super);
  mesg->method(method_name);
  symbols.insert(method_name);

  return mesg;
}

/*************************************
 * meat::grinder::Method::assignment *
 *************************************/

grinder::ast::Assignment *grinder::Method::assignment() {
  ast::Identifier *dest = new ast::Identifier(tokens[0]);
  ast::Node *src = NULL;

  tokens.permit(Token::WORD);
  tokens.permit(Token::WORD, "=");

  if (tokens.expect(Token::WORD)) {
    src = new ast::Value(tokens[0]);
    tokens.next();
  } else if (tokens.expect(Token::LITRL_STRING) or
             tokens.expect(Token::SUBST_STRING)) {
    src = new ast::Value(tokens[0], true);
    tokens.next();
  } else if (tokens.expect(Token::COMMAND)) {
    tokens.push();
    src = message();
    tokens.pop();
  } else
    throw Exception(std::string("Unexpected value ") +
                    (std::string &)(tokens[0]) +
                    " for assignment");

  tokens.permit(Token::EOL);

  return new ast::Assignment(dest, src);
}

/********************************
 * meat::grinder::Method::block *
 ********************************/

grinder::ast::ContextBlock *grinder::Method::block() {
  ast::ContextBlock *new_block = new ast::ContextBlock();
  ast::Block *save = astree;
  new_block->scope(save);
  astree = new_block;

#ifdef DEBUG
  std::cout << "COMPILER: Block start " << std::endl;
#endif

  while (tokens.is_more()) {
    if (tokens.count() > 1 and tokens[1] == "=") {
      new_block->add(assignment());
    } else {
      new_block->add(message());
    }
  }
  tokens.next(); // Remove newline

#ifdef DEBUG
  std::cout << "COMPILER: Block end " << std::endl;
#endif

  astree = save;

  return new_block;
}

/****************************************
 * meat::grinder::Method::text_constant *
 ****************************************/

grinder::ast::Value *grinder::Method::text_constant() {
  if (tokens.expect(Token::LITRL_STRING) or
      tokens.expect(Token::SUBST_STRING)) {
    std::string value = (const std::string &)tokens[0];
    tokens.next();
    return new grinder::ast::Value(value, true);
  } else {
    throw Exception("Was expecting string constant");
  }
}
