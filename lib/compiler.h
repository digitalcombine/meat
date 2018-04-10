/*                                                                  -*- c++ -*-
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

#include <meat/types.h>
#include <meat/datastore.h>
#include "ast.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <deque>
#include <set>
#include <stack>

#ifndef _MEAT_COMPILER_H
#define _MEAT_COMPILER_H

/** @ns */
namespace meat {

  /** @ns */
  namespace grinder {

    /** Initialize the Meat compiler.
     */
    DECLSPEC void initialize();

    class Class;
    class MethodBuilder;

    /** Parser location tracking.
     * @todo Finish this class and use it.
     */
    class DECLSPEC Location {
    public:
      Location();
      Location(const Location &other);
      virtual ~Location() throw();

      void inc_line();
      void inc_offset(std::uint32_t amount = 1);
      void offset(std::uint32_t position);
      void reset();
      void rewind();

      Location &operator =(const Location &other);

      operator std::string() const;

      friend
        std::ostream &operator << (std::ostream &out, const Location &value);

    private:
      unsigned int _line;
      unsigned int _offset;
    };

    std::ostream &operator << (std::ostream &out, const Location &value);

    /** Class for a single token used for parsing during compiling.
     */
    class DECLSPEC Token {
    public:
      /** Enum of the possible types of tokens that will be parsed.
       */
      typedef enum {
        WORD         = 0x00,
        SUBST_STRING = 0x01,
        LITRL_STRING = 0x02,
        INTEGER      = 0x03,
        NUMBER       = 0x04,
        COMMAND      = 0x05,
        BLOCK        = 0x06,
        EOL          = 0xff
      } token_t;

      Token(const std::string &token, token_t value_type,
            const Location &position);

      /** Default copy constructor.
       */
      Token(const Token &other);

      /** Destructor.
       */
      virtual ~Token() throw();

      /** Expands anything that requires substitution in literal or
       * substitution strings. This is safe to call on all token types.
       */
      void subst();

      /** Returns the type of token.
       * @see token_t
       */
      token_t type() const;

      /** Test if the token is of type.
       */
      bool is_type(token_t value_type) const;

      const Location &position() const;

      /** Assign the values of another token to this token.
       */
      Token &operator = (const Token &other);

      /** Test if the token is equal to a constant string.
       * @param value The constant string to compare the token to.
       * @return True if the token is equal to value, otherwise return false.
       */
      bool operator == (const std::string &value) const;

      /** Cast the token as a constant string.
       */
      operator std::string () const;

      /** Convert the token to a double if possible. If the token could not
       * be converted then 0.0f is returned.
       */
      operator std::int32_t () const;

      /** Convert the token to a double if possible. If the token could not
       * be converted then 0.0f is returned.
       */
      operator double () const;

    private:
      token_t value_type; /**< The type of token parsed */
      std::string value;  /**< The value of the token. */
      std::int32_t i_value;
      double d_value;
      Location _position; /**< Location of the token in the original text. */
    };

    /**
     */
    class DECLSPEC SyntaxException : public meat::Exception {
    public:
      SyntaxException(const Token &token, const std::string &message);
      virtual ~SyntaxException() throw();

      unsigned int line() const;
      unsigned int character() const;

    private:
      Token token;
    };

    /** Collection of tokens parsed from a given command or text.
     */
    class DECLSPEC Tokenizer {
    public:
      Tokenizer();
      virtual ~Tokenizer() throw();

      /** Parse the code into tokens passing each command to Language::command.
       */
      void parse(std::istream &code);

      void parse(const std::string &code);

      /** Push the current token for futher parsing, in cases of subcommands or
       * code blocks.
       * @param token The token to continue parsing.
       */
      void push();

      void pop();

      /** Return the number of tokens that have been currently parsed.
       * @return The number of tokens currently parsed.
       */
      size_t count();

      /** Returns true if the parsed tokens are possibly a complete command.
       * This only means that all brackets and quotes are closed and the
       * end of the line was found. It does not determine if the command
       * is syntactically correct.
       *
       * @return True if the command is complete, false if more needs to be
       *         parsed.
       */
      bool is_complete();

      bool expect(Token::token_t id);

      bool expect(Token::token_t id, const std::string &value);

      bool expect(size_t index, Token::token_t id);

      bool expect(size_t index, Token::token_t id, const std::string &value);

      void permit(Token::token_t id);

      void permit(Token::token_t id, const std::string &value);

      void next();

      /** Clear all the currently parsed tokens for parsing new commands.
       * @see parse
       */
      void clear();

      bool is_more();

      std::string to_string() const;

      /** Retrieve a token.
       * @throws out_of_range If the index is beyond the number of tokens.
       * @see count
       */
      Token &operator[] (size_t index);

    protected:
      void get_next_line();

      std::string remaining;
      std::istream *stream;

      std::deque<Token> tokens;

      bool get_uchar(std::string &destination);
      void get_line(std::string &destination);
      void parse_line(const std::string &command, bool more = false);

    private:
      bool complete;

      std::istringstream strs;

      //Location start;
      Location position;

      /** Flags get_next_line to do some of its own parsing, dropping comments
       * and trimming white space, otherwise it returns the next line as is.
       */
      bool cook_lines;
      bool cont_line;

      struct stack_s {
        std::deque<Token> tokens;
        std::string remaining;
        std::istream *stream;
        Location position;
      };
      std::stack<stack_s> states;
    };

    /** Base class for creating language structures.
     */
    class DECLSPEC Grammer {
    public:
      /** Initialize a new Language object.
       */
      Grammer();

      /** Cleanup a Language object.
       */
      virtual ~Grammer() throw();

      /** Parse the code into tokens passing each command to Language::command.
       */
      void execute(std::istream &code);

      void execute(const std::string &code);

      virtual void command();

    protected:
      Tokenizer tokens;

      void reset();

    private:
      Location position;
      unsigned long curr_line;
    };

    /**************************************************************************
     */

    class DECLSPEC Interpreter : public Grammer {
    public:
      Interpreter();
      virtual ~Interpreter() throw();

      virtual void command();

    protected:
      Reference resolve_object(Token &token);

      void assignment();
      Reference message();

    private:
      std::map<std::string, Reference> variables;

      Reference context;
      Reference result;
    };

    /**************************************************************************
     * Library Class
     */

    /** Language parser for compiling library files.
     */
    class DECLSPEC Library : public Grammer, public Object,
                             public meat::CompilerInterface {
    public:
      /** Construct a new Library object.
       */
      Library(Reference klass, uint8_t properties);

      /** Clean up a Library object.
       */
      virtual ~Library() throw();

      void register_as(const std::string &name);

      void add_class(Reference klass);

      /** Interpret a parsed command already tokenized.
       */
      virtual void command();

      void add_symbol(const std::string &symbol);
      void clear_symbols();

      virtual void import(const std::string &library, meat::Reference context);
      virtual void include(const std::string &code);
      virtual void create_class(meat::Reference super,
                                const std::string &cls_name,
                                const std::string &cls_body,
                                meat::Reference context);
      virtual void set_application_class(meat::Reference klass);

      void compile();

      /** Actually create the new library file.
       * XXX Rename to compile
       */
      void write();

      virtual void unserialize(data::Archive &store,
                               std::istream &data_stream);

      friend class Class;

    protected:
      bool is_cpp() const;

      meat::Reference message();

    private:
      data::Library *library;
      std::set<std::string> symbols;

      bool to_cpp;
      Reference context;
      Reference result;

      void write_mlib(std::ostream &out);
      void write_cpp(std::ostream &out);
    };

    /**************************************************************************
     * Property 0 Class Name
     * Property 1 List of Properties
     * Property 2 List of Class Properties
     * Property 3 Index of Methods
     * Property 4 Index of Class Methods
     * Property 5 Constructor
     */

    class DECLSPEC Class : public Grammer, public Object {
    public:
      Class(Reference klass, std::uint8_t properties);
      virtual ~Class() throw() {};

      void add_method(Reference method);
      void add_class_method(Reference method);

      Reference super() const {
        return meat::Class::resolve(cast<const Text>(property(1)));
      }

      /** Creates the actual class within the Meat engine.
       * XXX Rename to compile
       */
      void create_class() const;

      bool is_cpp() const;

      void cpp_methods(std::ostream &out);
      void cpp_new_class(std::ostream &out) const;


      virtual void command();

      virtual void unserialize(data::Archive &store,
                               std::istream &data_stream);

      friend class Library;

    protected:

      void property_def();
      void method_def(bool is_native = false);

    private:
      Library *library;
      uint16_t cpp_bytecode;
      uint8_t m_count;
      uint8_t cm_count;

      uint8_t method_count() const;
      uint8_t class_method_count() const;
      uint8_t obj_property(const std::string &name);
      uint8_t cls_property(const std::string &name);
      int16_t have_obj_property(const std::string &name) const;
      int16_t have_cls_property(const std::string &name) const;
      uint8_t constant(const std::string name);
      void update_symbols(std::set<std::string> &symbols) const;
      std::string cpp_hash_id() const;
    };

    /**************************************************************************
     * Property 0 Method Name
     * Property 1 Parameters
     * Property 2 Body
     */

    class DECLSPEC Method : public Grammer, public Object {
    public:
      Method(Reference klass, std::uint8_t properties);
      Method(Class &cb, bool is_cpp = false);
      virtual ~Method() throw() {};

      virtual void command();

      /** Compile the method into bytecode.
       */
      void compile();

      /** Compile C++ code
       */
      void cpp_method(const std::string &prelim, std::ostream &out);

      void gen_bytecode(std::vector<uint8_t> &bytecode);
      uint8_t locals() { return _locals; }

      void is_cpp(bool value) { _is_cpp = value; }
      bool is_cpp() const { return _is_cpp; }

      friend class Class;

    protected:
      ast::Message *message();
      ast::Assignment *assignment();
      ast::ContextBlock *block();
      ast::Constant *constant();

    private:
      /** Reference to the class that contains the method */
      Class *cb;
      ast::Block *astree;

      bool _is_cpp;
      std::vector<std::uint8_t> bytecode;
      uint8_t _locals;

      std::set<std::string> symbols;

      std::string cpp_hash_id();
      void update_symbols(std::set<std::string> &symbols) const;
      std::string cpp_name(const std::string &prelim);
      void add_parameter(const std::string &name);
      void add_body(const std::string &body);
    };

    namespace Utils {
      DECLSPEC bool is_integer(const std::string &value, std::int32_t &res);
      DECLSPEC bool is_float(const std::string &value, double &res);
    }

  } /* namespace compiler */
} /* namespace meat */

#endif /* _MEAT_COMPILER_H */
