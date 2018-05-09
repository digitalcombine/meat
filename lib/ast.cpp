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
  : _name(), _block(NULL) {
}

LocalVariable::LocalVariable(const std::string &name, Block &block_node)
  : _name(name), _block(&block_node) {
}

LocalVariable::LocalVariable(const LocalVariable &other)
  : _name(other._name), _block(other._block) {
}

/*************************************************
 * meat::grinder::ast::LocalVariable::operator = *
 *************************************************/

LocalVariable &LocalVariable::operator =(const LocalVariable &other) {
  if (this != &other) {
    _name = other._name;
    _block = other._block;
  }
  return *this;
}

/********************************************
 * meat::grinder::ast::LocalVariable::index *
 ********************************************/

std::uint8_t LocalVariable::index() const {
  if (_block) {
    for (unsigned int idx = 0; idx < _block->_locals.size(); ++idx) {
      if (_block->_locals[idx] == _name)
        return (_block->locals() - _block->_locals.size()) + idx;
    }
  }

  throw Exception(std::string("Internal AST Error: Undefined local variable ")
                  + _name + " reference.");
}

/******************************************************************************
 * meat::grinder::ast::Node Class
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

/************************************
 * meat::grinder::ast::Node::locals *
 ************************************/

std::uint8_t Node::locals() const {
  return scope().locals();
}

/***********************************
 * meat::grinder::ast::Node::local *
 ***********************************/

LocalVariable Node::local(const std::string &name) {
  return scope().local(name);
}

/****************************************
 * meat::grinder::ast::Node::anon_local *
 ****************************************/

LocalVariable Node::anon_local() {
  return scope().anon_local();
}

/*********************************************
 * meat::grinder::ast::Node::set_result_dest *
 *********************************************/

void Node::set_result_dest(LocalVariable dest) {
  result = dest;
  result_set = true;
}

void Node::set_result_dest() {
  if (not result_set) { // Only if the result hasn't been already set.
    result = anon_local();
    result_set = true;
  }
}

/*********************************************
 * meat::grinder::ast::Node::get_result_dest *
 *********************************************/

LocalVariable Node::get_result_dest() {
  return result;
}

/**********************************************
 * meat::grinder::ast::Node::resolve_property *
 **********************************************/

std::int16_t Node::resolve_property(const std::string &name) const {
  return scope().resolve_property(name);
}

/****************************************************
 * meat::grinder::ast::Node::resolve_class_property *
 ****************************************************/

std::int16_t Node::resolve_class_property(const std::string &name) const {
  return scope().resolve_class_property(name);
}

/*******************************************
 * meat::grinder::ast::Node::resolve_local *
 *******************************************/

LocalVariable Node::resolve_local(const std::string &name) const {
  return scope().resolve_local(name);
}

/****************************************
 * meat::grinder::ast::Node::add_symbol *
 ****************************************/

void Node::add_symbol(const std::string &name) {
  if (_scope)
    _scope->add_symbol(name);
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
  for (auto &node: _nodes)
    delete node;
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

/**************************************
 * meat::grinder::ast::Method::Method *
 **************************************/

Method::Method(const meat::List &properties, int p_offset,
               const meat::List &class_properties, int cp_offset) {
  // Add the properties to the method for easier resolution.
  for (auto &property_name: properties)
    this->_properties.push_back(cast<const Text>(property_name));
  this->_p_offset = p_offset;

  // Add the class properties to the method for easier resolution.
  for (auto &property_name: class_properties)
    this->_class_properties.push_back(cast<const Text>(property_name));
  this->_cp_offset = cp_offset;

  local("self");
  local("class");
  local("context");
  local("null");
}

/***************************************
 * meat::grinder::ast::Method::~Method *
 ***************************************/

Method::~Method() throw() {
}

/*********************************************
 * meat::grinder::ast::Method::add_parameter *
 *********************************************/

std::uint8_t Method::add_parameter(const std::string &name) {
#ifdef DEBUG
  std::cout << "STAGE: Adding parameter " << name << std::endl;
#endif
  local(name);
  return _locals.size() - 1;
}

/********************************************
 * meat::grinder::ast::Method::gen_bytecode *
 ********************************************/

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

LocalVariable Method::gen_result(bool prelim __attribute__((unused))) {
  throw Exception("A method cannot be placed in a local variable.");
}

/*****************************************
 * meat::grinder::ast:Method::add_symbol *
 *****************************************/

void Method::add_symbol(const std::string &name) {
  _symbols.insert(name);
}

void Method::update_symbols(std::set<std::string> &symbols) {
  symbols.insert(_symbols.begin(), _symbols.end());
}

void Method::append_bytecode(std::vector<uint8_t> &bc) {
  for (auto &byte: _bytecode)
    bc.push_back(byte);
}

std::int16_t Method::resolve_property(const std::string &name) const {
  for (unsigned int index = 0; index < _properties.size(); index++) {
    if (_properties[index] == name) return index + _p_offset;
  }
  return -1;
}

std::int16_t Method::resolve_class_property(const std::string &name) const {
  for (unsigned int index = 0; index < _class_properties.size(); index++) {
    if (_class_properties[index] == name) return index + _cp_offset;
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

/****************************************
 * meat::grinder::ast::Method::bytecode *
 ****************************************/

std::uint16_t Method::bytecode() {
  return _bytecode.size();
}

void Method::bytecode(uint8_t value) {
  _bytecode.push_back(value);
}

void Method::bytecode(uint16_t value) {
  _bytecode.push_back(0); // Add two bytes to the bytecode
  _bytecode.push_back(0);

  // Now set the new value to the bytecode
  uint16_t *iptr = (uint16_t *)&_bytecode[_bytecode.size() - 2];
  *iptr = endian::write_be(value);
}

void Method::bytecode(uint16_t marker, uint16_t value) {
  // Set the new value to the bytecode
  uint16_t *iptr = (uint16_t *)&_bytecode[marker];
  *iptr = endian::write_be(value);
}

void Method::bytecode(uint32_t value) {
  _bytecode.push_back(0); // Add four bytes to the bytecode
  _bytecode.push_back(0);
  _bytecode.push_back(0);
  _bytecode.push_back(0);

  // Now set the new value to the bytecode
  int32_t *iptr = (int32_t *)&_bytecode[_bytecode.size() - 4];
  *iptr = endian::write_be(value);
}

void Method::bytecode(int32_t value) {
  _bytecode.push_back(0); // Add four bytes to the bytecode
  _bytecode.push_back(0);
  _bytecode.push_back(0);
  _bytecode.push_back(0);

  // Now set the new value to the bytecode
  uint32_t *iptr = (uint32_t *)&_bytecode[_bytecode.size() - 4];
  *iptr = endian::write_be(value);
}

void Method::bytecode(double value) {
  for (unsigned int c = 0; c < sizeof(std::int64_t) + sizeof(std::int32_t); c++)
    _bytecode.push_back(0);

  // Now set the new value to the bytecode
  std::int64_t *mi = (std::int64_t *)&_bytecode[_bytecode.size() -
                                                (sizeof(std::int64_t) +
                                                 sizeof(std::int32_t))];
  std::int32_t *exponent =  (std::int32_t *)&_bytecode[_bytecode.size() -
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
}

/**************************************************
 * meat::grinder::ast::ContextBlock::gen_bytecode *
 **************************************************/

void ContextBlock::gen_bytecode(bool prelim __attribute__((unused))) {
  throw Exception("Can't generate a code block without a local variable"
      " destination");
}

/************************************************
 * meat::grinder::ast::ContextBlock::gen_result *
 ************************************************/

LocalVariable ContextBlock::gen_result(bool prelim) {
  uint16_t bc_mark = 0;

  if (prelim) {
    // Get a local variable for the BlockContext object.
    _local_var = scope().anon_local();

  } else {
#ifdef DEBUG
    std::cout << "BC: BLOCK " << _local_var.name() << " " << std::dec
              << (unsigned int)_local_names.size() << "\n";
#endif
    bytecode(meat::bytecode::BLOCK);
    bytecode((uint8_t)_local_var.index());
    bytecode((uint8_t)_locals.size());
    bc_mark = bytecode();
    bytecode((uint16_t)0);
  }

  for (auto &node: _nodes) {
    node->gen_bytecode(prelim);
    temp_counter = 0;
  }

  if (not prelim) {
#ifdef DEBUG
    std::cout << "BC: CTXEND" << std::endl;
#endif
    bytecode(meat::bytecode::CONTEXT_END);

    uint16_t bc_mark_end = bytecode();
    bytecode(bc_mark, (uint16_t)(bc_mark_end - bc_mark - 2));
  }

  return _local_var;
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
  if (_name[0] != '.')
    throw Exception(std::string("Identifier ") + _name +
                    " is not a block parameter.");
  result = anon_local();
  result_set = true;
  _parameter = block->local(_name);
  _type = BLOCK_PARAMETER;
}

/************************************************
 * meat::grinder::ast::Identifier::gen_bytecode *
 ************************************************/

void Identifier::gen_bytecode(bool prelim __attribute__((unused))) {
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
            add_symbol(_name);
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
      std::cout << "BC: PROP " << (unsigned int)result.index() << " "
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

/******************************************
 * meat::grinder::ast::Constant::Constant *
 ******************************************/

Constant::Constant(const std::string &value) : _type(TEXT), _text(value) {
}

Constant::Constant(std::int32_t value) : _type(INTEGER) {
  _value.integer = value;
}

Constant::Constant(double value) : _type(NUMBER) {
  _value.number = value;
}

/*******************************************
 * meat::grinder::ast::Constant::~Constant *
 *******************************************/

Constant::~Constant() noexcept {
}

/**********************************************
 * meat::grinder::ast::Constant::gen_bytecode *
 **********************************************/

void Constant::gen_bytecode(bool prelim __attribute__((unused))) {
}

/********************************************
 * meat::grinder::ast::Constant::gen_result *
 ********************************************/

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
                << " \"" << (double)_value.number << "\"" << std::endl;
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
  : _destination(dest), _source(src) {
}

Assignment::~Assignment() throw() {
  delete _destination;
  delete _source;
}

/*****************************************
 * meat::grinder::ast::Assignment::scope *
 *****************************************/

void Assignment::scope(Block *block) {
  Node::scope(block);
  _destination->scope(block);
  _source->scope(block);
}

/************************************************
 * meat::grinder::ast::Assignment::gen_bytecode *
 ************************************************/

void Assignment::gen_bytecode(bool prelim) {
  // Generate the code needed for the source value.

  if (prelim) {
    _destination->gen_result(prelim);

    // This indicates the first time the variable has been addressed.
    _destination->new_local();

    if (_destination->type() == Identifier::LOCAL_VARIABLE)
      _source->set_result_dest(_destination->get_result_dest());
    _source->gen_result(prelim);
  } else {
    LocalVariable source = _source->gen_result(prelim);

    /* Determine the type of assignment and generate the bytecode.
     */
    switch (_destination->type()) {

    case Identifier::OBJECT_PROPERTY:
      // Assign to an object property.
#ifdef DEBUG
      std::cout << "BC: SETATTR " << std::dec
                << (unsigned int)_destination->index() << " ";
#endif
      bytecode(meat::bytecode::SET_PROP);
      bytecode(_destination->index());
#ifdef DEBUG
      std::cout << source.name() << std::endl;
#endif
      bytecode(source.index());
      break;

    case Identifier::CLASS_PROPERTY:
      // Assign to a class property.
#ifdef DEBUG
      std::cout << "BC: GETCATTR " << std::dec
                << (unsigned int)_destination->index() << " ";
#endif
      bytecode(meat::bytecode::SET_CLASS_PROP);
      bytecode(_destination->index());
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

/**********************************************
 * meat::grinder::ast::Assignment::gen_result *
 **********************************************/

LocalVariable Assignment::gen_result(bool prelim __attribute__((unused))) {
  throw Exception("Cannot get the results of an assignment.");
}

/******************************************************************************
 * AST Message Class Node
 */

/****************************************
 * meat::grinder::ast::Message::Message *
 ****************************************/

Message::Message(Node *who) : _who(who) {
}

Message::Message(Node *who, const std::string &name)
  : _who(who), _method(name) {
}

/*****************************************
 * meat::grinder::ast::Message::~Message *
 *****************************************/

Message::~Message() throw() {
  delete _who;

  for (auto &node: _parameters)
    delete node;
}

/***************************************
 * meat::grinder::ast::Message::method *
 ***************************************/

void Message::method(const std::string &name) {
  _method = name;
}

/******************************************
 * meat::grinder::ast::Message::add_param *
 ******************************************/

void Message::add_param(Node *param) {
  _parameters.push_back(param);
  param->scope(_scope);
}

/**********************************************
 * meat::grinder::ast::Message::message_super *
 **********************************************/

void Message::message_super(bool to_super) {
  _super = to_super;
}

/**************************************
 * meat::grinder::ast::Message::scope *
 **************************************/

void Message::scope(Block *block) {
  Node::scope(block); // Set our scope.

  _who->scope(block); // Set the scope for the messaged object.

  // Set the scope for our parameters.
  for (auto &node: _parameters)
    node->scope(block);
}

/*********************************************
 * meat::grinder::ast::Message::gen_bytecode *
 *********************************************/

void Message::gen_bytecode(bool prelim) {
  LocalVariable who_var = _who->gen_result(prelim);

  std::deque<LocalVariable> param_idxs;

  for (auto it = _parameters.begin(); it != _parameters.end(); it++) {

    if ((*it)->is_block() and it != _parameters.begin()) {

      for  (auto param_it = it;; param_it--) {

        if (param_it == it) continue;
        if ((*param_it)->is_block()) break;

        if ((*param_it)->is_value()) {
          if (((Identifier *)(*param_it))->type() == Identifier::UNKNOWN)  {
            ((Identifier *)(*param_it))->block_parameter((Block *)*it);
          }
        }

        if (param_it == _parameters.begin()) break;
      }
    }

    param_idxs.push_back((*it)->gen_result(prelim));
  }

  if (not prelim) {
    LocalVariable lindex;

    if (_super) {
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
    for (auto &local: param_idxs) {
#ifdef DEBUG
      std::cout << " " << local.name();
#endif
      bytecode(local.index());
    }

#ifdef DEBUG
    std::cout << std::endl;
#endif
  }
}

/*******************************************
 * meat::grinder::ast::Message::gen_result *
 *******************************************/

LocalVariable Message::gen_result(bool prelim) {
  LocalVariable who_var = _who->gen_result(prelim);

  std::deque<LocalVariable> param_idxs;
  for (auto &node: _parameters)
    param_idxs.push_back(node->gen_result(prelim));

  if (prelim) set_result_dest();

  if (not prelim) {
    LocalVariable lindex;

    if (_super) {
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

    for (auto &local: param_idxs) {
#ifdef DEBUG
      std::cout << " " << local.name();
#endif
      bytecode(local.index());
    }

#ifdef DEBUG
    std::cout << std::endl;
#endif
  }

  return result;
}
