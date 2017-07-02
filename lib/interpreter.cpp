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

#include <meat/compiler.h>
#include <meat/datastore.h>
#include <meat/utilities.h>

#ifdef DEBUG
#include <iostream>
#endif

/******************************************************************************
 * Interpreter Class Implementation
 */

/****************************
 * Interpreter::Interpreter *
 ****************************/

meat::grinder::Interpreter::Interpreter() : context(meat::Null()) {
}

/*****************************
 * Interpreter::~Interpreter *
 *****************************/

meat::grinder::Interpreter::~Interpreter() throw() {
}

/************************
 * Interpreter::command *
 ************************/

void meat::grinder::Interpreter::command(Tokenizer &tokens) {

  std::string object, message;

#ifdef DEBUG
  std::cout << "COMMAND: " << tokens.to_string() << std::endl;
#endif

  if (tokens.count() == 3 and tokens[1] == "=") {
    // Found assignment statement

#ifdef DEBUG
    std::cout << "DEBUG: Assignment" << std::endl;
#endif
    if (tokens[0].type() != Token::WORD) {
      throw Exception("Can only assign to a variable");
    }

    variables[tokens[0]] = resolve_object(tokens[2]);
  } else {
    Reference obj = resolve_object(tokens[0]);

#ifdef TESTING
    if (obj.is_null()) {
      throw Exception("Can't message a null object");
    }
#endif

    if (tokens.count() == 2) {
      if (tokens[1].is_type(Token::WORD)) {
				message = (std::string &)(tokens[1]);
      }

#ifdef DEBUG
      std::cout << "DEBUG: message \"" << message << "\"" << std::endl;
#endif

      context = meat::message(obj, message.c_str(), context);
      result = meat::execute(context);
      context = ((Context &)(*context)).get_messenger();

#ifdef DEBUG
      if (not result.is_null())
				std::cout << "       got result" << std::endl;
#endif

    } else if (tokens.count() > 2 && tokens.count() % 2 == 1) {
      /* Build the message name. */
      for (unsigned int c = 1; c < tokens.count(); c += 2) {
				message += (std::string &)(tokens[c]);
      }

#ifdef DEBUG
      std::cout << "DEBUG: message \"" << message << "\"" << std::endl;
#endif

      /* Create the new context. */
      Reference new_ctx = meat::message(obj, hash(message.c_str()), context);

      /* Add the parameters */
      uint32_t c;
      uint8_t param;
      for (c = 2, param = 0; c < tokens.count(); c += 2, param++) {
        Reference value = resolve_object(tokens[c]);
        CONTEXT(new_ctx).set_param(param, value);
      }

      context = new_ctx;

      result = meat::execute(context);
      context = CONTEXT(context).get_messenger();

#ifdef DEBUG
      if (not result.is_null())
        std::cout << "       got result" << std::endl;
#endif

    } else {
      //std::cerr << "ERROR: Syntax error" << std::endl;
      throw Exception("Invalid message syntax");
    }
  }
#ifdef DEBUG
  //std::cout << "DEBUG:  clear " << object << " " << message << std::endl;
#endif /* DEBUG */
  tokens.clear();
}

/***********************
 * Interpreter::create *
 ***********************/

void meat::grinder::Interpreter::create(const std::string &name) {
  archive = new meat::data::Archive(name.c_str(), true);
  variables["archive"] = archive;
}

/**********************
 * Interpreter::write *
 **********************/

void meat::grinder::Interpreter::write() {
  if (archive != NULL) archive->sync();
}

/*******************************
 * Interpreter::resolve_object *
 *******************************/

meat::Reference meat::grinder::Interpreter::resolve_object(Token &token) {
  int32_t int_value;
  float_t flt_value;

#ifdef DEBUG
  std::cout << "COMPILER: Resolving token " << (std::string &)token
      << std::endl;
#endif

  switch (token.type()) {
  case Token::WORD:
    if (Utils::is_integer(token, &int_value)) {
#ifdef DEBUG
      std::cout << "          is integer" << std::endl;
#endif
      return new Object(int_value);
    } else if (Utils::is_float(token, &flt_value)) {
#ifdef DEBUG
      std::cout << "          is float" << std::endl;
#endif
      return new Object(flt_value);
    } else if (variables.find(token) != variables.end()) {
#ifdef DEBUG
      std::cout << "          is variable" << std::endl;
#endif
      return variables[token];
    } else {
#ifdef DEBUG
      std::cout << "          is class" << std::endl;
#endif
      try {
        Reference result = meat::Class::resolve(((std::string &)token).c_str());
        return result;
      } catch (...) {
        throw meat::Exception(std::string("Symbol ") +
          ((std::string &)token).c_str() +
          " was not found");
      }
    }
  case Token::SUBST_STRING:
  case Token::LITRL_STRING:
#ifdef DEBUG
    std::cout << "          is string " << std::endl;
#endif
    return new Object((std::string &)token);
  case Token::COMMAND: {
#ifdef DEBUG
    std::cout << "          is command " << std::endl;
#endif
    Tokenizer cmd_parser;
    cmd_parser.parse(token);
    command(cmd_parser);
#ifdef DEBUG
    std::cout << "COMPILER: Command [" << (std::string &)token << "] is done"
        << std::endl;
#endif
#ifdef TESTING
    if (result.is_null())
      throw Exception("Command is missing result");
#endif
    return result;
  }
  }
  return meat::Null();
}
