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

#include <cstdlib>
#ifdef TESTING
#include <testing.h>
#endif

/******************************************************************************
 * Location Class
 */

meat::compiler::Location::Location()
	: source(""), line(1), offset(0) {
}

meat::compiler::Location::Location(const Location &other)
	: source(other.source), line(other.line), offset(other.offset) {
}

meat::compiler::Location::~Location() throw() {
}

void meat::compiler::Location::set_source(std::string &source_name) {
	source = source_name;
}

void meat::compiler::Location::inc_line() {
	line++;
}

void meat::compiler::Location::inc_offset(uint32_t amount) {
	offset += amount;
}

void meat::compiler::Location::set_offset(uint32_t position) {
	offset = position;
}

meat::compiler::Location &
meat::compiler::Location::operator =(const Location &other) {
	if (&other != this) {
		source = other.source;
		line = other.line;
		offset = other.offset;
	}

	return *this;
}

/******************************************************************************
 * Token Class
 */

/********************************
 * meat::compiler::Token::Token *
 ********************************/

meat::compiler::Token::Token(const std::string &value,
															meat::compiler::Token::token_t value_type,
															Location &start, Location &end)
	: value_type(value_type), value(value), start(start), end(end) {
	subst();
}

meat::compiler::Token::Token(const Token &other)
	: value_type(other.value_type), value(other.value), start(other.start),
		end(other.end) {
}

/*********************************
 * meat::compiler::Token::~Token *
 *********************************/

meat::compiler::Token::~Token() throw() {
}

/********************************
 * meat::compiler::Token::subst *
 ********************************/

void meat::compiler::Token::subst() {
#ifdef DEBUG
	//std::cout << "DEBUG: subst " << value << std::endl;
#endif
	switch (value_type) {
	case LITRL_STRING:
		for (size_t offset = value.find("\\'");
				 offset != value.npos;
				 offset = value.find("\\'", offset)) {
			value.replace(offset, 2, "'");
		}
#ifdef DEBUG
		//std::cout << "DEBUG:  -> " << value << std::endl;
#endif
	case SUBST_STRING:
		// XXX Command substitution here!!!

		for (size_t offset = value.find("\\");
				 offset != value.npos;
				 offset = value.find("\\", offset)) {
			switch (value[offset + 1]) {
			case '"':
			case '\\':
			case '\'':
			case '[':
			case ']':
			case '?':
				value.replace(offset, 2, &value[offset + 1]);
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

meat::compiler::Token &
meat::compiler::Token::operator =(const Token &other) {
	if (this != &other) {
		value_type = other.value_type;
		value = other.value;
	}
	return *this;
}

bool meat::compiler::Token::operator ==(const char *value) const {
	return (this->value == value);
}

meat::compiler::Token::operator float_t () {
	return std::atof(value.c_str());
}

/******************************************************************************
 * Tokens Class Implementation
 */

/************************
 * Tokenizer::Tokenizer *
 ************************/

meat::compiler::Tokenizer::Tokenizer() : code(0) {
	complete = false;
  cook_lines = true;
}

/*************************
 * Tokenizer::~Tokenizer *
 *************************/

meat::compiler::Tokenizer::~Tokenizer() throw() {
}

/********************
 * Tokenizer::parse *
 ********************/

void meat::compiler::Tokenizer::parse(std::istream &code) {
	this->code = &code;
  remaining = "";
  //position.reset();
}

void meat::compiler::Tokenizer::parse(const std::string &code) {
  strs.str(code);
  this->code = &strs;
  remaining = "";
  get_next_line();
}

void meat::compiler::Tokenizer::parse(const Location &position,
                                       const std::string &line,
                                       bool more) {
	size_t t_begin = 0;
	size_t t_end = 0;
	bool command_done = false;
	std::string command = remaining + line;
	Location end;

	if (remaining.empty()) start = position;
	end = start;

#ifdef DEBUG
	//std::cout << "DEBUG: parsing \"" << command << "\"" << std::endl;
#endif

	complete = true;

	while (!command_done) {
		t_begin = command.find_first_not_of(" \t\n", t_begin);

		if (t_begin == command.npos) {
			command_done = true;
		} else {
			unsigned int count = 1; // Flag for matching up brackets.
			//start.inc_offset(t_begin - ???);

#ifdef DEBUG
			//std::cout << "DEBUG:  found token at " << t_begin << std::endl;
#endif

			switch (command[t_begin]) {

			case '"': // Parse a substitutable string
				t_end = t_begin + 1;
				while (command[t_end] != '"') {
					t_end += 1;

					// Found a new line.
					if (command[t_end] == '\n')
						throw meat::Exception("Syntax error: Multiline text constant not"
																	 " allowed");

					// Incomplete string, but not a syntax error yet.
					if (t_end >= command.length()) {
						command_done = true;
						complete = false;
						break;
					}

					// An escaped double quote.
					if (command[t_end] == '\\' && command[t_end + 1] == '"')
						t_end += 2;
				}

				tokens.push_back(Token(command.substr(t_begin + 1,
																							t_end - t_begin - 1),
															 Token::SUBST_STRING,
															 start, end));
				t_end += 1;
				start.inc_offset(t_end - t_begin);
				break;

			case '\'': // Parse a literal string
				t_end = t_begin + 1;
				while (command[t_end] != '\'') {
					t_end += 1;

					if (command[t_end] == '\n')
						throw meat::Exception("Syntax error: Multiline text constant not"
																	 " allowed");

					if (t_end >= command.length()) {
						command_done = true;
						complete = false;
						break;
					}

					if (command[t_end] == '\\' && command[t_end + 1] == '\'')
						t_end += 2;
				}

				tokens.push_back(Token(command.substr(t_begin + 1,
																							t_end - t_begin - 1),
															 Token::LITRL_STRING,
															 start, end));
				t_end += 1;
				break;

			case '[': // Parse a command token
				t_end = t_begin + 1;

				if (command[t_end] == '[') count++;

				while (command[t_end] != ']' || count > 0) {
					t_end += 1;

					if (command[t_end] == '\n')
						throw meat::Exception(
							std::string("Syntax error: Missing closing bracket "
													"\"]\""));

					if (command[t_end] == '[') {
						count += 1;
					} else if (command[t_end] == ']') {
						count -= 1;
					}
				}

				tokens.push_back(Token(command.substr(t_begin + 1,
																							t_end - t_begin - 1),
															 Token::COMMAND,
															 start, end));
				t_end += 1;
				break;

			case '{': { // Parse a block token
				t_end = t_begin;
        cook_lines = false;
				while (command[t_end] != '}' or count > 0) {
					t_end += 1;
					if (command[t_end] == '{') {
						count += 1;
					} else if (command[t_end] == '}') {
						count -= 1;
					} else if (command[t_end] == '\0') {
						remaining = command.substr(t_begin, t_end - t_begin) + '\n';
						complete = false;
						command_done = true;
						return;
					}
				}

        cook_lines = true;
				tokens.push_back(Token(command.substr(t_begin + 1,
																							t_end - t_begin - 1),
															 Token::BLOCK,
															 start, end));
				t_end += 1;
				break;
			}
			default: // Parse a word token

				t_end = command.find_first_of(" \t\n", t_begin);
				if (t_end == command.npos) {
					command_done = true;
					complete = true;
				}

				tokens.push_back(Token(command.substr(t_begin,
																							t_end - t_begin),
															 Token::WORD,
															 start, end));

				break;
			}

			t_begin = t_end;
		}
		if (complete) remaining = "";
	}
}

void meat::compiler::Tokenizer::parse(const Token &token) {
  strs.str((const std::string &)token);
  this->code = &strs;
  remaining = "";
  get_next_line();
}

size_t meat::compiler::Tokenizer::count() {
	return tokens.size();
}

bool meat::compiler::Tokenizer::is_complete() {
	return complete;
}

void meat::compiler::Tokenizer::clear() {
	tokens.clear();
}

bool meat::compiler::Tokenizer::expect(Token::token_t id) {
  if (is_more() && tokens.front().is_type(id))
      return true;
  return false;
}

bool meat::compiler::Tokenizer::expect(Token::token_t id, const char *value) {
  if (is_more() && tokens.front().is_type(id))
      return tokens.front() == value;
  return false;
}

void meat::compiler::Tokenizer::permit(Token::token_t id) {
  if (expect(id))
    tokens.pop_front();
  else
    throw Exception(std::string("Got unexpected value of ") +
                    (std::string &)tokens.front());
}

void meat::compiler::Tokenizer::permit(Token::token_t id, const char *value) {
  if (expect(id, value))
    tokens.pop_front();
  else
    throw Exception(std::string("Got unexpected value of ") +
                    (std::string &)tokens.front());
}

void meat::compiler::Tokenizer::next() {
  if (!tokens.empty()) tokens.pop_front();
}

bool meat::compiler::Tokenizer::is_more() {
  if (tokens.empty()) get_next_line();
  if (tokens.empty()) return false;
  return true;
}

std::string meat::compiler::Tokenizer::to_string() const {
	std::string result;

	std::deque<Token>::const_iterator it;
	for (it = tokens.begin(); it != tokens.end(); it++) {
		result += (const std::string &)(*it) + " ";
	}

	return result;
}

meat::compiler::Token &meat::compiler::Tokenizer::operator[] (size_t index) {
	return tokens.at(index);
}

void meat::compiler::Tokenizer::get_next_line() {

#ifdef TESTING
    meat::test::test("Null code stream", false);
    if (code == (void *)0) {
      throw Exception("Attempting to parse a null code stream");
    } else {
      meat::test::passed("Null code stream");
    }
#endif // TESTING

	while (!code->eof()) {
		// Read a line from the file.
		std::string line;
		Location start = position;

		std::getline(*code, line);

    if (cook_lines) {
      // Just skip over comments.
      if (line[line.find_first_not_of(" \n\r\t")] == '#') {
        position.inc_line();
        continue;
      }

      // Trim any trailing white space
      size_t first_char_offset = line.find_last_not_of(" \n\r\t");
      line.erase(first_char_offset + 1);

      // Record where out tokens start.
      //start.set_offset(first_char_offset);

      // If the line ends with a "\", then we append the next line to it.
      while (line[line.length() - 1] == '\\' && !code->eof()) {
        position.inc_line();

        std::string next_line;
        std::getline(*code, next_line);

        line.erase(line.length() - 2);
        line += next_line;

        line.erase(line.find_last_not_of(" \n\r\t") + 1);
      }

      // If the line is not empty then parser the tokens and compile the command.
      if (!line.empty()) {
#ifdef DEBUG
        //std::cout << "DEBUG: Read cooked line \"" << line << "\"" << std::endl;
#endif /* DEBUG */
        parse(start, line);
      } else
        continue;
    } else { // not cook_lines
#ifdef DEBUG
      //std::cout << "DEBUG: Read line \"" << line << "\"" << std::endl;
#endif /* DEBUG */
      parse(start, line);
    }
    position.inc_line();
    if (complete) return;
	}
}

/******************************************************************************
 * Language Class Implementation
 */

meat::compiler::Language::Language() {
	curr_line = 0;
}

meat::compiler::Language::~Language() throw() {
}

void meat::compiler::Language::execute(std::istream &code) {
	Tokenizer parser;
  parser.parse(code);
  while (parser.is_more()) {
    if (parser.is_complete() && parser.count() > 0) {
      command(parser);
    }
  }
}

void meat::compiler::Language::execute(const std::string &code) {
	Tokenizer parser;
  parser.parse(code);
  while (parser.is_more()) {
    if (parser.is_complete() && parser.count() > 0) {
      command(parser);
    }
  }
}

void meat::compiler::Language::execute(const Token &token) {
	Tokenizer parser;

  parser.parse(token);
  while (parser.is_more()) {
    if (parser.is_complete() && parser.count() > 0) {
      command(parser);
    }
  }
}

void meat::compiler::Language::command(Tokenizer &tokens) {
}

/******************************************************************************
 */

extern "C" {
	DECLSPEC void init_Compiler(meat::data::Library &library);
}

void init_Compiler(meat::data::Library &library) {
	meat::Class *cls;

	/* Create the Comiler.Library class. */
	cls = new meat::Class("Object", 2);
	library.add(cls, "Grinder.Library");

	/* Create the Compiler.Class class */
	cls = new meat::Class("Object", 6);
	library.add(cls, "Grinder.Class");

	/* Create the Compiler.Method class. */
	cls = new meat::Class("Object", 3);
	library.add(cls, "Grinder.Method");
}
