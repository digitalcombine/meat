/*                                                                  -*- c++ -*-
 * Abstract Syntax Tree
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

#include <meat.h>

#ifndef _MEAT_AST_H
#define _MEAT_AST_H

/** @ns */
namespace meat {

  /** @ns */
  namespace grinder {

		namespace ast {

			class LocalVariable;
			class Method;
			class Block;

			/**
			 */
			class LocalVariable {
			public:
				LocalVariable();
				LocalVariable(const std::string &name, Method &method_node);
				LocalVariable(const std::string &name, Block &block_node);
				LocalVariable(const LocalVariable &other);

				LocalVariable &operator = (const LocalVariable &other);

				uint8_t get_index() const;
				const std::string &get_name() const { return name; }

			private:
				std::string name;
				Method *method;  // The method context of the variable.
				Block *block;    // The block context of the variable if there is one.
			};

			/**
			 */
			class ASTNode {
			public:
				ASTNode();
				virtual ~ASTNode() throw();

				virtual void gen_bytecode(bool prelim) = 0;
				virtual LocalVariable gen_result(bool prelim) = 0;

				virtual bool is_value() { return false; }
				virtual bool is_block() { return false; }

				virtual uint8_t locals() const;
				virtual LocalVariable add_local(const std::string &name);
				virtual LocalVariable add_temp_local();

				virtual void add(ASTNode *node);

				/** To have any access to a value it needs to be set in a local variable.
				 * This includes object properties and message results. To create
				 * efficient byte code, here we can set the local variable destination
				 * for our value instead of having the AST nodes first creating
				 * temperary local variables for the values.
				 *
				 */
				void set_result_dest(LocalVariable dest);
				void set_result_dest();
				LocalVariable get_result_dest();

			protected:
				ASTNode *container;

				bool result_set;
				LocalVariable result;

				virtual int16_t resolve_property(const std::string &name) const;
				virtual int16_t resolve_class_property(const std::string &name) const;
				virtual LocalVariable resolve_local(const std::string &name) const;

				/** Returns the offset of the next bytecode that will be inserted.
				 */
				virtual uint16_t bytecode();
				virtual void bytecode(uint8_t value);
				virtual void bytecode(uint16_t value);
				virtual void bytecode(uint16_t marker, uint16_t value);
				virtual void bytecode(uint32_t value);
				virtual void bytecode(int32_t value);
				virtual void bytecode(float_t value);
			};

			/**
			 */
			class Method : public ASTNode {
			public:
				Method(const meat::List &properties, int p_offset,
							 const meat::List &class_properties, int cp_offset);
				virtual ~Method() throw();

				virtual void add(ASTNode *command);

				uint8_t add_parameter(const std::string &name);

				uint8_t local_count() const;

				virtual void gen_bytecode(bool prelim);
				virtual LocalVariable gen_result(bool prelim);

				virtual uint8_t locals() const;
				virtual LocalVariable add_local(const std::string &name);
				virtual LocalVariable add_temp_local();

				void append_bytecode(std::vector<uint8_t> &bc);

				friend class LocalVariable;

			protected:
				virtual int16_t resolve_property(const std::string &name) const;
				virtual int16_t resolve_class_property(const std::string &name) const;
				virtual LocalVariable resolve_local(const std::string &name) const;

				virtual uint16_t bytecode();
				virtual void bytecode(uint8_t value);
				virtual void bytecode(uint16_t value);
				virtual void bytecode(uint16_t marker, uint16_t value);
				virtual void bytecode(uint32_t value);
				virtual void bytecode(int32_t value);
				virtual void bytecode(float_t value);

			private:
				std::deque<std::string> properties;
				int p_offset;
				std::deque<std::string> class_properties;
				int cp_offset;

				std::deque<std::string> local_names;
				std::deque<ASTNode *> commands;

				std::vector<uint8_t> bc;

				uint8_t temp_counter;
			};

			class Block : public ASTNode {
			public:
				Block();
				virtual ~Block() throw();

				virtual void add(ASTNode *command);

				void set_parent_block(Block *parent);

				virtual void gen_bytecode(bool prelim);
				virtual LocalVariable gen_result(bool prelim);

				virtual bool is_block() { return true; }

				virtual uint8_t locals() const;
				virtual LocalVariable add_local(const std::string &name);
				virtual LocalVariable add_temp_local();

				friend class LocalVariable;

			protected:
				virtual LocalVariable resolve_local(const std::string &name) const;

				uint8_t get_layer() const;
				uint8_t local_offset() const;
				uint8_t local_count() const;

			private:
				std::deque<std::string> local_names;
				std::deque<ASTNode *> commands;

				LocalVariable local_var;

				Method *top_level;
				Block *parent;

				uint8_t temp_counter;
			};

			/** Represents a value with in the interpreter. The value can be a
			 * constant, local variable, code block or an object property.
			 */
			class Value : public ASTNode {
			public:
				typedef enum {
					UNKNOWN,
					STRING_CONST,
					FLOAT_CONST,
					INT_CONST,
					LOCAL_VARIABLE,
					BLOCK_PARAMETER,
					OBJECT_PROPERTY,
					CLASS_PROPERTY,
					CLASS_REFERENCE
				} value_type_t;

				Value(const std::string &value, bool string_constant = false);
				virtual ~Value() throw();

				value_type_t get_type() const;
				uint8_t get_index() const { return index; }

				/** Resolves the value to...
				 */
				void resolve();

				virtual void gen_bytecode(bool prelim);
				virtual LocalVariable gen_result(bool prelim);

				virtual bool is_value() { return true; }

				const std::string &get_value() const { return value; }

				void make_local();
				void make_block_param(Block *block);

			private:
				value_type_t vtype;
				std::string value;

				//LocalVariable destination;
				LocalVariable block_param;

				uint8_t index;
				int32_t int_value;
				float_t flt_value;
				//std::set<std::string> &symbols;
			};

			class Assignment : public ASTNode {
			public:
				Assignment(Value *dest, ASTNode *src);
				virtual ~Assignment() throw();

				virtual void gen_bytecode(bool prelim);
				virtual LocalVariable gen_result(bool prelim);

			private:
				Value *dest;
				ASTNode *src;
			};

			class Message : public ASTNode {
			public:
				Message(ASTNode *who, const std::string &mesg);
				virtual ~Message() throw();

				void add_param(ASTNode *param);

				void message_super(bool to_super);

				virtual void gen_bytecode(bool prelim);
				virtual LocalVariable gen_result(bool prelim);

			private:
				ASTNode *who;
				std::string method;
				std::deque<ASTNode *> params;

				bool super;
			};

		}; /* namespace ast */
  }; /* namespace compiler */
}; /* namespace meat */

#endif /* _MEAT_AST_H */
