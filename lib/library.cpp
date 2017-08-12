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
  meat::uint32_t hash_id;
  std::string str_hash;
	std::string str_class_hash;
  std::string flags;
  meat::uint8_t locals;
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
static std::string cook_c_name(const char *value) {
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
	DECLSPEC void *create_interpreter(const std::string &);
	DECLSPEC void exec_interpreter(void *, std::istream &);
	DECLSPEC void close_interpreter(void *);

	DECLSPEC void exec_library(meat::Reference, std::istream &);

	DECLSPEC void exec_class(meat::Reference, std::istream &);
}

void * create_interpreter(const std::string &name) {
	meat::grinder::Interpreter *interp = new meat::grinder::Interpreter();

	interp->create(name);

	return (void *)interp;
}

void exec_interpreter(void *interp, std::istream &code) {
	((meat::grinder::Interpreter *)interp)->execute(code);
}

void close_interpreter(void *interp) {
	((meat::grinder::Interpreter *)interp)->write();
	delete ((meat::grinder::Interpreter *)interp);
}

void exec_library(meat::Reference library, std::istream &code) {
	dynamic_cast<meat::grinder::Library &>(*library).execute(code);
}

void exec_class(meat::Reference klass, std::istream &code) {
	dynamic_cast<meat::grinder::Class &>(*klass).execute(code);
	dynamic_cast<meat::grinder::Class &>(*klass).create_class();
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

	uint8_t size() { return vtable.size(); }

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

/***********************************
 * meat::grinder::Library::Library *
 ***********************************/

grinder::Library::Library(Reference klass,
													uint8_t properties)
  : Object(klass, properties), library(NULL), to_cpp(false) {

	this->property(1) = new List();
	this->property(2) = new Text("");
}

/************************************
 * meat::grinder::Library::~Library *
 ************************************/

grinder::Library::~Library() throw() {
	if (library)
		data::Library::unload(library->get_name());
}

/***************************************
 * meat::grinder::Library::register_as *
 ***************************************/

void grinder::Library::register_as(const std::string &name) {
	this->property(0) = new Text(name);
	library = data::Library::create(name.c_str());
	this->property(3) = library->get_imports();
}

/*************************************
 * meat::grinder::Library::add_class *
 *************************************/

void grinder::Library::add_class(Reference klass) {
	Reference &classes = this->property(1);
	LIST(classes).push_back(klass);
	dynamic_cast<meat::grinder::Class &>(*klass).library = this;
}

/*********************************
 * meat::grinder::Library::write *
 *********************************/

void grinder::Library::write() {
  if (is_cpp()) {
    std::string cppcode;

    cppcode  = "/*************************************************************"
               "*****************\n";
    cppcode += " * Auto-generated by the Grinder Compiler Library\n";
    cppcode += " */\n\n";
    cppcode += "#include <meat.h>\n";
    cppcode += "#include <meat/datastore.h>\n";
    cppcode += data::Library::include();

    cppcode += "\n#define null (meat::Null())\n\n";

    List::const_iterator cit;

    // Create all the class methods and vtables.
    List &classes = LIST(this->property(1));
    for (cit = classes.begin(); cit != classes.end(); cit++) {
      cppcode += ((Class &)(*(*cit))).cpp_methods();
    }

		// Create symbols table
		if (!symbols.empty()) {
			cppcode += "static meat::uint8_t Symbols[] = {\n";

			std::string line = "  \"";

			std::set<std::string>::const_iterator it = symbols.begin();
			for (; it != symbols.end(); ++it) {

				if (line.length() + it->length() + 2 < 79) {
					line += *it + "\\0";
				} else {
					cppcode += line + "\"\n";
					line = std::string("  \"") + *it + "\\0";
				}
			}
			if (line.length() + 2 < 79)
				cppcode += line + "\\0\"\n";
			else
				cppcode += line + "\"\n  \"\\0\"\n";

			cppcode += "};\n";
		}

    cppcode += "\n";

    // Declare the init function using C conventions.
    cppcode += "// We need to declare init_" +
      std::string(library->get_name()) + " as a C function.\n";
    cppcode += "extern \"C\" {\n";
    cppcode += "  DECLSPEC void init_" + std::string(library->get_name()) +
      "(meat::data::Library &library);\n}\n\n";

    // Generate the init function.
    cppcode += "void init_" + std::string(library->get_name()) +
      "(meat::data::Library &library) {\n  meat::Class *cls;\n";

		// Import required libraries
		if (not (property(3) == meat::Null())) {
			cppcode += "\n  // Import required libraries.\n";

			List &imports = LIST(property(3));
			for (cit = imports.begin(); cit != imports.end(); ++cit) {
        cppcode += "  meat::data::Library::import(\"" + CONST_TEXT(*cit) +
          "\");\n";
      }
    }

    // Create all the class methods and vtables.
    for (cit = classes.begin(); cit != classes.end(); cit++) {
      cppcode += ((Class &)(*(*cit))).cpp_new_class();
			dynamic_cast<const Class &>(*(*cit)).update_symbols(symbols);
    }

		if (!symbols.empty())
			cppcode += "\n  library.set_symbols(Symbols, meat::STATIC);\n";

		cppcode += "}\n";

		// Now write the code to the file stream.
    std::ofstream cpp_file;
    cpp_file.open((std::string(library->get_name()) + ".cpp").c_str());
    if (cpp_file.is_open()) {
      cpp_file << cppcode << std::flush;
      cpp_file.close();
    }

  } else {
		library->clear_symbols();

		List &classes = LIST(this->property(1));
		List::const_iterator cit = classes.begin();
		for (; cit != classes.end(); cit++) {
			dynamic_cast<const Class &>(*(*cit)).update_symbols(symbols);
    }

		std::stringstream syms_table;
		std::set<std::string>::const_iterator it = symbols.begin();
		for (; it != symbols.end(); ++it) {
			syms_table << *it << '\0';
		}
		syms_table << '\0';
		library->set_symbols((meat::uint8_t *)syms_table.str().c_str(),
												 meat::COPY);

    library->write();
  }
}

/***************************************
 * meat::grinder::Library::unserialize *
 ***************************************/

void meat::grinder::Library::unserialize(data::Archive &store,
																				 std::istream &data_stream) {
	library = data::Library::create(TEXT(this->property(0)).c_str());

	List &imports = LIST(this->property(3));
	List::iterator iit = imports.begin();
	for (; iit != imports.end(); ++iit) {
		library->add_import(TEXT(*iit));
	}
	property(3) = library->get_imports();

	List &classes = LIST(this->property(1));
	List::iterator cit = classes.begin();
	for (; cit != classes.end(); cit++) {
		dynamic_cast<Class &>(*(*cit)).library = this;
	}
}

/***********************************
 * meat::grinder::Library::command *
 ***********************************/

void grinder::Library::command(Tokenizer &tokens) {
  Reference obj;
  std::string object, message;

  /* Determine the object to send the message to. */
  switch (tokens[0].type()) {
  case Token::WORD: {
    object = (std::string &)(tokens[0]);
    obj = meat::Class::resolve(((const std::string &)tokens[0]).c_str());
    break;
  }
  default:
    throw Exception("Syntax error: Unable to resolve class " +
      (std::string &)tokens[0]);
  }

  if (tokens.count() == 2) {
    /* This is an object message with no parameters. */

    // Get the object.
    if (tokens[0].is_type(Token::WORD)) {
      object = (std::string &)(tokens[0]);
    }

    // Get the message.
    if (tokens[1].is_type(Token::WORD)) {
      message = (std::string &)(tokens[1]);
    }

#ifdef DEBUG
    std::cout << "DEBUG: message \"" << message << "\"" << std::endl;
#endif

    // Build the context and execute it.
    context = meat::message(obj, message.c_str(), context);
    result = meat::execute(context);
    context = ((Context &)(*context)).get_messenger();

  } else if (tokens.count() > 2 && tokens.count() % 2 == 1) {
    /* A message with parameters. */

    // Build the message name.
    for (unsigned int c = 1; c < tokens.count(); c += 2) {
      message += (std::string &)(tokens[c]);
    }

#ifdef DEBUG
    std::cout << "DEBUG: message \"" << message << "\"" << std::endl;
#endif

    // Create the context for the new message.
    context = meat::message(obj, message.c_str(), context);

    // Add the parameters to the context.
    uint8_t param;
    uint32_t c;
    for (c = 2, param = 0;
         c < tokens.count();
         c += 2, param++) {
      switch (tokens[c].type()) {
      case Token::WORD:      // Simple object value parameters.
      case Token::BLOCK:
      case Token::SUBST_STRING:
      case Token::LITRL_STRING: {
        Reference text(new Text((const std::string &)(tokens[c])));
        ((Context &)(*context)).set_param(param, text);
        break;
      }
      case Token::COMMAND: { // Parameter that comes from a command result.
        Tokenizer cmd_parser;
        cmd_parser.parse(tokens[0]);
        command(cmd_parser);
        ((Context &)(*context)).set_param(param, result);
        break;
      }
      default:
        throw meat::Exception("Syntax error");
      }
    }

    // Execute the new context.
    result = meat::execute(context);
    context = ((Context &)(*context)).get_messenger();

  } else {
    throw Exception("Syntax error");
  }

  tokens.clear();
}

/**************************************
 * meat::grinder::Library::add_import *
 **************************************/

void grinder::Library::add_import(const std::string &name) {
	if (library) library->add_import(name);
}

void grinder::Library::remove_import(const std::string &name) {

}

/***************************************
 * meat::grinder::Library::get_imports *
 ***************************************/

Reference grinder::Library::get_imports() const {
	if (library) return library->get_imports();
	return new meat::List();
}

/*******************************************
 * meat::grinder::Library::set_application *
 *******************************************/

void grinder::Library::set_application(const std::string &name) {
	if (library) library->set_application(name);
}

void grinder::Library::add_symbol(const std::string &symbol) {
	symbols.insert(symbol);
}

void grinder::Library::clear_symbols() {
	symbols.clear();
}

/**********************************
 * meat::grinder::Library::is_cpp *
 **********************************/

bool grinder::Library::is_cpp() const {
  List::const_iterator mit;

  // Check the methods to see if any of them are cpp methods.
  const List &classes = CONST_LIST(this->property(1));
  for (mit = classes.begin(); mit != classes.end(); mit++) {
    if (((Class &)(*(*(mit)))).is_cpp()) {
      return true;
    }
  }
  return false;
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

meat::uint8_t
meat::grinder::Class::obj_property(const std::string &name) {
#ifdef DEBUG
  std::cout << "COMPILE: Adding property " << name << " to class."
            << std::endl;
#endif /* DEBUG */
  List &properties = LIST(objectProperties);

  for (uint8_t index = 0; index < properties.size(); index++) {
    if (TEXT(properties.at(index)) == name) return index;
  }

  properties.push_back(new Text(name));
  return properties.size() - 1;
}

/**************************************
 * meat::Grinder::Class::cls_property *
 **************************************/

uint8_t grinder::Class::cls_property(const std::string &name) {
#ifdef DEBUG
  std::cout << "COMPILE: Adding class property " << name << " to class."
            << std::endl;
#endif /* DEBUG */
  List &cls_properties = LIST(classProperties);

  for (uint8_t index = 0; index < cls_properties.size(); index++) {
    if (TEXT(cls_properties.at(index)) == name) return index;
  }

  cls_properties.push_back(new Text(name));
  return cls_properties.size() - 1;
}

/******************************************
 * meat::Grinder::Class::have_obj_propery *
 ******************************************/

int16_t
grinder::Class::have_obj_property(const std::string &name) const {

  const List &properties = CONST_LIST(objectProperties);

  for (uint8_t index = 0; index < properties.size(); index++) {
    if (CONST_TEXT(properties.at(index)) == name) return index;
  }
  return -1;
}

/*******************************************
 * meat::Grinder::Class::have_cls_property *
 *******************************************/

int16_t grinder::Class::have_cls_property(const std::string &name)
  const {
  const List &cls_properties = CONST_LIST(classProperties);

  for (uint8_t index = 0; index < cls_properties.size(); index++) {
    if (CONST_TEXT(cls_properties.at(index)) == name) return index;
  }
  return -1;
}

/************************************
 * meat::Grinder::Class::add_method *
 ************************************/

void grinder::Class::add_method(Reference method) {
	INDEX(objectMethods)[dynamic_cast<Method &>(*method).property(0)] = method;
	dynamic_cast<Method &>(*method).cb = this;
}

/******************************************
 * meat::Grinder::Class::add_class_method *
 ******************************************/

void grinder::Class::add_class_method(Reference method) {
	INDEX(classMethods)[dynamic_cast<Method &>(*method).property(0)] = method;
	dynamic_cast<Method &>(*method).cb = this;
}

/**************************************
 * meat::Grinder::Class::create_class *
 **************************************/

void grinder::Class::create_class() const {

#ifdef DEBUG
  std::cout << "COMPILER: Creating class " << TEXT(className) << std::endl;
#endif /* DEBUG */

  const List &properties = CONST_LIST(objectProperties);
  const List &cls_properties = CONST_LIST(classProperties);

	Reference super = meat::Class::resolve(CONST_TEXT(superClass));
	//Reference super = superClass;
  meat::Class *cls =
    new meat::Class(super,
										(uint8_t)cls_properties.size(),
										(uint8_t)properties.size() +
										CONST_CLASS(super).get_obj_properties());

  std::vector<uint8_t> bytecode;

	uint8_t vt_size;
	const vtable_entry_t *vt;
  Index::const_iterator mit;
	VtableBuilder<struct _vtable_entry_s> vtable;
	VtableBuilder<struct _vtable_entry_s>::iterator vtable_it;

  /* Generate the bytecode for the object methods and record it in the
   * virtual table.
   */
	vt = CONST_CLASS(super).get_vtable(vt_size);
	vtable.set(vt, vt_size);
	for (vtable_it = vtable.begin(); vtable_it != vtable.end(); ++vtable_it)
		vtable_it->flags = VTM_SUPER;
  const Index &methods = CONST_INDEX(objectMethods);
  for (mit = methods.begin(); mit != methods.end(); mit++) {
    struct _vtable_entry_s entry;
    uint16_t offset = bytecode.size();
    ((Method &)(*(mit->second))).gen_bytecode(bytecode);
    entry.hash_id = hash(CONST_TEXT(mit->first));
		entry.class_id = hash(CONST_TEXT(className));
    entry.flags = VTM_BYTECODE;
    entry.locals = ((Method &)(*(mit->second))).local_count();
    entry.method.offset = offset;
#ifdef DEBUG
		std::cout << "VTABLE: method " << CONST_TEXT(mit->first)
							<< " bytecode @ " << offset << std::endl;
#endif
		vtable.add_entry(entry);
		library->add_symbol(CONST_TEXT(mit->first));
  }
	std::sort(vtable.begin(), vtable.end(), vtable_comp);
  cls->set_vtable(vtable.size(), vtable.entries(), COPY);
  vtable.clear();

  /* Generate the bytecode for the class methods and record it in the class
   * virtual table.
   */
	vt = CONST_CLASS(super).get_class_vtable(vt_size);
	vtable.set(vt, vt_size);
	for (vtable_it = vtable.begin(); vtable_it != vtable.end(); ++vtable_it)
		vtable_it->flags = VTM_SUPER;
  const Index &class_methods = CONST_INDEX(classMethods);
  for (mit = class_methods.begin(); mit != class_methods.end(); mit++) {
    struct _vtable_entry_s entry;
    uint16_t offset = bytecode.size();
    ((Method &)(*(mit->second))).gen_bytecode(bytecode);
    entry.hash_id = hash(CONST_TEXT(mit->first));
		entry.class_id = hash(CONST_TEXT(className));
    entry.flags = VTM_BYTECODE;
    entry.locals = ((Method &)(*(mit->second))).local_count();
    entry.method.offset = offset;
#ifdef DEBUG
		std::cout << "VTABLE: class method " << CONST_TEXT(mit->first)
							<< " bytecode @ " << offset << std::endl;
#endif
    vtable.add_entry(entry);
		library->add_symbol(CONST_TEXT(mit->first));
  }
	std::sort(vtable.begin(), vtable.end(), vtable_comp);
  cls->set_class_vtable(vtable.size(), vtable.entries(), COPY);
  vtable.clear();

  // Add the bytecode to the class.
  cls->set_bytecode(bytecode.size(), (uint8_t *)&bytecode[0], COPY);

  // Register the new class and add it to the library.
  library->library->add(cls, CONST_TEXT(className).c_str());
	library->add_symbol(CONST_TEXT(className));
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
  const Index &methods = CONST_INDEX(objectMethods);
  for (mit = methods.begin(); mit != methods.end(); mit++) {
    if (((Method &)(*(mit->second))).is_cpp()) {
      return true;
    }
  }

  // Check the class methods to see if any of them are cpp methods.
  const Index &class_methods = CONST_INDEX(classMethods);
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

std::string meat::grinder::Class::cpp_methods() {
  std::string cppcode;
  std::string class_name = cook_c_name(TEXT(className).c_str());
  std::vector<uint8_t> bytecode;

  cppcode += "/***************************************************************"
    "***************\n";
  cppcode += std::string(" * ") + TEXT(className) + " Class\n";
  cppcode += " */\n\n";

  if (!(constr == meat::Null())) {
      cppcode += "static meat::Reference " + class_name + "_constructor(\n" +
        "  meat::Reference &klass,\n  meat::uint8_t properties) {\n";
      cppcode += TEXT(constr);
      cppcode += "\n}\n\n";
  }

  List::const_iterator pit;
  unsigned int c;
  List &obj_props = LIST(objectProperties);
  List &cls_props = LIST(classProperties);

  if (obj_props.size() > 0) {
    for (pit = obj_props.begin(), c = 0; pit != obj_props.end(); pit++, c++) {
        cppcode += std::string("#define ") + CONST_TEXT(*pit) +
          " (self->property(" + ::to_string(c) + "))\n";
    }
    cppcode += "\n";
  }
  if (cls_props.size() > 0) {
    for (pit = cls_props.begin(), c = 0; pit != cls_props.end(); pit++, c++) {
        cppcode += std::string("#define ") + CONST_TEXT(*pit) +
          " (klass->property(" + ::to_string(c) + "))\n";
    }
    cppcode += "\n";
  }

	uint8_t vt_size;
	const vtable_entry_t *vt;
	VtableBuilder<struct _c_vtable_entry_s> vtable;
	VtableBuilder<struct _c_vtable_entry_s>::iterator vtable_it;

  /* Methods */
	Reference super = meat::Class::resolve(CONST_TEXT(superClass));

	vt = CLASS(super).get_vtable(vt_size);
	vtable.set(vt, vt_size);
	for (vtable_it = vtable.begin(); vtable_it != vtable.end(); ++vtable_it) {
		vtable_it->str_hash = itohex(vtable_it->hash_id);
		vtable_it->str_class_hash = "0x00000000";
		vtable_it->flags = "VTM_SUPER   ";
		vtable_it->func_name = "{.offset = 0}";
	}
  if (method_count() > 0) {
    Index &methods = INDEX(objectMethods);
    //std::vector<struct _c_vtable_entry_s> vtable;

    Index::const_iterator mit;
    for (mit = methods.begin(); mit != methods.end(); mit++) {
      meat::uint32_t hash_id = hash(CONST_TEXT(mit->first));
      struct _c_vtable_entry_s entry;

      if (((Method &)(*(mit->second))).is_cpp()) {
        // Build the virtual table entry.
        entry.hash_id = hash_id;
        entry.str_hash = ((Method &)(*(mit->second))).cpp_hash_id();
				entry.str_class_hash = cpp_hash_id();
        entry.flags = "VTM_NATIVE  ";
        entry.func_name =
          ((Method &)(*(mit->second))).cpp_name((class_name +
            "_om_").c_str());

        cppcode += "// method " + CONST_TEXT(mit->first) + "\n";
        cppcode +=
          ((Method &)(*(mit->second))).cpp_method((class_name +
            "_om_").c_str());

        entry.locals = ((Method &)(*(mit->second))).local_count();

        vtable.add_entry(entry);
      } else {
        // Build the virtual table entry.
        entry.hash_id = hash_id;
        entry.str_hash = ((Method &)(*(mit->second))).cpp_hash_id();
				entry.str_class_hash = cpp_hash_id();
        entry.flags = "VTM_BYTECODE";
        entry.func_name = "{.offset = " + ::to_string(bytecode.size()) + "}";
        ((Method &)(*(mit->second))).gen_bytecode(bytecode);
        entry.locals = ((Method &)(*(mit->second))).local_count();
        vtable.add_entry(entry);
      }
    }

    // Sort the virtual table according to the hash ids.
    std::sort(vtable.begin(), vtable.end(), c_vtable_comp);
		m_count = vtable.size();

    cppcode += "static meat::vtable_entry_t " +
      class_name +
      "Methods[] = {\n";

    bool first = true;
    std::vector<struct _c_vtable_entry_s>::const_iterator it;
    for (it = vtable.begin(); it != vtable.end(); it++) {
      if (!first) cppcode += ",\n";
      else first = false;

      cppcode += "  {" + it->str_hash + ", " + it->str_class_hash +
				", " + it->flags + ", " + ::to_string(it->locals) + ", " +
        it->func_name + "}"; // XXX Need to deal with the comma.
    }
    cppcode += "\n";

    cppcode += "};\n\n";
  }

	if (obj_props.size() > 0) {
    for (pit = obj_props.begin(); pit != obj_props.end(); pit++) {
      cppcode += std::string("#undef ") + CONST_TEXT(*pit) + "\n";
    }
    cppcode += "\n";
  }

  /* Class methods */
	vt = CLASS(super).get_class_vtable(vt_size);
	vtable.set(vt, vt_size);
	for (vtable_it = vtable.begin(); vtable_it != vtable.end(); ++vtable_it) {
		vtable_it->str_hash = itohex(vtable_it->hash_id);
		vtable_it->str_class_hash = "0x00000000";
		vtable_it->flags = "VTM_SUPER   ";
		vtable_it->func_name = "{.offset = 0}";
	}
  if (class_method_count() > 0) {
    Index &methods = INDEX(classMethods);

    Index::const_iterator mit;
    for (mit = methods.begin(); mit != methods.end(); mit++) {
      meat::uint32_t hash_id = hash(CONST_TEXT(mit->first));
      struct _c_vtable_entry_s entry;

      if (((Method &)(*(mit->second))).is_cpp()) {
        // Build the virtual table entry.
        entry.hash_id = hash_id;
        entry.str_hash = ((Method &)(*(mit->second))).cpp_hash_id();
				entry.str_class_hash = cpp_hash_id();
        entry.flags = "VTM_NATIVE  ";
        entry.func_name = ((Method &)(*(mit->second))).cpp_name((class_name + "_cm_").c_str());

        cppcode += "// class method " + CONST_TEXT(mit->first) + "\n";
        cppcode += ((Method &)(*(mit->second))).cpp_method((class_name + "_cm_").c_str());

        entry.locals = ((Method &)(*(mit->second))).local_count();

        vtable.add_entry(entry);
      } else {
        // Build the virtual table entry.
        entry.hash_id = hash_id;
        entry.str_hash = ((Method &)(*(mit->second))).cpp_hash_id();
				entry.str_class_hash = cpp_hash_id();
        entry.flags = "VTM_BYTECODE";
        entry.func_name = "{.offset = " + ::to_string(bytecode.size()) + "}";
        ((Method &)(*(mit->second))).gen_bytecode(bytecode);
        entry.locals = ((Method &)(*(mit->second))).local_count();
        vtable.add_entry(entry);
      }
    }

    // Sort the virtual table according to the hash ids.
    std::sort(vtable.begin(), vtable.end(), c_vtable_comp);
		cm_count = vtable.size();

    cppcode += "static meat::vtable_entry_t " +
      class_name +
      "CMethods[] = {\n";

    bool first = true;
    std::vector<struct _c_vtable_entry_s>::const_iterator it;
    for (it = vtable.begin(); it != vtable.end(); it++) {
      if (!first) cppcode += ",\n";
      else first = false;

      cppcode += "  {" + it->str_hash + ", " + it->str_class_hash +
				", " + it->flags + ", " + ::to_string(it->locals) + ", " +
        it->func_name + "}"; // XXX Need to deal with the comma.
    }
    cppcode += "\n";

    cppcode += "};\n\n";
  }

  if (cls_props.size() > 0) {
    for (pit = cls_props.begin(); pit != cls_props.end(); pit++) {
      cppcode += std::string("#undef ") + CONST_TEXT(*pit) + "\n";
    }
    cppcode += "\n";
  }

  /* Bytecode generation.
   */
  cpp_bytecode = bytecode.size();
  if (!bytecode.empty()) {
    cppcode += "static meat::uint8_t " + class_name + "Bytecode[] = {\n  ";
    bool first = true;
    for (unsigned int c = 0; c < bytecode.size(); c++) {
      if (!first) {
        if (c % 13 == 0)
          cppcode += ",\n  ";
        else
          cppcode += ", ";
      } else first = false;

      cppcode += itohex(bytecode[c], 2);
    }
    cppcode += "\n};\n\n";
  }

  return cppcode;
}

/***************************************
 * meat::grinder::Class::cpp_new_class *
 ***************************************/

std::string meat::grinder::Class::cpp_new_class() const {
  std::string cppcode;
  std::string class_name = CONST_TEXT(className);
  std::string cooked_name = cook_c_name(class_name.c_str());
  std::ostringstream hex;

	Reference super = meat::Class::resolve(CONST_TEXT(superClass));

  hex << std::hex << std::showbase << std::setw(8)
      << std::setfill('0');
  hex << CONST_CLASS(super).get_hash_id();

  cppcode += "\n  cls = new meat::Class(meat::Class::resolve(" + hex.str() +
    "), " + ::to_string(CONST_LIST(objectProperties).size()) + ", " +
    ::to_string(CONST_LIST(classProperties).size() +
								CONST_CLASS(super).get_obj_properties()) + ");\n";

  if (!(constr == meat::Null())) {
      cppcode += "  cls->set_constructor(" + cooked_name + "_constructor);\n";
  }

  if (method_count() > 0)
    cppcode += "  cls->set_vtable(" + ::to_string(method_count()) +
      ", " + cooked_name + "Methods, meat::STATIC);\n";

  if (class_method_count() > 0)
    cppcode += "  cls->set_class_vtable(" + ::to_string(class_method_count()) +
      ", " + cooked_name + "CMethods, meat::STATIC);\n";

  if (cpp_bytecode > 0)
    cppcode += "  cls->set_bytecode(" + ::to_string(cpp_bytecode) +
      ", " + cooked_name + "Bytecode, meat::STATIC);\n";

  cppcode += std::string("  library.add(cls, \"") + class_name + "\");\n";

  return cppcode;
}

/*************************************
 * meat::grinder::Class::cpp_hash_id *
 *************************************/

std::string meat::grinder::Class::cpp_hash_id() const {
	return itohex(hash(CONST_TEXT(this->property(0))));
}

/*********************************
 * meat::grinder::Class::command *
 *********************************/

void meat::grinder::Class::command(Tokenizer &tokens) {

  if (tokens[0].type() == Token::WORD) {

    /* Class definitions */
    if (tokens[0] == "class") {

      if (tokens[1].type() == Token::WORD) {

        if (tokens[1] == "property") {
          cls_property(tokens[2]);

        } else if (tokens[1] == "method" || tokens[1] == "function") {

          uint8_t body_index = tokens.count() - 1;
          std::string method_name;

          // Build the method name
          for (int c = 2; c < body_index; c += 2) {
            method_name += (std::string &)(tokens[c]);
          }

#ifdef DEBUG
          std::cout << "COMPILE: class " << tokens[1] << " " << method_name
                    << std::endl;
#endif /* DEBUG */

          Method *mb = new Method(*this,
                                                (tokens[1] == "function"));

          Reference name = new Text(method_name);
          INDEX(classMethods)[name] = mb;
          mb->property(0) = name;

          for (int c = 3; c < body_index; c += 2) {
            mb->add_parameter(tokens[c]);
          }

          mb->add_body(tokens[tokens.count() - 1]);
          //if (tokens[1] == "method")
          mb->compile();

        } else {
          throw Exception(
            "Expected class property, method or function keyword got class " +
            (std::string &)tokens[1]);
        }
      } else {
        throw Exception("Expected property or method keyword.");
      }

    } else {

      /* Object definitions. */
      if (tokens[0].type() == Token::WORD) {
        if (tokens[0] == "property") {
          obj_property(tokens[1]);

        } else if (tokens[0] == "constructor") {
          constr = new Text((std::string &)tokens[1]);

        } else if (tokens[0] == "method" || tokens[0] == "function") {

          uint8_t body_index = tokens.count() - 1;
          std::string method_name;

          // Build the method name
          for (int c = 1; c < body_index; c += 2) {
            method_name += (std::string &)(tokens[c]);
          }

#ifdef DEBUG
          std::cout << "COMPILE: " << tokens[0] << " " << method_name << " as "
                    << std::hex << (unsigned int)hash(method_name.c_str())
                    << std::endl;
#endif /* DEBUG */

          // Create a new method builder and add it to the vtable mappings.
          Method *mb = new Method(*this,
                                                (tokens[0] == "function"));
          Reference name = new Text(method_name);
          INDEX(objectMethods)[name] = mb;
          mb->property(0) = name;

          // Add the method parameters to the method build..
          for (int c = 2; c < body_index; c += 2) {
            mb->add_parameter(tokens[c]);
          }

          // Add the body to the method builder and compile the method.
          mb->add_body(tokens[tokens.count() - 1]);
          //if (tokens[0] == "method")
          mb->compile();

        } else {
          throw Exception(
            "Expected property, method, function or constructor keyword got " +
            (std::string &)tokens[0]);
        }
      } else {
        throw Exception("Expected property or method keyword.");
      }

    }
  } else {
    throw Exception("Expected class, property, method or function keyword.");
  }

  tokens.clear();
}

/*********************************************
 * meat::grinder::Class::update_symbol_table *
 *********************************************/

void
meat::grinder::Class::update_symbols(std::set<std::string> &symbols) const {
	const Index &methods = CONST_INDEX(objectMethods);

	meat::Index::const_iterator it;
	for (it = methods.begin(); it != methods.end(); ++it) {
		dynamic_cast<const Method &>(*(it->second)).update_symbols(symbols);
	}

	const Index cmethods = CONST_INDEX(classMethods);
	for (it = cmethods.begin(); it != cmethods.end(); ++it) {
		dynamic_cast<const Method &>(*(it->second)).update_symbols(symbols);
	}
}

/*************************************
 * meat::grinder::Class::unserialize *
 *************************************/

void meat::grinder::Class::unserialize(data::Archive &store,
																			 std::istream &data_stream) {
	Index &methods = INDEX(objectMethods);

	meat::Index::iterator it;
	for (it = methods.begin(); it != methods.end(); ++it) {
		dynamic_cast<Method &>(*(it->second)).cb = this;
	}

	Index cmethods = INDEX(classMethods);
	for (it = cmethods.begin(); it != cmethods.end(); ++it) {
		dynamic_cast<Method &>(*(it->second)).cb = this;
	}
}

/**************************************
 * meat::grinder::Class::method_count *
 **************************************/

uint8_t meat::grinder::Class::method_count() const {
	if (m_count == 0) {
		const Index &methods = CONST_INDEX(objectMethods);
		return methods.size();
	} else
		return m_count;
}

/********************************************
 * meat::Grinder::Class::class_method_count *
 ********************************************/

uint8_t meat::grinder::Class::class_method_count() const {
	if (cm_count == 0) {
		const Index &methods = CONST_INDEX(classMethods);
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
 * Method Class Implementation
 */

/*********************************
 * meat::grinder::Method::Method *
 *********************************/

meat::grinder::Method::Method(Reference klass, uint8_t properties)
	: Object(klass, properties), cb(NULL), _is_cpp(false) {
	this->property(1) = new List();    // Parameters
};

meat::grinder::Method::Method(Class &cb, bool is_cpp)
  : Object(meat::Class::resolve("Grinder.Method"), 3), cb(&cb),
		_is_cpp(is_cpp) {
  //this->property(0) = meat::Null(); // Method name
  this->property(1) = new List();    // Parameters
  //this->property(2) = meat::Null(); // Code
};

/**********************************
 * meat::grinder::Method::compile *
 **********************************/

void meat::grinder::Method::compile() {
  if (!_is_cpp) {
    Reference super = cb->get_super();
    List &properties = LIST(cb->property(2));
    List &cls_properties = LIST(cb->property(3));

    meat::grinder::ast::Method method(properties,
																			CLASS(super).get_obj_properties(),
																			cls_properties, 0);
    astree = (void *)&method;

    // Add the parameters to the AST Method node.
    List &parameters = LIST(this->property(1));
    meat::List::iterator it;
    for (it = parameters.begin(); it != parameters.end(); it++)
      method.add_parameter(TEXT(*it));

    // Tokenize and send command to command() method.
    execute(TEXT(this->property(2)));

    method.gen_bytecode(true);  // Bytecode generation stage 1
    method.gen_bytecode(false); // Bytecode generation stage 2

    locals = method.locals();
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
	return itohex(hash(CONST_TEXT(this->property(0))));
}

/***********************************
 * meat::grinder::Method::cpp_name *
 ***********************************/

std::string meat::grinder::Method::cpp_name(const char *prelim) {
  std::string orig_name = TEXT(this->property(0));

  // Transform operator names
  if (orig_name == "==") orig_name = "equals";
  if (orig_name == "!=") orig_name = "nequals";
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

std::string meat::grinder::Method::cpp_method(const char *prelim) {
  if (_is_cpp) {
    std::string code;

    code += "static meat::Reference " + cpp_name(prelim) +
      "(meat::Reference context) {\n";

    code += "  meat::Reference self = CONTEXT(context).get_self();\n";
    code += "  meat::Reference klass = CONTEXT(context).get_class();\n";

    List::const_iterator it;
    int i;
    for (i = 0, it = LIST(this->property(1)).begin();
         it != LIST(this->property(1)).end();
         it++, i++) {
      std::ostringstream convert;
      convert << i;
      code += std::string("  meat::Reference ") + CONST_TEXT(*it) +
        " = CONTEXT(context).get_param(" + convert.str() + ");\n";
    }

    code += TEXT(this->property(2));
    code += "}\n\n";

    locals = LIST(this->property(1)).size();

    return code;
  }

  return "";
}

/**********************************
 * meat::grinder::Method::command *
 **********************************/

void meat::grinder::Method::command(Tokenizer &tokens) {
  ast::ASTNode &current = *((ast::ASTNode *)astree);

  if (tokens[1] == "=") {

#ifdef DEBUG
    std::cout << "COMPILER: Assignment " << tokens.to_string() << std::endl;
#endif

    meat::grinder::ast::Value *dest =
      new meat::grinder::ast::Value(tokens[0]);
    meat::grinder::ast::ASTNode *src = NULL;

    switch (tokens[2].type()) {
    case Token::WORD:
      src = new meat::grinder::ast::Value(tokens[2]);
      break;

    case Token::LITRL_STRING:
    case Token::SUBST_STRING:
      src = new meat::grinder::ast::Value(tokens[2], true);
      break;

    case Token::COMMAND: {
      Tokenizer command_tokens;
      command_tokens.parse(tokens[2]);

      src = (meat::grinder::ast::ASTNode *)parse_message(command_tokens);
      break;
    }
    case Token::BLOCK:
      throw Exception("Unable to assign blocks to a variable.");
		case Token::TEOF:
			throw Exception("Unexpected end of file in assignment.");
    }

    ast::Assignment *assign = new ast::Assignment(dest, src);
    current.add(assign);

  } else {
    meat::grinder::ast::Message *mesg =
      (meat::grinder::ast::Message *)parse_message(tokens);

    current.add(mesg);
  }

  tokens.clear();
}

/****************************************
 * meat::grinder::Method::add_parameter *
 ****************************************/

void grinder::Method::add_parameter(const std::string &name) {
  List &parameters = LIST(this->property(1));
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

/****************************************
 * meat::grinder::Method::parse_message *
 ****************************************/

void *grinder::Method::parse_message(Tokenizer &tokens) {
  meat::grinder::ast::ASTNode *obj = NULL;
  std::string method_name;
  bool super = false;

#ifdef DEBUG
  std::cout << "COMPILER: Message " << tokens.to_string() << std::endl;
#endif

  /* Determine the object that we are messaging. */
  switch (tokens[0].type()) {
  case Token::WORD:
    if (tokens[0] == "super") {
      super = true;
      obj = new meat::grinder::ast::Value("self");
    } else
      obj = new meat::grinder::ast::Value(tokens[0]);
    break;
  case Token::LITRL_STRING: // Messaging a constant string object.
  case Token::SUBST_STRING:
    obj = new meat::grinder::ast::Value(tokens[0], true);
    break;
  case Token::COMMAND: {
    Tokenizer command_tokens;
    command_tokens.parse(tokens[0]);

    obj = (meat::grinder::ast::ASTNode *)parse_message(command_tokens);
    break;
  }
  case Token::BLOCK:
    throw Exception("Messages cannot be sent directly to a block");
	case Token::TEOF:
		throw Exception("Unexpected end of file in message.");
  }

  /*  Grab every second word from the message to build the method name
   * from.
   */
  for (unsigned int c = 1; c < tokens.count(); c += 2) {
    method_name += (std::string &)(tokens[c]);
  }

  // Create the new message ast branch.
  meat::grinder::ast::Message *mesg =
    new meat::grinder::ast::Message(obj, method_name);
  mesg->message_super(super);
	symbols.insert(method_name);

  /* Now add the parameters to the message ast branch.
   */
  if (tokens.count() > 2) {
    for (unsigned int c = 2; c < tokens.count(); c += 2) {
      switch (tokens[c].type()) {
      case Token::WORD:
        mesg->add_param(new meat::grinder::ast::Value(tokens[c]));
        break;
      case Token::SUBST_STRING:
      case Token::LITRL_STRING:
        mesg->add_param(new meat::grinder::ast::Value(tokens[c], true));
        break;
      case Token::BLOCK: {
        ast::Block *new_block = new ast::Block();
        void *save = astree;
        new_block->set_parent_block((ast::Block *)save);

        mesg->add_param(new_block);

        astree = (void *)new_block;
        Language::execute(tokens[c]);
        astree = save;

        break;
      }
      case Token::COMMAND: {
        Tokenizer command_tokens;
        command_tokens.parse(tokens[c]);

        meat::grinder::ast::Message *mesg_param =
          (grinder::ast::Message *)parse_message(command_tokens);
        mesg->add_param(mesg_param);

        break;
      }
			case Token::TEOF:
				throw Exception("Unexpected end of file.");
      }
    }
  }

  return mesg;
}
