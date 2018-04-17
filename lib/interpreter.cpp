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

#include <meat/datastore.h>
#include <meat/utilities.h>
#include "compiler.h"

#ifdef DEBUG
#include <iostream>
#endif

extern "C" {
  DECLSPEC void *create_interpreter(const std::string &);
  DECLSPEC void exec_interpreter(void *, std::istream &);
  DECLSPEC void close_interpreter(void *);
}

void *create_interpreter(const std::string &name __attribute__((unused))) {
  meat::grinder::Interpreter *interp = new meat::grinder::Interpreter();
  return (void *)interp;
}

void exec_interpreter(void *interp, std::istream &code) {
  ((meat::grinder::Interpreter *)interp)->execute(code);
}

void close_interpreter(void *interp) {
  delete ((meat::grinder::Interpreter *)interp);
}


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

void meat::grinder::Interpreter::command() {
  std::string object, message;

#ifdef DEBUG
  std::cout << "INTERPRETER: " << tokens.to_string() << std::flush;
#endif
  if (tokens.expect(Token::EOL)) {
    tokens.next();
  } else if (tokens.expect(1, Token::WORD, "=")) {
    assignment();
  } else {
    this->message();
  }
}

/*******************************
 * Interpreter::resolve_object *
 *******************************/

meat::Reference meat::grinder::Interpreter::resolve_object(Token &token) {
  int32_t int_value;
  double flt_value;

#ifdef DEBUG
  std::cout << "INTERPRETER: Resolving token " << (std::string)token
            << std::endl;
#endif

  switch (token.type()) {
  case Token::WORD:
    if (Utils::is_integer(token, int_value)) {
#ifdef DEBUG
      std::cout << "             is integer" << std::endl;
#endif
      return new Value(int_value);
    } else if (Utils::is_float(token, flt_value)) {
#ifdef DEBUG
      std::cout << "             is float" << std::endl;
#endif
      return new Value(flt_value);
    } else if (variables.find(token) != variables.end()) {
#ifdef DEBUG
      std::cout << "             is variable" << std::endl;
#endif
      return variables[token];
    } else {
#ifdef DEBUG
      std::cout << "             is class" << std::endl;
#endif
      try {
        Reference result = meat::Class::resolve(((std::string &)token).c_str());
        return result;
      } catch (...) {
        throw meat::Exception(std::string("Symbol ") +
          ((std::string &)token).c_str() +
          " could not be resolved.");
      }
    }
  case Token::SUBST_STRING:
  case Token::LITRL_STRING:
#ifdef DEBUG
    std::cout << "              is string " << std::endl;
#endif
    return new Text((std::string &)token);
  default:
    throw meat::Exception(std::string("Symbol ") +
          ((std::string &)token).c_str() +
          " could not be resolved.");
  }
  return meat::Null();
}

/******************************************
 * meat::grinder::Interpreter::assignment *
 ******************************************/

void meat::grinder::Interpreter::assignment() {
  std::string name = (const std::string &)tokens[0];
  tokens.permit(Token::WORD);
  tokens.permit(Token::WORD, "=");

  Reference source;

  if (tokens.expect(Token::WORD)) {
    source = resolve_object(tokens[0]);
    tokens.next();
  } else if (tokens.expect(Token::COMMAND)) {
    tokens.push();
    source = message();
    tokens.pop();
  }

  variables[name] = source;
}

/***************************************
 * meat::grinder::Interpreter::message *
 ***************************************/

meat::Reference meat::grinder::Interpreter::message() {
  Reference object;

#ifdef DEBUG
  std::cout << "INTERPRETER: Message " << tokens.to_string() << std::flush;
#endif

  // Determine the object that we are messaging.
  if (tokens.expect(Token::COMMAND)) {
    tokens.push();
    object = message();
    tokens.pop();
  } else if (tokens.expect(Token::WORD)) {
    if (variables.find(tokens[0]) != variables.end()) {
      object = variables[tokens[0]];
      tokens.permit(Token::WORD);
    } else {
      try {
        object = meat::Class::resolve(((std::string)tokens[0]));
        tokens.permit(Token::WORD);
      } catch (...) {
        throw meat::Exception(std::string("Identifier ") +
          ((std::string)tokens[0]) +
          " could not be resolved as an object to message.");
      }
    }
  } else {
    throw Exception("Unable to resolve object to be messaged");
  }

  // Get the message.
  std::string method_name;
  std::deque<Reference> parameters;

  for (unsigned int c = 0; not tokens.expect(Token::EOL); ++c) {
    if (c % 2 == 0) {
      // Put together the method name.
      method_name += (std::string)tokens[0];
      tokens.permit(Token::WORD);

    } else {
      // Parameters
      if (tokens.expect(Token::WORD)) {
        parameters.push_back(resolve_object(tokens[0]));
        tokens.next();
      } else if (tokens.expect(Token::LITRL_STRING) or
                 tokens.expect(Token::SUBST_STRING)) {
        parameters.push_back(new meat::Text(tokens[0]));
        tokens.next();
      } else if (tokens.expect(Token::COMMAND)) {
        tokens.push();
        parameters.push_back(message());
        tokens.pop();
      } else if (tokens.expect(Token::BLOCK, "{")) {
        parameters.push_back(new meat::Text(tokens[0]));
        tokens.next();
      }

    }
  }

  // Clean up ending tokens.
  tokens.permit(Token::EOL);

  // Create the context and execute it.
  Reference new_context = meat::message(object, method_name, context);
  for (unsigned int idx = 0; idx < parameters.size(); ++idx) {
    cast<Context>(new_context).parameter(idx, parameters[idx]);
  }
  context = new_context;
  Reference result = meat::execute(context);
  context = cast<Context>(context).messenger();
  return result;
}
