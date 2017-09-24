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

#ifdef TESTING
#include <testing.h>
#endif

using namespace meat::grinder::ast;

/******************************************************************************
 * LocalVariable Class
 */

LocalVariable::LocalVariable()
  : name(""), method((Method *)0), block((Block *)0) {
}

LocalVariable::LocalVariable(const std::string &name, Method &method_node)
  : name(name), method(&method_node), block((Block *)0) {

}

LocalVariable::LocalVariable(const std::string &name, Block &block_node)
  : name(name), method((Method *)0), block(&block_node) {
}

LocalVariable::LocalVariable(const LocalVariable &other)
  : name(other.name), method(other.method), block(other.block){
}

LocalVariable &LocalVariable::operator = (const LocalVariable &other) {
  if (this != &other) {
    name = other.name;
    method = other.method;
    block = other.block;
  }
  return *this;
}

meat::uint8_t LocalVariable::get_index() const {
  if (method != (Method *)0) {
    for (uint8_t index = 0; index < method->local_names.size(); index++) {
      if (method->local_names[index] == name) return index;
    }
  } else if (block != (Block *)0) {
    for (uint8_t index = 0; index < block->local_names.size(); index++) {
      if (block->local_names[index] == name)
        return block->local_offset() + index;
    }
  }

  throw Exception(std::string("Internal Error, undefined local variable ") +
                  name + " reference.");
}

/******************************************************************************
 * ASTNode Class
 */

ASTNode::ASTNode() : container((ASTNode *)0), result_set(false) {
}

ASTNode::~ASTNode() throw() {
}

void ASTNode::add(ASTNode *node) {
  node->container = this;
}

meat::uint8_t ASTNode::locals() const {
  if (container) return container->locals();
  else throw Exception("Local count wanted with no source.");
}

LocalVariable ASTNode::add_local(const std::string &name) {
  if (container) return container->add_local(name);
  else throw Exception("Local variable creation with no destination.");
}

LocalVariable ASTNode::add_temp_local() {
  if (container) return container->add_temp_local();
  else throw Exception("Temporary local variable creation with no"
           " destination.");
}

void ASTNode::set_result_dest(LocalVariable dest) {
  result = dest;
  result_set = true;
}

void ASTNode::set_result_dest() {
  if (not result_set) {
    result = add_temp_local();
    result_set = true;
  }
}

LocalVariable ASTNode::get_result_dest() {
  return result;
}

meat::int16_t ASTNode::resolve_property(const std::string &name) const {
  if (container) return container->resolve_property(name);
  else throw Exception(std::string("Attempting to resolve object property ") +
           name + " with no source.");
}

meat::int16_t ASTNode::resolve_class_property(const std::string &name) const {
  if (container) return container->resolve_class_property(name);
  else throw Exception("Attempting to resolve class property with no source.");
}

LocalVariable ASTNode::resolve_local(const std::string &name) const {
  if (container) return container->resolve_local(name);
  else throw Exception("Attempting to resolve local variable with no source.");
}

meat::uint16_t ASTNode::bytecode() {
  if (container) return container->bytecode();
  else throw Exception("Bytecode marker wanted with no destination.");
}

void ASTNode::bytecode(uint8_t value) {
  if (container) container->bytecode(value);
  else throw Exception("Bytecode generation with no destination.");
}

void ASTNode::bytecode(uint16_t value) {
  if (container) container->bytecode(value);
  else throw Exception("Bytecode generation with no destination.");
}

void ASTNode::bytecode(uint16_t marker, uint16_t value) {
  if (container) container->bytecode(marker, value);
  else throw Exception("Bytecode generation with no destination.");
}

void ASTNode::bytecode(uint32_t value) {
  if (container) container->bytecode(value);
  else throw Exception("Bytecode generation with no destination.");
}

void ASTNode::bytecode(int32_t value) {
  if (container) container->bytecode(value);
  else throw Exception("Bytecode generation with no destination.");
}

void ASTNode::bytecode(float_t value) {
  if (container) container->bytecode(value);
  else throw Exception("Bytecode generation with no destination.");
}

/******************************************************************************
 * AST Method Class Node
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

  temp_counter = 0;

  local_names.push_back("self");
  local_names.push_back("class"); // Internal class reference.
  local_names.push_back("context");
  local_names.push_back("null");
}

Method::~Method() throw() {
  std::deque<ASTNode *>::iterator it;
  for (it = commands.begin(); it != commands.end(); it++) {
    delete *it;
  }
}

void Method::add(ASTNode *command) {
  commands.push_back(command);
  ASTNode::add(command);
}

meat::uint8_t Method::add_parameter(const std::string &name) {
#ifdef DEBUG
  std::cout << "STAGE: Adding parameter " << name << std::endl;
#endif
  local_names.push_back(name);
  return local_names.size() - 1;
}

meat::uint8_t Method::local_count() const {
  return locals();
}

void Method::gen_bytecode(bool prelim) {

  std::deque<ASTNode *>::iterator it;
  for (it = commands.begin(); it != commands.end(); it++) {
    (*it)->gen_bytecode(prelim);
    temp_counter = 0; // We can reuse temporary variable previously created.
  }

  if (!prelim) {
#ifdef DEBUG
    std::cout << "BC: CTXEND" << std::endl;
		std::cout << "CONTEXT: " << local_names.size() << " locals" << std::endl;
#endif
    bytecode(meat::bytecode::CONTEXT_END);
  }
}

LocalVariable Method::gen_result(bool prelim) {
  throw Exception("A method cannot be placed in a local variable.");
}

meat::uint8_t Method::locals() const {
  return local_names.size();
}

LocalVariable Method::add_local(const std::string &name) {
  LocalVariable result(name, *this);
  for (unsigned int index = 0; index < local_names.size(); index++) {
    if (local_names[index] == name) return result;
  }
#ifdef DEBUG
  std::cout << "STAGE: Adding local " << name << std::endl;
#endif
  local_names.push_back(name);
  return result;
}

LocalVariable Method::add_temp_local() {
  std::stringstream temp_name;
  temp_name << ".temp.m." << (unsigned int)++temp_counter;
  return add_local(temp_name.str());
}

void Method::append_bytecode(std::vector<uint8_t> &bc) {
  std::vector<uint8_t>::iterator it;
  for (it = this->bc.begin(); it != this->bc.end(); it++) {
    bc.push_back(*it);
  }
}

meat::int16_t Method::resolve_property(const std::string &name) const {
  for (unsigned int index = 0; index < properties.size(); index++) {
    if (properties[index] == name) return index + p_offset;
  }
  return -1;
}

meat::int16_t Method::resolve_class_property(const std::string &name) const {
  for (unsigned int index = 0; index < class_properties.size(); index++) {
    if (class_properties[index] == name) return index + cp_offset;
  }
  return -1;
}

LocalVariable Method::resolve_local(const std::string &name) const {
  LocalVariable bad_result;
  LocalVariable good_result(name, (Method &)*this);

  for (unsigned int index = 0; index < local_names.size(); index++) {
    if (local_names[index] == name) return good_result;
  }

  return bad_result;
}

meat::uint16_t Method::bytecode() {
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

void Method::bytecode(float_t value) {
  for (unsigned int c = 0; c < sizeof(float_t); c++)
    bc.push_back(0);

  // Now set the new value to the bytecode
  float_t *iptr = (float_t *)&bc[bc.size() - sizeof(float_t)];
  *iptr = endian::write_be(value);
}

/******************************************************************************
 * AST Block Class Node
 */

Block::Block() {
  temp_counter = 0;
}

Block::~Block() throw() {
  std::deque<ASTNode *>::iterator it;
  for (it = commands.begin(); it != commands.end(); it++) {
    delete *it;
  }
}

void Block::add(ASTNode *command) {
  commands.push_back(command);
  ASTNode::add(command);
}

void Block::set_parent_block(Block *parent) {
  this->parent = parent;
}

void Block::gen_bytecode(bool prelim) {
  throw Exception("Can't generate a code block without a local variable"
      " destination");
}

LocalVariable Block::gen_result(bool prelim) {
  uint16_t bc_mark;

  if (prelim) {
    local_var = container->add_temp_local();
  } else {
#ifdef DEBUG
    std::cout << "BC: BLOCK " << local_var.get_name() << " " << std::dec
        << (unsigned int)local_names.size() << std::endl;
#endif
    bytecode(meat::bytecode::BLOCK);
    bytecode((uint8_t)local_var.get_index());
    bytecode((uint8_t)local_names.size());
    bc_mark = bytecode();
    bytecode((uint16_t)0);
  }

  std::deque<ASTNode *>::iterator it;
  for (it = commands.begin(); it != commands.end(); it++) {
    (*it)->gen_bytecode(prelim);
    temp_counter = 0;
  }

  if (!prelim) {
#ifdef DEBUG
    std::cout << "BC: CTXEND " << std::endl;
#endif
    bytecode(meat::bytecode::CONTEXT_END);

    uint16_t bc_mark_end = bytecode();
    bytecode(bc_mark, (uint16_t)(bc_mark_end - bc_mark - 2));
  }

  return local_var;
}

meat::uint8_t Block::locals() const {
  return container->locals() + local_names.size();
}

LocalVariable Block::add_local(const std::string &name) {
  for (unsigned int index = 0; index < local_names.size(); index++) {
    if (local_names[index] == name) return LocalVariable(name, *this);
  }
#ifdef DEBUG
  std::cout << "STAGE: Adding block local " << name << std::endl;
#endif
  local_names.push_back(name);
  return LocalVariable(name, *this);
}

LocalVariable Block::add_temp_local() {
  std::stringstream temp_name;
  temp_name << ".temp.b" << (unsigned int)get_layer() << "."
            << (unsigned int)++temp_counter;
  return add_local(temp_name.str());
}

LocalVariable Block::resolve_local(const std::string &name) const {
  for (unsigned int index = 0; index < local_names.size(); index++) {
    if (local_names[index] == name)
      return LocalVariable(name, (Block &)*this);
  }

  return ASTNode::resolve_local(name);
}

meat::uint8_t Block::get_layer() const {
  if (container == (Block *)0)
    return 1;
  else
    return parent->get_layer() + 1;
}

meat::uint8_t Block::local_offset() const {
  return container->locals();
}

meat::uint8_t Block::local_count() const {
  return container->locals() + local_names.size();
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

    if (not Utils::is_integer(value, &int_value) and
        not Utils::is_float(value, &flt_value)) {

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
        if (not source.get_name().empty()) {
#ifdef DEBUG
          std::cout << "STAGE: Resolved local " << source.get_name()
                    << " as " << (unsigned int)source.get_index() << std::endl;
#endif
          vtype = LOCAL_VARIABLE;
          index = source.get_index();
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

      if (Utils::is_integer(value, &int_value)) {
        vtype = INT_CONST;
        set_result_dest();
      } else if (Utils::is_float(value, &flt_value)) {
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
        if (not source.get_name().empty()) {
#ifdef DEBUG
          std::cout << "STAGE: Resolved local " << source.get_name()
                    << " as " << std::dec << (unsigned int)source.get_index()
                    << std::endl;
#endif
          vtype = LOCAL_VARIABLE;
          index = source.get_index();
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
      std::cout << "BC: STR " << result.get_name()
                << " \"" << value << "\"" << std::endl;
#endif
      bytecode(bytecode::ASSIGN_CONST_TXT);
      bytecode(result.get_index());
      for (unsigned int c = 0; c < value.size(); c++)
        bytecode((uint8_t)value[c]);
      bytecode((uint8_t)0);
      return result;
    }
    case INT_CONST: {
#ifdef DEBUG
      std::cout << "BC: INT " << result.get_name()
                << " \"" << std::dec << (int)int_value << "\"" << std::endl;
#endif
      bytecode(bytecode::ASSIGN_CONST_INT);
      bytecode(result.get_index());
      bytecode((int32_t)int_value);
      return result;
    }
    case FLOAT_CONST: {
#ifdef DEBUG
      std::cout << "BC: NUM " << result.get_name()
                << " \"" << (float)flt_value << "\"" << std::endl;
#endif
      bytecode(bytecode::ASSIGN_CONST_NUM);
      bytecode(result.get_index());
      bytecode((float_t)flt_value);
      return result;
    }
    case BLOCK_PARAMETER:
#ifdef DEBUG
      std::cout << "BC: INT " << result.get_name()
                << " \"" << std::dec << (int)int_value << "\"" << std::endl;
#endif
      bytecode(bytecode::ASSIGN_CONST_INT);
      bytecode(result.get_index());
      bytecode((int32_t)block_param.get_index());
      return result;
    case OBJECT_PROPERTY: {
#ifdef DEBUG
      std::cout << "BC: PROP " << result.get_name() << " "
                << std::dec << (unsigned int)index << std::endl;
#endif
      bytecode(bytecode::ASSIGN_PROP);
      bytecode(result.get_index());
      bytecode((uint8_t)index);
      return result;
    }
    case CLASS_PROPERTY: {
#ifdef DEBUG
      std::cout << "BC: CLASS PROP " << result.get_name() << " "
                << std::dec << (unsigned int)index << std::endl;
#endif
      bytecode(bytecode::ASSIGN_CLASS_PROP);
      bytecode(result.get_index());
      bytecode((uint8_t)index);
      return result;
    }
    case CLASS_REFERENCE:
#ifdef DEBUG
      std::cout << "BC: CLASS " << result.get_name() << " "
                << std::hex << value << std::endl;
#endif
      bytecode(bytecode::ASSIGN_CLASS);
      bytecode(result.get_index());
      bytecode((uint32_t)hash(value.c_str()));
      return result;
    case LOCAL_VARIABLE:
      if (result.get_name() != value) {
#ifdef DEBUG
        std::cout << "BC: MOV " << result.get_name() << " " << std::dec
                  << value << std::endl;;
#endif
        bytecode(meat::bytecode::ASSIGN);
        bytecode(result.get_index());
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
  result = add_local(value);
  result_set = true;
  vtype = LOCAL_VARIABLE;
  index = result.get_index();
}

void Value::make_block_param(Block *block) {
  result = add_temp_local();
  result_set = true;
  block_param = block->add_local(value);
  vtype = BLOCK_PARAMETER;
}

/******************************************************************************
 * AST Assignment Class Node
 */

Assignment::Assignment(Value *dest, ASTNode *src)
  : dest(dest), src(src) {
  add(dest);
  add(src);
}

Assignment::~Assignment() throw() {
  delete dest;
  delete src;
}

void Assignment::gen_bytecode(bool prelim) {
  // Generate the code needed for the source value.

  if (prelim) {
    dest->resolve();

    // This indicates the first time the variable has been addressed.
    if (dest->get_type() == Value::UNKNOWN) {
      dest->make_local();
    }

    if (dest->get_type() == Value::LOCAL_VARIABLE)
      src->set_result_dest(dest->get_result_dest());
    src->gen_result(prelim);
  } else {
    LocalVariable source = src->gen_result(prelim);

    /* Determine the type of assignment and generate the bytecode.
     */
    switch (dest->get_type()) {
    case Value::OBJECT_PROPERTY:
      // Assign to an object property.
#ifdef DEBUG
      std::cout << "BC: SETATTR " << std::dec
    << (unsigned int)dest->get_index() << " ";
#endif
      bytecode(meat::bytecode::SET_PROP);
      bytecode(dest->get_index());
#ifdef DEBUG
    std::cout << source.get_name() << std::endl;
#endif
      bytecode(source.get_index());
      break;
    case Value::CLASS_PROPERTY:
#ifdef DEBUG
      std::cout << "BC: GETCATTR " << std::dec
    << (unsigned int)dest->get_index() << " ";
#endif
      // Assign to a class property.
      bytecode(meat::bytecode::SET_CLASS_PROP);
      bytecode(dest->get_index());
#ifdef DEBUG
    std::cout << source.get_name() << std::endl;
#endif
      bytecode(source.get_index());
      break;
    case Value::LOCAL_VARIABLE: // Should already be set to a local variable
      /*if (src->get_type() == Value::LOCAL_VARIABLE) {
#ifdef DEBUG
  std::cout << "BC: MOV " << dest->get_value() << " ";
#endif
  bytecode(meat::bytecode::ASSIGN);
  bytecode(dest->get_index());
  bytecode(source.get_index());
  }*/
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

Message::Message(ASTNode *who, const std::string &mesg) {
  this->who = who;
  method = mesg;

  add(who);
}

Message::~Message() throw() {
  delete who;

  std::deque<ASTNode *>::iterator param_it;
  for (param_it = params.begin(); param_it != params.end(); param_it++) {
    delete (*param_it);
  }
}

void Message::add_param(ASTNode *param) {
  params.push_back(param);

  add(param);
}

void Message::message_super(bool to_super) {
  super = to_super;
}

void Message::gen_bytecode(bool prelim) {
  LocalVariable who_var = who->gen_result(prelim);

  std::deque<LocalVariable> param_idxs;
  std::deque<ASTNode *>::iterator it;
  for (it = params.begin(); it != params.end(); it++) {

    if ((*it)->is_block() and it != params.begin()) {
      std::deque<ASTNode *>::iterator param_it = it;

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
      std::cout << "BC: SUPER " << who_var.get_name() << " " << method << " "
                << std::dec << (unsigned int)param_idxs.size();
#endif
      bytecode(meat::bytecode::MESG_SUPER);
    } else {
#ifdef DEBUG
      std::cout << "BC: MESSAGE " << who_var.get_name() << " "
                << method << " " << std::dec
                << (unsigned int)param_idxs.size();
#endif
      bytecode(meat::bytecode::MESSAGE);
    }

    bytecode(who_var.get_index());
    bytecode((uint32_t)hash(method.c_str()));
    bytecode((uint8_t)param_idxs.size());
    for (uint16_t c = 0; c < param_idxs.size(); c++) {
#ifdef DEBUG
      std::cout << " " << param_idxs[c].get_name();
#endif
      bytecode(param_idxs[c].get_index());
    }

#ifdef DEBUG
    std::cout << std::endl;
#endif
  }
}

LocalVariable Message::gen_result(bool prelim) {
  LocalVariable who_var = who->gen_result(prelim);

  std::deque<LocalVariable> param_idxs;
  std::deque<ASTNode *>::iterator it;
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
      std::cout << "BC: SUPRES " << who_var.get_name() << " "
                << result.get_name() << " " << method << " "
                << std::dec << (unsigned int)param_idxs.size();
#endif
      bytecode(meat::bytecode::MESG_SUPER_RESULT);
    } else {
#ifdef DEBUG
      std::cout << "BC: MSGRES " << who_var.get_name() << " "
                << result.get_name() << " " << method << " "
                << std::dec << (unsigned int)param_idxs.size();
#endif
      bytecode(meat::bytecode::MESG_RESULT);
    }

    bytecode(who_var.get_index());
    bytecode(result.get_index());
    bytecode((uint32_t)hash(method.c_str()));
    bytecode((uint8_t)param_idxs.size());

    for (uint16_t c = 0; c < param_idxs.size(); c++) {
#ifdef DEBUG
      std::cout << " " << param_idxs[c].get_name();
#endif
      bytecode(param_idxs[c].get_index());
    }

#ifdef DEBUG
    std::cout << std::endl;
#endif
  }

  return result;
}
