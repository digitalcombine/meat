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

#include "compiler.h"

#include <cstdlib>
#ifdef TESTING
#include <testing.h>
#endif

/******************************************************************************
 * meat::grinder::Location Class
 */

/*************************************
 * meat::grinder::Location::Location *
 *************************************/

meat::grinder::Location::Location()
  : _line(0), _offset(1) {
}

meat::grinder::Location::Location(const Location &other)
  : _line(other._line), _offset(other._offset) {
}

/**************************************
 * meat::grinder::Location::~Location *
 **************************************/

meat::grinder::Location::~Location() throw() {
}

/*************************************
 * meat::grinder::Location::inc_line *
 *************************************/

void meat::grinder::Location::inc_line() {
  ++_line;
  _offset = 1;
}

/***************************************
 * meat::grinder::Location::inc_offset *
 ***************************************/

void meat::grinder::Location::inc_offset(uint32_t amount) {
  _offset += amount;
}

/***********************************
 * meat::grinder::Location::offset *
 ***********************************/

void meat::grinder::Location::offset(uint32_t position) {
  _offset = position;
}

/**********************************
 * meat::grinder::Location::reset *
 **********************************/

void meat::grinder::Location::reset() {
  _line = 0;
  _offset = 1;
}

/***********************************
 * meat::grinder::Location::rewind *
 ***********************************/

void meat::grinder::Location::rewind() {
  --_line;
}

/***************************************
 * meat::grinder::Location::operator = *
 ***************************************/

meat::grinder::Location &
meat::grinder::Location::operator =(const Location &other) {
  if (&other != this) {
    _line = other._line;
    _offset = other._offset;
  }

  return *this;
}

/***************************************
 * meat::grinder::operator std::string *
 ***************************************/

meat::grinder::Location::operator std::string() const {
  std::stringstream result;
  result << (unsigned int)_line << ":" << (unsigned int)_offset;
  return result.str();
}

/******************************
 * meat::grinder::operator << *
 ******************************/

std::ostream &meat::grinder::operator << (std::ostream &out,
                                          const Location &value) {
  return (out << (std::string)value);
}

/******************************************************************************
 * meat::grinder::Token Class
 */

/*******************************
 * meat::grinder::Token::Token *
 *******************************/

meat::grinder::Token::Token(const std::string &value,
                            meat::grinder::Token::token_t value_type,
                            const Location &position)
  : value_type(value_type), value(value), i_value(0), d_value(0.0),
  _position(position) {

  if (this->value_type == Token::WORD) {
    if (Utils::is_integer(value, i_value)) {
      this->value_type = Token::INTEGER;
    } else if (Utils::is_float(value, d_value)) {
      this->value_type = Token::NUMBER;
    }
  }
  subst();
}

meat::grinder::Token::Token(const Token &other)
  : value_type(other.value_type), value(other.value), i_value(other.i_value),
    d_value(other.d_value), _position(other._position) {
}

/********************************
 * meat::grinder::Token::~Token *
 ********************************/

meat::grinder::Token::~Token() throw() {
}

/*******************************
 * meat::grinder::Token::subst *
 *******************************/

void meat::grinder::Token::subst() {
  switch (value_type) {
  case LITRL_STRING:
    for (size_t offset = value.find("\\'");
         offset != value.npos;
         offset = value.find("\\'", offset + 1)) {
      value.replace(offset, 2, "'");
    }
  case SUBST_STRING:
    for (size_t offset = value.find("\\");
         offset != value.npos;
         offset = value.find("\\", offset + 1)) {
      switch (value[offset + 1]) {
      case '"':
      case '\\':
      case '\'':
      case '[':
      case ']':
      case '?':
        value.replace(offset, 2, 1, value[offset + 1]);
        break;
      case 'b':
        value.replace(offset, 2, "\b");
        break;
      case 'f':
        value.replace(offset, 2, "\f");
        break;
      case 'n':
        value.replace(offset, 2, "\n");
        break;
      case 'r':
        value.replace(offset, 2, "\r");
        break;
      case 't':
        value.replace(offset, 2, "\t");
        break;
      case 'v':
        value.replace(offset, 2, "\v");
        break;
      }
    }
    break;
  default:
    break;
  }
}

/******************************
 * meat::grinder::Token::type *
 ******************************/

meat::grinder::Token::token_t
meat::grinder::Token::type() const {
  return value_type;
}

/*********************************
 * meat::grinder::Token::is_type *
 *********************************/

bool meat::grinder::Token::is_type(token_t value_type) const {
  return (this->value_type == value_type);
}

/**********************************
 * meat::grinder::Token::position *
 **********************************/

const meat::grinder::Location &
meat::grinder::Token::position() const {
  return _position;
}

/************************************
 * meat::grinder::Token::operator = *
 ************************************/

meat::grinder::Token &
meat::grinder::Token::operator =(const Token &other) {
  if (this != &other) {
    value_type = other.value_type;
    value = other.value;
  }
  return *this;
}

/*************************************
 * meat::grinder::Token::operator == *
 *************************************/

bool meat::grinder::Token::operator ==(const std::string &value) const {
  return (this->value == value);
}

/************************************************
 * meat::grinder::Token::operator std::string & *
 ************************************************/

meat::grinder::Token::operator std::string () const {
  std::stringstream out;

  if (value_type == INTEGER) {
    out << (int)i_value;
    return out.str();
  } else if (value_type == NUMBER) {
    out << (double)d_value;
    return out.str();
  } else
    return value;
}

/***********************************************
 * meat::grinder::Token::operator std::int32_t *
 ***********************************************/

meat::grinder::Token::operator std::int32_t () const {
  return i_value;
}

/*****************************************
 * meat::grinder::Token::operator double *
 *****************************************/

meat::grinder::Token::operator double () const {
  return d_value;
}

/******************************************************************************
 * meat::grinder::SyntaxException Class
 */

/***************************************************
 * meat::grinder::SyntaxException::SyntaxException *
 ***************************************************/

meat::grinder::SyntaxException::SyntaxException(const Token &token,
                                                const std::string &message)
  : meat::Exception(meat::Class::resolve("Grinder.SyntaxException"), 2),
    token(token) {
  this->property(0) = new Text(std::string("Syntax Error (") +
                               (std::string)token.position() + ") " +
                               message);
}

meat::grinder::SyntaxException::SyntaxException(const SyntaxException &other)
  : meat::Exception(other.what()), token(other.token) {

}

/****************************************************
 * meat::grinder::SyntaxException::~SyntaxException *
 ****************************************************/

meat::grinder::SyntaxException::~SyntaxException() throw() {
}

/****************************************
 * meat::grinder::SyntaxException::line *
 ****************************************/

unsigned int meat::grinder::SyntaxException::line() const {
  return 0;
}

/*********************************************
 * meat::grinder::SyntaxException::character *
 *********************************************/

unsigned int meat::grinder::SyntaxException::character() const {
  return 0;
}

/******************************************************************************
 * meat::grinder::Tokenizer Class
 */

/************************
 * Tokenizer::Tokenizer *
 ************************/

meat::grinder::Tokenizer::Tokenizer() : stream(NULL) {
  complete = false;
  cook_lines = true;
  cont_line = false;
}

/*************************
 * Tokenizer::~Tokenizer *
 *************************/

meat::grinder::Tokenizer::~Tokenizer() throw() {
}

/********************
 * Tokenizer::parse *
 ********************/

void meat::grinder::Tokenizer::parse(std::istream &code) {
  stream = &code;
  remaining = "";
  current_line.reset();
  current_token.reset();
  get_next_line();
}

void meat::grinder::Tokenizer::parse(const std::string &code) {
  strs.str(code);
  stream = &strs;
  remaining = "";
  current_line.reset();
  current_token.reset();
  get_next_line();
}

/**********************************
 * meat::grinder::Tokenizer::push *
 **********************************/

void meat::grinder::Tokenizer::push() {
  Token current = tokens.front();

  // We only allow COMMAND and BLOCK to be pushed and reparsed.
  if (not current.is_type(Token::COMMAND) and
      not current.is_type(Token::BLOCK)) {
    throw SyntaxException(current, "Internal error reparsing token");
  }

  // Remove the token and save the state of the tokenizer.
  tokens.pop_front();
  states.push({tokens, remaining, stream, current_line, current_token});

  // Set up the tokenizer to parse the token.
  stream = new std::stringstream((const std::string &)current);
  remaining = "";
  current_line = current.position();
  current_token = current.position();
  current_line.rewind(); // Backup the line position by one.
  current_line.inc_offset();
  tokens.clear();

  // Start parsing.
  get_next_line();
}

/*********************************
 * meat::grinder::Tokenizer::pop *
 *********************************/

void meat::grinder::Tokenizer::pop() {
  if (not states.empty()) {
    delete stream;

    // Restore the state of the tokenizer.
    tokens = states.top().tokens;
    remaining = states.top().remaining;
    stream = states.top().stream;
    current_line = states.top().current_line;
    current_token = states.top().current_token;

    // Pop it from the states stack.
    states.pop();
  }
}

/***********************************
 * meat::grinder::Tokenizer::count *
 ***********************************/

size_t meat::grinder::Tokenizer::count() {
  return tokens.size();
}

/*****************************************
 * meat::grinder::Tokenizer::is_complete *
 *****************************************/

bool meat::grinder::Tokenizer::is_complete() {
  return complete;
}

/***********************************
 * meat::grinder::Tokenizer::clear *
 ***********************************/

void meat::grinder::Tokenizer::clear() {
  tokens.clear(); // Clear all the tokens.
}

/************************************
 * meat::grinder::Tokenizer::expect *
 ************************************/

bool meat::grinder::Tokenizer::expect(Token::token_t id) {
  if (is_more() && tokens.front().is_type(id))
    return true;
  return false;
}

bool meat::grinder::Tokenizer::expect(Token::token_t id,
                                      const std::string &value) {
  // Same type and value
  if (is_more() && tokens.front().is_type(id))
    return (tokens.front() == value);
  return false;
}

bool meat::grinder::Tokenizer::expect(size_t index, Token::token_t id) {
  if (is_more() && tokens.at(index).is_type(id))
    return true;
  return false;
}

bool meat::grinder::Tokenizer::expect(size_t index, Token::token_t id,
                                      const std::string &value) {
  // Same type and value
  if (is_more() && tokens.at(index).is_type(id))
    return (tokens.at(index) == value);
  return false;
}

/************************************
 * meat::grinder::Tokenizer::permit *
 ************************************/

void meat::grinder::Tokenizer::permit(Token::token_t id) {
  if (expect(id)) next();
  else
    throw SyntaxException(tokens.front(),
                          (std::string)tokens.front().position() +
                          ": Got unexpected value of " +
                          (std::string)tokens.front());
}

void meat::grinder::Tokenizer::permit(Token::token_t id,
                                      const std::string &value) {
  if (expect(id, value)) next();
  else
    throw SyntaxException(tokens.front(),
                          (std::string)tokens.front().position() +
                          ": Got unexpected value of " +
                          (std::string)tokens.front());
}

/**********************************
 * meat::grinder::Tokenizer::next *
 **********************************/

void meat::grinder::Tokenizer::next() {
  if (is_more()) tokens.pop_front();
  is_more();
}

/*************************************
 * meat::grinder::Tokenizer::is_more *
 *************************************/

bool meat::grinder::Tokenizer::is_more() {
  if (tokens.empty()) get_next_line();
  if (tokens.empty()) return false;
  return true;
}

/***************************************
 * meat::grinder::Tokenizer::to_string *
 ***************************************/

std::string meat::grinder::Tokenizer::to_string() const {
  std::string result;

  for (auto &token: tokens) {
    switch (token.type()) {
    case Token::WORD:
    case Token::INTEGER:
    case Token::NUMBER:
      result += (std::string)(token) + " "; break;
    case Token::SUBST_STRING:
    case Token::LITRL_STRING:
      result += std::string("'") + (std::string)(token) + "' "; break;
    case Token::COMMAND:
      result += std::string("[") + (std::string)(token) + "] "; break;
    case Token::BLOCK:
      result += std::string("{") + (std::string)(token) + "} "; break;
    case Token::EOL:
      result += "\u21b5\n"; break;
    };
  }

  return result;
}

/***************************************
 * meat::grinder::Tokenizer::get_uchar *
 ***************************************/

bool meat::grinder::Tokenizer::get_uchar(std::string &destination) {
  char ch[5] = {0, 0, 0, 0, 0};

  // Read in the first byte of the character.
  destination.clear();
  if (not stream->get(ch[0])) return stream->good();

  /* Determine and read in more characters if necessary. This doesn't not
   * check for UTF-8 correctness.
   */
  if ((ch[0] & 0x80) != 0) {
    if ((ch[0] & 0xe0) == 0xc0) {
      stream->get(ch[1]);
    } else if ((ch[0] & 0xf0) == 0xe0) {
      stream->get(&ch[1], 2);
    } else if ((ch[0] & 0xf0) == 0xf0) {
      stream->get(&ch[1], 3);
    }
  }

  // Return the results.
  destination = ch;
  return stream->good();
}

/**************************************
 * meat::grinder::Tokenizer::get_line *
 **************************************/

void meat::grinder::Tokenizer::get_line(std::string &destination) {
  std::string ch;
  while (get_uchar(ch)) {
    if (ch == "\n" or ch == "\r" or ch == "\v" or ch == "\f"
        or ch == "\u2028" or ch == "\u2029") {
      current_line.inc_line();
      if (cook_lines)
        current_token = current_line;
      return;
    }
    destination += ch;
  }
}

/****************************************
 * meat::grinder::Tokenizer::parse_line *
 ****************************************/

void meat::grinder::Tokenizer::parse_line(const std::string &line,
                                          bool more __attribute__((unused))) {
  size_t t_begin = 0;
  size_t t_end = 0;
  bool command_done = false;
  std::string command = remaining + line;

  complete = true;

  while (!command_done) {
    // Skip any leading whitespace.
    t_begin = command.find_first_not_of(" \t", t_begin);

    if (t_begin == command.npos) {
      command_done = true;
    } else {
      unsigned int count = 1; // Flag for matching up brackets.

      if (cook_lines)
        current_token.offset(t_begin + current_line.offset());

      switch (command[t_begin]) {

      case '\n': // \n
        t_end = t_begin;
        if (cont_line)
          cont_line = false;
        else
          tokens.push_back(Token("\n", Token::EOL, current_token));

        t_end += 1;
        break;

      case '[': // Parse a command token
        t_end = t_begin + 1;

        count = 0;
        while (command[t_end] != ']' or count > 0) {
          if (command[t_end] == '\n') {
            if (cont_line) {
              remaining = command.substr(t_begin, t_end - t_begin);
              complete = false;
              command_done = true;
              return;
            } else
              throw SyntaxException(
                Token(command.substr(t_begin + 1,
                                     t_end - t_begin - 1),
                      Token::COMMAND,
                      current_token),
                std::string("Syntax error: Missing closing bracket "
                            "\"]\""));
          }

          if (command[t_end] == '[') {
            ++count;
          } else if (command[t_end] == ']') {
            --count;
          }

          t_end += 1;
        }

        complete = true;
        tokens.push_back(Token(command.substr(t_begin + 1,
                                              t_end - t_begin - 1),
                               Token::COMMAND,
                               current_token));
        t_end += 1;
        break;

      case '{': { // Parse a block token
        /*  Here we read the block punction {} tokens as an unmodified
         * string. This will later be parsed by the library compiler or the
         * interpreter.
         */
        t_end = t_begin;
        cook_lines = false;
        while (command[t_end] != '}' or count > 0) {
          t_end += 1;
          if (command[t_end] == '{') {
            count += 1;
          } else if (command[t_end] == '}') {
            count -= 1;
          } else if (command[t_end] == '\0') {
            remaining = command.substr(t_begin, t_end - t_begin);
            complete = false;
            command_done = true;
            return;
          }
        }

        cook_lines = true;
        tokens.push_back(Token(command.substr(t_begin + 1,
                                              t_end - t_begin - 1),
                               Token::BLOCK,
                               current_token));
        t_end += 1;

        break;
      }

      case '"': // Parse a substitutable string
        t_end = t_begin + 1;
        while (command[t_end] != '"') {
          t_end += 1;

          // Found a new line.
          if (command[t_end] == '\n')
            throw SyntaxException(Token(command.substr(t_begin + 1,
                                                       t_end - t_begin - 1),
                                        Token::SUBST_STRING,
                                        current_token),
                                  "Multiline text constant not allowed");

          // Incomplete string, but not a syntax error yet.
          if (t_end >= command.length()) {
            command_done = true;
            complete = false;
            break;
          }

          // An escaped double quote.
          if (command[t_end] == '"' && command[t_end - 1] == '\\')
            t_end += 1;
        }

        tokens.push_back(Token(command.substr(t_begin + 1,
                                              t_end - t_begin - 1),
                               Token::SUBST_STRING,
                               current_token));
        t_end += 1;
        break;

      case '\'': // Parse a literal string
        t_end = t_begin + 1;
        while (command[t_end] != '\'') {
          t_end += 1;

          if (command[t_end] == '\n')
            throw SyntaxException(Token(command.substr(t_begin + 1,
                                                       t_end - t_begin - 1),
                                        Token::LITRL_STRING,
                                        current_token),
                                  "Multiline text constant not allowed");

          if (t_end >= command.length()) {
            command_done = true;
            complete = false;
            break;
          }

          if (command[t_end] == '\'' && command[t_end - 1] == '\\')
            t_end += 1;
        }

        tokens.push_back(Token(command.substr(t_begin + 1,
                                              t_end - t_begin - 1),
                               Token::LITRL_STRING,
                               current_token));
        t_end += 1;
        break;

      default: // Parse a word token

        t_end = command.find_first_of(" \t\n]", t_begin);
        if (t_end == command.npos) {
          command_done = true;
          complete = true;
        }

        tokens.push_back(Token(command.substr(t_begin,
                                           t_end - t_begin),
                               Token::WORD,
                               current_token));
        break;
      }

      t_begin = t_end;
    }

    if (complete) remaining = "";
  }
}

/****************************************
 * meat::grinder::Tokenizer::operator[] *
 ****************************************/

meat::grinder::Token &meat::grinder::Tokenizer::operator[] (size_t index) {
  return tokens.at(index);
}

/*******************************************
 * meat::grinder::Tokenizer::get_next_line *
 *******************************************/

void meat::grinder::Tokenizer::get_next_line() {

#ifdef TESTING
    meat::test::test("Null code stream", false);
    if (stream == NULL) {
      throw Exception("Attempting to parse a null code stream");
    } else {
      meat::test::passed("Null code stream");
    }
#endif // TESTING

  while (!stream->eof()) {
    // Read a line from the file.
    std::string line;

    get_line(line);

    if (cook_lines) {
      // Just skip over comments.
      if (line[line.find_first_not_of(" \n\r\t")] == '#')
        continue;

      // Trim any trailing white space
      line.erase(line.find_last_not_of(" \t") + 1);

      // If the line ends with a "\", then we append the next line to it.
      if (line[line.length() - 1] == '\\') {
        line.erase(line.length() - 1);
        cont_line = true;
      } else
        cont_line = false;

      /* If the line is not empty then parser the tokens and compile the
       * command.
       */
      if (!line.empty()) {
        parse_line(line + '\n');
      } else {
        continue;
      }
    } else { // not cook_lines
      parse_line(line + '\n');
    }

    if (complete) return;
  }
}

/******************************************************************************
 * Language Class Implementation
 */

/***********************************
 * meat::grinder::Grammer::Grammer *
 ***********************************/

meat::grinder::Grammer::Grammer() {
  curr_line = 0;
}

/************************************
 * meat::grinder::Grammer::~Grammer *
 ************************************/

meat::grinder::Grammer::~Grammer() throw() {
}

/***********************************
 * meat::grinder::Grammer::execute *
 ***********************************/

void meat::grinder::Grammer::execute(std::istream &code) {
  tokens.parse(code);
  while (tokens.is_more()) {
    if (tokens.is_complete() and tokens.count() > 0) {
      command();
    }
  }
}

void meat::grinder::Grammer::execute(const std::string &code) {
  tokens.parse(code);
  while (tokens.is_more()) {
    if (tokens.is_complete() and tokens.count() > 0) {
      command();
    }
  }
}

/***********************************
 * meat::grinder::Grammer::command *
 ***********************************/

void meat::grinder::Grammer::command() {
}
