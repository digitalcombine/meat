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

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <deque>
#include <set>

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

			void set_source(std::string &source_name);
			void inc_line();
			void inc_offset(uint32_t amount = 1);
			void set_offset(uint32_t position);

			Location &operator =(const Location &other);

		private:
			std::string source;

			uint32_t line;
			uint32_t offset;
		};

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
				COMMAND      = 0x03,
				BLOCK        = 0x04,
        TEOF         = 0xff
			} token_t;

			Token(const std::string &token, token_t value_type,
						Location &start, Location &end);

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
			token_t type() const { return value_type; };

			bool is_type(token_t value_type) const {
				return (this->value_type == value_type);
			};

			const Location &get_start_location() const { return start; };

			/** Assign the values of another token to this token.
			 */
			Token &operator = (const Token &other);

			/** Test if the token is equal to a constant string.
			 * @param value The constant string to compare the token to.
			 * @return True if the token is equal to value, otherwise return false.
			 */
			bool operator == (const char *value) const;

			/** Cast the token as a constant string.
			 */
			operator const std::string &() const { return value; };

			/** Cast the token as a string.
			 */
			operator std::string &() { return value; };

			/** Convert the token to a float_t if possible. If the token could not
			 * be converted then 0.0f is returned.
			 */
			operator float_t ();

		private:
			token_t value_type;  /**< The type of token parsed */
			std::string value;   /**< The value of the token. */
			Location start, end; /**< Location of the token in the original text. */
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

			void parse(const Location &location, const std::string &command,
								 bool more = false);

			/** Further parse a token, in cases of subcommands or code blocks.
			 * @param token The token to continue parsing.
			 */
			void parse(const Token &token);

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

      bool expect(Token::token_t id, const char *value);

      void permit(Token::token_t id);

      void permit(Token::token_t id, const char *value);

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

		private:
			bool complete;
			std::string remaining;

      std::istringstream strs;
      std::istream *code;

			Location start;
      Location position;
      bool cook_lines;

			std::deque<Token> tokens;
		};

		/** Base class for creating language structures.
		 */
		class DECLSPEC Language {
		public:
			/** Initialize a new Language object.
			 */
			Language();

			/** Cleanup a Language object.
			 */
			virtual ~Language() throw();

			/** Parse the code into tokens passing each command to Language::command.
			 */
			void execute(std::istream &code);

      void execute(const std::string &code);

			/** Parse the code into tokens passing each command to Language::command.
			 */
			void execute(const Token &token);

			virtual void command(Tokenizer &tokens);

		protected:
			void reset();

		private:
			Location position;
			unsigned long curr_line;
		};

		/**************************************************************************
		 */

		class DECLSPEC Interpreter : public Language {
		public:
			Interpreter();
			virtual ~Interpreter() throw();

			virtual void command(Tokenizer &tokens);

			void create(const std::string &name);

			void write();

		protected:
			Reference resolve_object(Token &token);

		private:
			std::map<std::string, Reference> variables;
			data::Archive *archive;

			Reference context;
			Reference result;
		};

		/**************************************************************************
		 * Library Class
		 *
		 * Property 0 Library Name
		 * Property 1 List of Classes
		 */

		/** Language parser for compiling library files.
		 */
		class DECLSPEC Library : public Language, public Object {
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
			virtual void command(Tokenizer &tokens);

			void add_import(const std::string &name);
			void remove_import(const std::string &name);

			Reference get_imports() const;

			void set_application(const std::string &name);

			void add_symbol(const std::string &symbol);
			void clear_symbols();

			/** Actually create the new library file.
			 * XXX Rename to compile
			 */
			void write();

			virtual void unserialize(data::Archive &store,
															 std::istream &data_stream);

			friend class Class;

		protected:
			bool is_cpp() const;

		private:
			data::Library *library;
			std::set<std::string> symbols;

			bool to_cpp;
			Reference context;
			Reference result;
		};

		/**************************************************************************
		 * Property 0 Class Name
		 * Property 1 List of Properties
		 * Property 2 List of Class Properties
		 * Property 3 Index of Methods
		 * Property 4 Index of Class Methods
     * Property 5 Constructor
		 */

		class DECLSPEC Class : public Language, public Object {
		public:
			Class(Reference klass, uint8_t properties);
			virtual ~Class() throw() {};

			uint8_t obj_property(const std::string &name);
			uint8_t cls_property(const std::string &name);
			int16_t have_obj_property(const std::string &name) const;
			int16_t have_cls_property(const std::string &name) const;
      Reference get_super() const {
				return meat::Class::resolve(CONST_TEXT(property(1)));
			}

			void add_method(Reference method);
			void add_class_method(Reference method);

			uint8_t constant(const std::string name);

			/** Creates the actual class within the Meat engine.
			 * XXX Rename to compile
			 */
			void create_class() const;

			bool is_cpp() const;

			std::string cpp_methods();
			std::string cpp_new_class() const;
			std::string cpp_hash_id() const;

			virtual void command(Tokenizer &tokens);

			void update_symbols(std::set<std::string> &symbols) const;

			virtual void unserialize(data::Archive &store,
															 std::istream &data_stream);

			friend class Library;

		private:
			Library *library;
			uint16_t cpp_bytecode;
			uint8_t m_count;
			uint8_t cm_count;

			uint8_t method_count() const;
			uint8_t class_method_count() const;
		};

		/**************************************************************************
		 * Property 0 Method Name
		 * Property 1 Parameters
		 * Property 2 Body
		 */

		class DECLSPEC Method : public Language, public Object {
		public:
			Method(Reference klass, uint8_t properties);
			Method(Class &cb, bool is_cpp = false);
			virtual ~Method() throw() {};

			virtual void command(Tokenizer &tokens);

			void add_parameter(const std::string &name);
			void add_body(const std::string &body);

			/** Compile the method into bytecode.
			 */
			void compile();

			void update_symbols(std::set<std::string> &symbols) const;

			std::string cpp_hash_id();

			std::string cpp_name(const char *prelim);

			/** Compile C++ code
			 */
			std::string cpp_method(const char *prelim);

			void gen_bytecode(std::vector<uint8_t> &bytecode);
			uint8_t local_count() { return locals; }

			void is_cpp(bool value) { _is_cpp = value; }
			bool is_cpp() const { return _is_cpp; }

			friend class Class;

		protected:
			void *parse_message(Tokenizer &tokens);

		private:
			/** Reference to the class that contains the method */
			Class *cb;
			void *astree;

			bool _is_cpp;
			std::vector<uint8_t> bytecode;
			uint8_t locals;

			std::set<std::string> symbols;
		};

		namespace Utils {
			DECLSPEC bool is_integer(const std::string &value, int32_t *res);
			DECLSPEC bool is_float(const std::string &value, float_t *res);
		};

	}; /* namespace compiler */
}; /* namespace meat */

#endif /* _MEAT_COMPILER_H */
