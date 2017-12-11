/*                                                                  -*- c++ -*-
 * Compilers abstract syntax tree implementation.
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
#include <meat/bytecode.h>
#include "compiler.h"
#include "ast.h"
#include <sstream>
#include <cmath>

#ifdef TESTING
#include <testing.h>
#endif

using namespace meat::grinder::ast;

/******************************************************************************
 * meat::grinder::ast::LocalVariable Class
 */

/****************************************************
 * meat::grinder::ast::LocalVariable::LocalVariable *
 ****************************************************/

LocalVariable::LocalVariable()
  : _name(), block(NULL) {
}

LocalVariable::LocalVariable(const std::string &name, Block &block_node)
  : _name(name), block(&block_node) {
}

LocalVariable::LocalVariable(const LocalVariable &other)
  : _name(other._name), block(other.block) {
}

/*************************************************
 * meat::grinder::ast::LocalVariable::operator = *
 *************************************************/

LocalVariable &LocalVariable::operator =(const LocalVariable &other) {
  if (this != &other) {
    _name = other._name;
    block = other.block;
  }
  return *this;
}

/********************************************
 * meat::grinder::ast::LocalVariable::index *
 ********************************************/

std::uint8_t LocalVariable::index() const {
  if (block) {
    for (unsigned int idx = 0; idx < block->_locals.size(); ++idx) {
      if (block->_locals[idx] == _name)
        return (block->locals() - block->_locals.size()) + idx;
    }
  }

  throw Exception(std::string("Internal AST Error: Undefined local variable ")
                  + _name + " reference.");
}

/******************************************************************************
 * Node Class
 */

/**********************************
 * meat::grinder::ast::Node::Node *
 **********************************/

Node::Node() : _scope(NULL), result_set(false) {
}

/***********************************
 * meat::grinder::ast::Node::~Node *
 ***********************************/

Node::~Node() throw() {
}

/***********************************
 * meat::grinder::ast::Node::scope *
 ***********************************/

Block &Node::scope() const {
  if (not _scope)
    throw Exception("Internal AST Error: Node without a scope block.");
  return *_scope;
}

void Node::scope(Block *block) {
  _scope = block;
}

std::uint8_t Node::locals() const {
  return scope().locals();
}

/***********************************
 * meat::grinder::ast::Node::local *
 ***********************************/

LocalVariable Node::local(const std::string &name) {
  return scope().local(name);
}

LocalVariable Node::anon_local() {
  return scope().anon_local();
}

void Node::set_result_dest(LocalVariable dest) {
  result = dest;
  result_set = true;
}

void Node::set_result_dest() {
  if (not result_set) {
    result = anon_local();
    result_set = true;
  }
}

LocalVariable Node::get_result_dest() {
  return result;
}

std::int16_t Node::resolve_property(const std::string &name) const {
  return scope().resolve_property(name);
}

std::int16_t Node::resolve_class_property(const std::string &name) const {
  return scope().resolve_class_property(name);
}

LocalVariable Node::resolve_local(const std::string &name) const {
  return scope().resolve_local(name);
}

/*************************************
 * meat::grinder::ast::Node::bytcode *
 *************************************/

std::uint16_t Node::bytecode() {
  if (_scope) return _scope->bytecode();
  else throw Exception("Bytecode marker wanted with no destination.");
}

void Node::bytecode(std::uint8_t value) {
  if (_scope) _scope->bytecode(value);
  else throw Exception("Bytecode generation with no destination.");
}

void Node::bytecode(std::uint16_t value) {
  if (_scope) _scope->bytecode(value);
  else throw Exception("Bytecode generation with no destination.");
}

void Node::bytecode(std::uint16_t marker, uint16_t value) {
  if (_scope) _scope->bytecode(marker, value);
  else throw Exception("Bytecode generation with no destination.");
}

void Node::bytecode(std::uint32_t value) {
  if (_scope) _scope->bytecode(value);
  else throw Exception("Bytecode generation with no destination.");
}

void Node::bytecode(int32_t value) {
  if (_scope) _scope->bytecode(value);
  else throw Exception("Bytecode generation with no destination.");
}

void Node::bytecode(double value) {
  if (_scope) _scope->bytecode(value);
  else throw Exception("Bytecode generation with no destination.");
}

/******************************************************************************
 * meat::grinder::ast::Block Class
 */

Block::Block() : temp_counter(0) {
}

Block::~Block() noexcept {
  std::deque<Node *>::iterator it;
  for (it = _nodes.begin(); it != _nodes.end(); it++) {
    delete *it;
  }
}

/**********************************
 * meat::grinder::ast::Block::add *
 **********************************/

void Block::add(Node *node) {
  _nodes.push_back(node);
  node->scope(this);
}

/*************************************
 * meat::grinder::ast::Block::locals *
 *************************************/

std::uint8_t Block::locals() const {
  if (_scope) return _scope->locals() + _locals.size();
  return _locals.size();
}

/************************************
 * meat::grinder::ast::Block::local *
 ************************************/

LocalVariable Block::local(const std::string &name) {
  LocalVariable result(name, *this);

  for (unsigned int index = 0; index < _locals.size(); index++) {
    if (_locals[index] == name) return result;
  }

#ifdef DEBUG
  std::cout << "AST: Adding local " << name << std::endl;
#endif
  if (locals() < 0xff)
    _locals.push_back(name);
  else
    throw Exception("To many local variables created in a method");

  return result;
}

/*****************************************
 * meat::grinder::ast::Block::anon_local *
 *****************************************/

LocalVariable Block::anon_local() {
  std::stringstream temp_name;
  temp_name << ".anon." << (unsigned int)++temp_counter;
  return local(temp_name.str());
}

/******************************************************************************
 * meat::grinder::ast::Method Class
 */

Method::Method(const meat::List &properties, int p_offset,
               const meat::List &class_properties, int cp_offset) {

  List::const_iterator it;
  for (it = properties.begin(); it != properties.end(); it++)
    this->properties.push_back(meat::cast<const meat::Text>(*it));
  this->p_offset = p_offset;

  for (it = class_properties.begin(); it != class_properties.end(); it++)
    this->class_properties.push_back(meat::cast<const meat::Text>(*it));
  this->cp_offset = cp_offset;

  local("self");
  local("class");
  local("context");
  local("null");
}

Method::~Method() throw() {
}

std::uint8_t Method::add_parameter(const std::string &name) {
#ifdef DEBUG
  std::cout << "STAGE: Adding parameter " << name << std::endl;
#endif
  local(name);
  return _locals.size() - 1;
}

void Method::gen_bytecode(bool prelim) {

  std::deque<Node *>::iterator it;
  for (it = _nodes.begin(); it != _nodes.end(); it++) {
    (*it)->gen_bytecode(prelim);
    temp_counter = 0; // We can reuse temporary variable previously created.
  }

  if (!prelim) {
#ifdef DEBUG
    std::cout << "BC: CTXEND" << std::endl;
    std::cout << "CONTEXT: " << _locals.size() << " locals" << std::endl;
#endif
    bytecode(meat::bytecode::CONTEXT_END);
  }
}

LocalVariable Method::gen_result(bool prelim) {
  throw Exception("A method cannot be placed in a local variable.");
}

void Method::append_bytecode(std::vector<uint8_t> &bc) {
  std::vector<uint8_t>::iterator it;
  for (it = this->bc.begin(); it != this->bc.end(); it++) {
    bc.push_back(*it);
  }
}

std::int16_t Method::resolve_property(const std::string &name) const {
  for (unsigned int index = 0; index < properties.size(); index++) {
    if (properties[index] == name) return index + p_offset;
  }
  return -1;
}

std::int16_t Method::resolve_class_property(const std::string &name) const {
  for (unsigned int index = 0; index < class_properties.size(); index++) {
    if (class_properties[index] == name) return index + cp_offset;
  }
  return -1;
}

LocalVariable Method::resolve_local(const std::string &name) const {
  LocalVariable bad_result;
  LocalVariable good_result(name, (Method &)*this);

  for (unsigned int index = 0; index < _locals.size(); index++) {
    if (_locals[index] == name) return good_result;
  }

  return bad_result;
}

std::uint16_t Method::bytecode() {
  return bc.size();
}

void Method::bytecode(uint8_t value) {
  bc.push_back(value);
}

void Method::bytecode(uint16_t value) {
  bc.push_back(0); // Add two bytes to the bytecode
  bc.push_back(0);

  // Now set the new value to the bytecode
  uint16_t *iptr = (uint16_t *)&bc[bc.size() - 2];
  *iptr = endian::write_be(value);
}

void Method::bytecode(uint16_t marker, uint16_t value) {
  // Set the new value to the bytecode
  uint16_t *iptr = (uint16_t *)&bc[marker];
  *iptr = endian::write_be(value);
}

void Method::bytecode(uint32_t value) {
  bc.push_back(0); // Add four bytes to the bytecode
  bc.push_back(0);
  bc.push_back(0);
  bc.push_back(0);

  // Now set the new value to the bytecode
  int32_t *iptr = (int32_t *)&bc[bc.size() - 4];
  *iptr = endian::write_be(value);
}

void Method::bytecode(int32_t value) {
  bc.push_back(0); // Add four bytes to the bytecode
  bc.push_back(0);
  bc.push_back(0);
  bc.push_back(0);

  // Now set the new value to the bytecode
  uint32_t *iptr = (uint32_t *)&bc[bc.size() - 4];
  *iptr = endian::write_be(value);
}

void Method::bytecode(double value) {
  for (unsigned int c = 0; c < sizeof(std::int64_t) + sizeof(std::int32_t); c++)
    bc.push_back(0);

  // Now set the new value to the bytecode
  std::int64_t *mi = (std::int64_t *)&bc[bc.size() -
                                         (sizeof(std::int64_t) +
                                          sizeof(std::int32_t))];
  std::int32_t *exponent =  (std::int32_t *)&bc[bc.size() -
                                                sizeof(std::int32_t)];

  // Seperate the float point value into mantissa and exponent.
  double mantissa = frexp(value, exponent);
  *mi = trunc(ldexp(mantissa, 53)); // Mantissa to integer value.

  // Write to big endian values.
  *exponent = endian::write_be(*exponent);
  *mi = endian::write_be(*mi);
}

/******************************************************************************
 * meat::grinder::ast::ContextBlock Class
 */

ContextBlock::ContextBlock() {
  temp_counter = 0;
}

ContextBlock::~ContextBlock() throw() {
  std::deque<Node *>::iterator it;
  /*for (it = commands.begin(); it != commands.end(); it++) {
    delete *it;
  }*/
}

void ContextBlock::gen_bytecode(bool prelim) {
  throw Exception("Can't generate a code block without a local variable"
      " destination");
}

LocalVariable ContextBlock::gen_result(bool prelim) {
  uint16_t bc_mark;

  if (prelim) {
    // Get a local variable for the BlockContext object.
    local_var = scope().anon_local();

  } else {
#ifdef DEBUG
    std::cout << "BC: BLOCK " << local_var.name() << " " << std::dec
              << (unsigned int)local_names.size() << "\n";
#endif
    bytecode(meat::bytecode::BLOCK);
    bytecode((uint8_t)local_var.index());
    bytecode((uint8_t)_locals.size());
    bc_mark = bytecode();
    bytecode((uint16_t)0);
  }

  std::deque<Node *>::iterator it;
  for (it = _nodes.begin(); it != _nodes.end(); it++) {
    (*it)->gen_bytecode(prelim);
    temp_counter = 0;
  }

  if (!prelim) {
#ifdef DEBUG
    std::cout << "BC: CTXEND" << std::endl;
#endif
    bytecode(meat::bytecode::CONTEXT_END);

    uint16_t bc_mark_end = bytecode();
    bytecode(bc_mark, (uint16_t)(bc_mark_end - bc_mark - 2));
  }

  return local_var;
}

std::uint8_t ContextBlock::locals() const {
  return _scope->locals() + _locals.size();
}

LocalVariable ContextBlock::anon_local() {
  std::stringstream temp_name;
  temp_name << ".anon.b." << (unsigned int)++temp_counter;
  return local(temp_name.str());
}

LocalVariable ContextBlock::resolve_local(const std::string &name) const {
  for (unsigned int index = 0; index < _locals.size(); index++) {
    if (_locals[index] == name)
      return LocalVariable(name, (Block &)*this);
  }

  return Node::resolve_local(name);
}

std::uint8_t ContextBlock::local_offset() const {
  return scope().locals();
}

std::uint8_t ContextBlock::local_count() const {
  return scope().locals() + _locals.size();
}

/******************************************************************************
 * AST Value Class Node
 */

Value::Value(const std::string &value, bool string_constant)
  : vtype(UNKNOWN), value(value) {

  if (string_constant) vtype = STRING_CONST;
}

Value::~Value() throw() {
}

Value::value_type_t Value::get_type() const {
  return vtype;
}

void Value::resolve() {
  if (vtype == UNKNOWN) {
    int16_t local_idx;

    if (not Utils::is_integer(value, int_value) and
        not Utils::is_float(value, flt_value)) {

      if ((local_idx = resolve_property(value)) != -1) {
#ifdef DEBUG
        std::cout << "STAGE: Resolved property " << value << " as "
                  << std::dec << local_idx << std::endl;
#endif
        vtype = OBJECT_PROPERTY;
        index = local_idx;
      } else if ((local_idx = resolve_class_property(value)) != -1) {
#ifdef DEBUG
        std::cout << "STAGE: Resolved class property " << value << std::endl;
#endif
        vtype = CLASS_PROPERTY;
        index = local_idx;
      } else {
        //set_result_dest(resolve_local(value));
        LocalVariable source = resolve_local(value);
        if (not source.name().empty()) {
#ifdef DEBUG
          std::cout << "STAGE: Resolved local " << source.name()
                    << " as " << (unsigned int)source.index() << std::endl;
#endif
          vtype = LOCAL_VARIABLE;
          index = source.index();
          if (not result_set)
            set_result_dest(source);
        }
      }
    }
  }
}

void Value::gen_bytecode(bool prelim) {
  throw Exception("Unable to generate a value without a local variable"
                  " destination");
}

LocalVariable Value::gen_result(bool prelim) {
  if (prelim) {
    // First state in compilation, attempt to optimize assignment.

    if (vtype == STRING_CONST) {
#ifdef DEBUG
      std::cout << "STAGE: String constant \"" << value << "\""
                << std::endl;
#endif
      set_result_dest();
    } else if (vtype == UNKNOWN) {
      int16_t local_idx;

      if (Utils::is_integer(value, int_value)) {
        vtype = INT_CONST;
        set_result_dest();
      } else if (Utils::is_float(value, flt_value)) {
        vtype = FLOAT_CONST;
        set_result_dest();
      } else if ((local_idx = resolve_property(value)) != -1) {
#ifdef DEBUG
        std::cout << "STAGE: Resolved property " << value << std::endl;
#endif
        vtype = OBJECT_PROPERTY;
        index = local_idx;
        set_result_dest();
      } else if ((local_idx = resolve_class_property(value)) != -1) {
#ifdef DEBUG
        std::cout << "STAGE: Resolved class property " << value << std::endl;
#endif
        vtype = CLASS_PROPERTY;
        index = local_idx;
        set_result_dest();
      } else {
        LocalVariable source = resolve_local(value);
        if (not source.name().empty()) {
#ifdef DEBUG
          std::cout << "STAGE: Resolved local " << source.name()
                    << " as " << std::dec << (unsigned int)source.index()
                    << std::endl;
#endif
          vtype = LOCAL_VARIABLE;
          index = source.index();
          if (not result_set)
            set_result_dest(source);
        } else {
          result_set = false;
          if (meat::Class::have_class(value.c_str())) {
#ifdef DEBUG
            std::cout << "STAGE: Resolved class " << value << std::endl;
#endif
            vtype = CLASS_REFERENCE;
            set_result_dest();
          } else {
#ifdef DEBUG
            std::cout << "STAGE: " << value << " is unresolved." << std::endl;
#endif
          }
        }
      }
    }
  } else {
    // The actual byte code generation.

    switch (vtype) {
    case STRING_CONST: {
#ifdef DEBUG
      std::cout << "BC: STR " << result.name()
                << " \"" << value << "\"" << std::endl;
#endif
      bytecode(bytecode::ASSIGN_CONST_TXT);
      bytecode(result.index());
      for (unsigned int c = 0; c < value.size(); c++)
        bytecode((uint8_t)value[c]);
      bytecode((uint8_t)0);
      return result;
    }
    case INT_CONST: {
#ifdef DEBUG
      std::cout << "BC: INT " << result.name()
                << " \"" << std::dec << (int)int_value << "\"" << std::endl;
#endif
      bytecode(bytecode::ASSIGN_CONST_INT);
      bytecode(result.index());
      bytecode((int32_t)int_value);
      return result;
    }
    case FLOAT_CONST: {
#ifdef DEBUG
      std::cout << "BC: NUM " << result.name()
                << " \"" << (float)flt_value << "\"" << std::endl;
#endif
      bytecode(bytecode::ASSIGN_CONST_NUM);
      bytecode(result.index());
      bytecode((float_t)flt_value);
      return result;
    }
    case BLOCK_PARAMETER:
#ifdef DEBUG
      std::cout << "BC: INT " << result.name()
                << " \"" << std::dec << (int)int_value << "\"" << std::endl;
#endif
      bytecode(bytecode::ASSIGN_CONST_INT);
      bytecode(result.index());
      bytecode((int32_t)block_param.index());
      return result;
    case OBJECT_PROPERTY: {
#ifdef DEBUG
      std::cout << "BC: PROP " << result.name() << " "
                << std::dec << (unsigned int)index << std::endl;
#endif
      bytecode(bytecode::ASSIGN_PROP);
      bytecode(result.index());
      bytecode((uint8_t)index);
      return result;
    }
    case CLASS_PROPERTY: {
#ifdef DEBUG
      std::cout << "BC: CLASS PROP " << result.name() << " "
                << std::dec << (unsigned int)index << std::endl;
#endif
      bytecode(bytecode::ASSIGN_CLASS_PROP);
      bytecode(result.index());
      bytecode((uint8_t)index);
      return result;
    }
    case CLASS_REFERENCE:
#ifdef DEBUG
      std::cout << "BC: CLASS " << result.name() << " "
                << std::hex << value << std::endl;
#endif
      bytecode(bytecode::ASSIGN_CLASS);
      bytecode(result.index());
      bytecode((uint32_t)hash(value.c_str()));
      return result;
    case LOCAL_VARIABLE:
      if (result.name() != value) {
#ifdef DEBUG
        std::cout << "BC: MOV " << result.name() << " " << std::dec
                  << value << std::endl;;
#endif
        bytecode(meat::bytecode::ASSIGN);
        bytecode(result.index());
        bytecode(index);
      }
      break;
    default:
      throw Exception(std::string("Unknown result destination in assignment"
                                  " for ") + value);
    }
  }

  return result;
}

void Value::make_local() {
  result = local(value);
  result_set = true;
  vtype = LOCAL_VARIABLE;
  index = result.index();
}

void Value::make_block_param(Block *block) {
  result = anon_local();
  result_set = true;
  block_param = block->local(value);
  vtype = BLOCK_PARAMETER;
}

/******************************************************************************
 * meat::grinder::ast::Identifier Class
 */

/**********************************************
 * meat::grinder::ast::Identifier::Identifier *
 **********************************************/

Identifier::Identifier(const std::string &value)
  : _type(UNKNOWN), _name(value), _index(0) {
}

/***********************************************
 * meat::grinder::ast::Identifier::~Identifier *
 ***********************************************/

Identifier::~Identifier() noexcept {
}

/*********************************************
 * meat::grinder::ast::Identifier::new_local *
 *********************************************/

void Identifier::new_local() {
  if (_type == UNKNOWN) {
    result = local(_name);
    result_set = true;
    _type = LOCAL_VARIABLE;
    _index = result.index();
  }
}

/***************************************************
 * meat::grinder::ast::Identifier::block_parameter *
 ***************************************************/

void Identifier::block_parameter(Block *block) {
  result = anon_local();
  result_set = true;
  _parameter = block->local(_name);
  _type = BLOCK_PARAMETER;
}

/************************************************
 * meat::grinder::ast::Identifier::gen_bytecode *
 ************************************************/

void Identifier::gen_bytecode(bool prelim) {
  throw Exception("Internal AST Error: Can't generate bytecode for an "
                  "identifier without a result destination.");
}

/**********************************************
 * meat::grinder::ast::Identifier::gen_result *
 **********************************************/

LocalVariable Identifier::gen_result(bool prelim) {
  if (prelim) {
    /* The first stage in compilation where we resolve the identifier and
     * attempt to optimize the destination of the result.
     */

    if (_type == UNKNOWN) {
      int16_t local_idx;

      if ((local_idx = resolve_property(_name)) != -1) {
        // Is it an object property?
  #ifdef DEBUG
        std::cout << "STAGE: Resolved identifier " << _name << " as property "
                  << std::dec << local_idx << std::endl;
  #endif
        _type = OBJECT_PROPERTY;
        _index = local_idx;
        set_result_dest();

      } else if ((local_idx = resolve_class_property(_name)) != -1) {
        // Is it a class property?
  #ifdef DEBUG
        std::cout << "STAGE: Resolved identifier " << _name
                  << " as class property " << std::dec << local_idx
                  << std::endl;
  #endif
        _type = CLASS_PROPERTY;
        _index = local_idx;
        set_result_dest();

      } else {
        // Is it a local variable?
        LocalVariable source = resolve_local(_name);
        if (not source.name().empty()) {
  #ifdef DEBUG
          std::cout << "STAGE: Resolved identifier " << source.name()
                    << " as local variable " << (unsigned int)source.index()
                    << std::endl;
  #endif
          _type = LOCAL_VARIABLE;
          _index = source.index();
          if (not result_set) set_result_dest(source);

        } else {
          // Is it a class object?
          result_set = false;
          if (meat::Class::have_class(_name)) {
  #ifdef DEBUG
            std::cout << "STAGE: Resolved identifier " << _name
                      << " as class. " << std::endl;
  #endif
            _type = CLASS_REFERENCE;
            set_result_dest();

          } else {
            // So far we haven't been able to resolve the identifier.
  #ifdef DEBUG
            std::cout << "STAGE: Identifier " << _name << " is unresolved."
                      << std::endl;
  #endif
          }
        }
      }
    }

  } else {
    // The byte code generation.

    switch (_type) {
    case BLOCK_PARAMETER:
#ifdef DEBUG
      std::cout << "BC: INT " << result.name()
                << " \"" << std::dec << (int)_parameter.index() << "\""
                << std::endl;
#endif
      bytecode(bytecode::ASSIGN_CONST_INT);
      bytecode(result.index());
      bytecode((int32_t)_parameter.index());
      return result;
    case OBJECT_PROPERTY: {
#ifdef DEBUG
      std::cout << "BC: PROP " << result.name() << " "
                << std::dec << (unsigned int)_index << std::endl;
#endif
      bytecode(bytecode::ASSIGN_PROP);
      bytecode(result.index());
      bytecode((uint8_t)_index);
      return result;
    }
    case CLASS_PROPERTY: {
#ifdef DEBUG
      std::cout << "BC: CLASS PROP " << result.name() << " "
                << std::dec << (unsigned int)_index << std::endl;
#endif
      bytecode(bytecode::ASSIGN_CLASS_PROP);
      bytecode(result.index());
      bytecode((uint8_t)_index);
      return result;
    }
    case CLASS_REFERENCE:
#ifdef DEBUG
      std::cout << "BC: CLASS " << result.name() << " "
                << std::hex << _name << std::endl;
#endif
      bytecode(bytecode::ASSIGN_CLASS);
      bytecode(result.index());
      bytecode((uint32_t)hash(_name));
      return result;
    case LOCAL_VARIABLE:
      if (result.name() != _name) {
#ifdef DEBUG
        std::cout << "BC: MOV " << result.name() << " " << std::dec
                  << _name << std::endl;;
#endif
        bytecode(meat::bytecode::ASSIGN);
        bytecode(result.index());
        bytecode(_index);
      }
      break;
    default:
      throw Exception(std::string("Identifier ") + _name + " is unresolved.");
    }
  }

  return result;
}

/******************************************************************************
 * meat::grinder::ast::Constant Class
 */

Constant::Constant(const std::string &value)
  : _type(TEXT), _text(value) {
}

Constant::Constant(std::int32_t value)
  : _type(INTEGER) {
  _value.integer = value;
}

Constant::Constant(double value)
  : _type(NUMBER) {
  _value.number = value;
}

Constant::~Constant() noexcept {
}

void Constant::gen_bytecode(bool prelim) {
}

LocalVariable Constant::gen_result(bool prelim) {
  if (prelim) {
    set_result_dest();
  } else {
    switch (_type) {
    case TEXT: {
#ifdef DEBUG
      std::cout << "BC: STR " << result.name()
                << " \"" << _text << "\"" << std::endl;
#endif
      bytecode(bytecode::ASSIGN_CONST_TXT);
      bytecode(result.index());
      for (unsigned int c = 0; c < _text.size(); c++)
        bytecode((uint8_t)_text[c]);
      bytecode((uint8_t)0);
      return result;
    }
    case INTEGER: {
#ifdef DEBUG
      std::cout << "BC: INT " << result.name()
                << " \"" << std::dec << (int)_value.integer << "\""
                << std::endl;
#endif
      bytecode(bytecode::ASSIGN_CONST_INT);
      bytecode(result.index());
      bytecode((int32_t)_value.integer);
      return result;
    }
    case NUMBER: {
#ifdef DEBUG
      std::cout << "BC: NUM " << result.name()
                << " \"" << (float)_value.number << "\"" << std::endl;
#endif
      bytecode(bytecode::ASSIGN_CONST_NUM);
      bytecode(result.index());
      bytecode((double)_value.number);
      return result;
    }
    }
  }

  return result;
}

/******************************************************************************
 * meat::grinder::ast::Assignment Class
 */

Assignment::Assignment(Identifier *dest, Node *src)
  : dest(dest), src(src) {
}

Assignment::~Assignment() throw() {
  delete dest;
  delete src;
}

void Assignment::scope(Block *block) {
  Node::scope(block);
  dest->scope(block);
  src->scope(block);
}

void Assignment::gen_bytecode(bool prelim) {
  // Generate the code needed for the source value.

  if (prelim) {
    dest->gen_result(prelim);

    // This indicates the first time the variable has been addressed.
    dest->new_local();

    if (dest->type() == Identifier::LOCAL_VARIABLE)
      src->set_result_dest(dest->get_result_dest());
    src->gen_result(prelim);
  } else {
    LocalVariable source = src->gen_result(prelim);

    /* Determine the type of assignment and generate the bytecode.
     */
    switch (dest->type()) {

    case Identifier::OBJECT_PROPERTY:
      // Assign to an object property.
#ifdef DEBUG
      std::cout << "BC: SETATTR " << std::dec
                << (unsigned int)dest->index() << " ";
#endif
      bytecode(meat::bytecode::SET_PROP);
      bytecode(dest->index());
#ifdef DEBUG
      std::cout << source.name() << std::endl;
#endif
      bytecode(source.index());
      break;

    case Identifier::CLASS_PROPERTY:
      // Assign to a class property.
#ifdef DEBUG
      std::cout << "BC: GETCATTR " << std::dec
                << (unsigned int)dest->index() << " ";
#endif
      bytecode(meat::bytecode::SET_CLASS_PROP);
      bytecode(dest->index());
#ifdef DEBUG
      std::cout << source.name() << std::endl;
#endif
      bytecode(source.index());
      break;

    case Identifier::LOCAL_VARIABLE:
      // Should already be set to a local variable
      break;
    default: { // TODO This should not be the default
      throw Exception("Illegal assignment");
    }
    }
  }
}

LocalVariable Assignment::gen_result(bool prelim) {
  throw Exception("Cannot get the results of an assignment.");
}

/******************************************************************************
 * AST Message Class Node
 */

Message::Message(Node *who) : who(who) {
}

Message::Message(Node *who, const std::string &name)
  : who(who), _method(name) {
}

Message::~Message() throw() {
  delete who;

  std::deque<Node *>::iterator param_it;
  for (param_it = params.begin(); param_it != params.end(); param_it++) {
    delete (*param_it);
  }
}

void Message::method(const std::string &name) {
  _method = name;
}

void Message::add_param(Node *param) {
  params.push_back(param);
  param->scope(_scope);
}

void Message::message_super(bool to_super) {
  super = to_super;
}

/**************************************
 * meat::grinder::ast::Message::scope *
 **************************************/

void Message::scope(Block *block) {
  Node::scope(block); // Set our scope.

  who->scope(block); // Set the scope for the messaged object.

  // Set the scope for our parameters.
  std::deque<Node *>::iterator it = params.begin();
  for (; it != params.end(); ++it)
    (*it)->scope(block);
}

void Message::gen_bytecode(bool prelim) {
  LocalVariable who_var = who->gen_result(prelim);

  std::deque<LocalVariable> param_idxs;
  std::deque<Node *>::iterator it;
  for (it = params.begin(); it != params.end(); it++) {

    if ((*it)->is_block() and it != params.begin()) {
      std::deque<Node *>::iterator param_it = it;

      for  (;; param_it--) {

        if (param_it == it)
          continue;

        if ((*param_it)->is_block())
          break;

        if ((*param_it)->is_value()) {
          if (((Value *)(*param_it))->get_type() == Value::UNKNOWN)  {
            ((Value *)(*param_it))->make_block_param((Block *)*it);
          }
        }

        if (param_it == params.begin()) break;
      }
    }

    param_idxs.push_back((*it)->gen_result(prelim));
  }

  if (!prelim) {
    LocalVariable lindex;

    if (super) {
#ifdef DEBUG
      std::cout << "BC: SUPER " << who_var.name() << " " << _method << " "
                << std::dec << (unsigned int)param_idxs.size();
#endif
      bytecode(meat::bytecode::MESG_SUPER);
    } else {
#ifdef DEBUG
      std::cout << "BC: MESSAGE " << who_var.name() << " "
                << _method << " " << std::dec
                << (unsigned int)param_idxs.size();
#endif
      bytecode(meat::bytecode::MESSAGE);
    }

    bytecode(who_var.index());
    bytecode((uint32_t)hash(_method));
    bytecode((uint8_t)param_idxs.size());
    for (uint16_t c = 0; c < param_idxs.size(); c++) {
#ifdef DEBUG
      std::cout << " " << param_idxs[c].name();
#endif
      bytecode(param_idxs[c].index());
    }

#ifdef DEBUG
    std::cout << std::endl;
#endif
  }
}

LocalVariable Message::gen_result(bool prelim) {
  LocalVariable who_var = who->gen_result(prelim);

  std::deque<LocalVariable> param_idxs;
  std::deque<Node *>::iterator it;
  for (it = params.begin(); it != params.end(); it++) {
    param_idxs.push_back((*it)->gen_result(prelim));
  }

  if (prelim) {
    //result = add_temp_local();
    set_result_dest();
  }

  if (!prelim) {
    LocalVariable lindex;

    if (super) {
#ifdef DEBUG
      std::cout << "BC: SUPRES " << who_var.name() << " "
                << result.name() << " " << _method << " "
                << std::dec << (unsigned int)param_idxs.size();
#endif
      bytecode(meat::bytecode::MESG_SUPER_RESULT);
    } else {
#ifdef DEBUG
      std::cout << "BC: MSGRES " << who_var.name() << " "
                << result.name() << " " << _method << " "
                << std::dec << (unsigned int)param_idxs.size();
#endif
      bytecode(meat::bytecode::MESG_RESULT);
    }

    bytecode(who_var.index());
    bytecode(result.index());
    bytecode((uint32_t)hash(_method));
    bytecode((uint8_t)param_idxs.size());

    for (uint16_t c = 0; c < param_idxs.size(); c++) {
#ifdef DEBUG
      std::cout << " " << param_idxs[c].name();
#endif
      bytecode(param_idxs[c].index());
    }

#ifdef DEBUG
    std::cout << std::endl;
#endif
  }

  return result;
}
