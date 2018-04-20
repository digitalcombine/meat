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

    /** @ns */
    namespace ast {

      class Block;

      /**
       */
      class LocalVariable {
      public:
        LocalVariable();
        LocalVariable(const std::string &name, Block &block_node);
        LocalVariable(const LocalVariable &other);

        LocalVariable &operator = (const LocalVariable &other);

        std::uint8_t index() const;
        const std::string &name() const { return _name; }

      private:
        std::string _name;
        Block *_block;    // The block context of the variable if there is one.
      };

      /**
       */
      class Node {
      public:
        Node();
        virtual ~Node() throw();

        virtual void gen_bytecode(bool prelim) = 0;
        virtual LocalVariable gen_result(bool prelim) = 0;

        virtual bool is_value() { return false; }
        virtual bool is_block() { return false; }

        virtual std::uint8_t locals() const;
        virtual LocalVariable local(const std::string &name);
        virtual LocalVariable anon_local();

        virtual Block &scope() const;
        virtual void scope(Block *block);

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
        Block *_scope;

        bool result_set;
        LocalVariable result;

        virtual std::int16_t resolve_property(const std::string &name) const;
        virtual std::int16_t
        resolve_class_property(const std::string &name) const;
        virtual LocalVariable resolve_local(const std::string &name) const;

        virtual void add_symbol(const std::string &name);

        /** Returns the offset of the next bytecode that will be inserted.
         */
        virtual std::uint16_t bytecode();
        virtual void bytecode(std::uint8_t value);
        virtual void bytecode(std::uint16_t value);
        virtual void bytecode(std::uint16_t marker, std::uint16_t value);
        virtual void bytecode(std::uint32_t value);
        virtual void bytecode(std::int32_t value);
        virtual void bytecode(double value);
      };

      /**
       */
      class Block : public Node {
      public:
        virtual ~Block() throw();

        /** Add an ast node to the block. The scope for the node will also be
         * be set in the node as it's added.
         */
        virtual void add(Node *node);

        /**
         */
        std::uint8_t locals() const;

        /** Add a local variable to the block.
         */
        LocalVariable local(const std::string &name);

        /** Creates an anonymous local variable, usually needed when a command
         * is passed as a parameter.
         */
        virtual LocalVariable anon_local();

        friend class LocalVariable;

      protected:
        std::deque<std::string> _locals;
        std::deque<Node *> _nodes;
        unsigned int temp_counter;

        Block();

        std::uint8_t locals_offset() const;
      };

      /**
       */
      class Method : public Block {
      public:
        Method(const meat::List &properties, int p_offset,
               const meat::List &class_properties, int cp_offset);
        virtual ~Method() throw();

        std::uint8_t add_parameter(const std::string &name);

        virtual void gen_bytecode(bool prelim);
        virtual LocalVariable gen_result(bool prelim);

        virtual void add_symbol(const std::string &name);
        void update_symbols(std::set<std::string> &symbols);

        void append_bytecode(std::vector<std::uint8_t> &bc);

        friend class LocalVariable;

      protected:
        virtual std::int16_t resolve_property(const std::string &name) const;
        virtual std::int16_t resolve_class_property(const std::string &name) const;
        virtual LocalVariable resolve_local(const std::string &name) const;

        virtual std::uint16_t bytecode();
        virtual void bytecode(std::uint8_t value);
        virtual void bytecode(std::uint16_t value);
        virtual void bytecode(std::uint16_t marker, uint16_t value);
        virtual void bytecode(std::uint32_t value);
        virtual void bytecode(std::int32_t value);
        virtual void bytecode(double value);

      private:
        std::deque<std::string> _properties;
        int _p_offset;
        std::deque<std::string> _class_properties;
        int _cp_offset;

        std::vector<std::uint8_t> _bytecode;
        std::set<std::string> _symbols;
      };

      /**
       */
      class ContextBlock : public Block {
      public:
        ContextBlock();
        virtual ~ContextBlock() throw();

        virtual void gen_bytecode(bool prelim);
        virtual LocalVariable gen_result(bool prelim);

        virtual bool is_block() { return true; }

        virtual std::uint8_t locals() const;
        virtual LocalVariable anon_local();

      protected:
        virtual LocalVariable resolve_local(const std::string &name) const;

        std::uint8_t local_offset() const;
        std::uint8_t local_count() const;

      private:
        std::deque<std::string> _local_names;

        LocalVariable _local_var;

        //std::uint8_t _temp_counter;
      };

      /**
       */
      class Identifier : public Node {
      public:
        typedef enum {
          UNKNOWN,
          LOCAL_VARIABLE,
          BLOCK_PARAMETER,
          OBJECT_PROPERTY,
          CLASS_PROPERTY,
          CLASS_REFERENCE
        } ident_type_t;

        Identifier(const std::string &value);
        virtual ~Identifier() throw();

        void new_local();
        void block_parameter(Block *block);

        virtual bool is_value() { return true; }

        ident_type_t type() const { return _type; }
        std::uint8_t index() const { return _index; }

        virtual void gen_bytecode(bool prelim);
        virtual LocalVariable gen_result(bool prelim);

      private:
        ident_type_t _type;

        std::string _name;
        std::uint8_t _index;
        LocalVariable _parameter;
      };

      /** Represents a constant value.
       */
      class Constant : public Node {
      public:
        Constant(const std::string &value);
        Constant(std::int32_t value);
        Constant(double value);
        virtual ~Constant() throw();

        virtual void gen_bytecode(bool prelim);
        virtual LocalVariable gen_result(bool prelim);

      private:
        enum {
          TEXT, INTEGER, NUMBER
        } _type;

        std::string _text;
        union {
          std::int32_t integer;
          double number;
        } _value;
      };

      /**
       */
      class Assignment : public Node {
      public:
        Assignment(Identifier *dest, Node *src);
        virtual ~Assignment() throw();

        virtual void scope(Block *block);

        virtual void gen_bytecode(bool prelim);
        virtual LocalVariable gen_result(bool prelim);

      private:
        Identifier *_destination;
        Node *_source;
      };

      /**
       */
      class Message : public Node {
      public:
        Message(Node *who);
        Message(Node *who, const std::string &name);
        virtual ~Message() throw();

        void method(const std::string &name);

        void add_param(Node *param);

        void message_super(bool to_super);

        virtual void scope(Block *block);

        virtual void gen_bytecode(bool prelim);
        virtual LocalVariable gen_result(bool prelim);

      private:
        Node *_who;
        std::string _method;
        std::deque<Node *> _parameters;

        bool _super;
      };

    } /* namespace ast */
  } /* namespace grinder */
} /* namespace meat */

#endif /* _MEAT_AST_H */
