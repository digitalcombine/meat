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

#include <compiler.h>
#include <testsuite>

static const std::string code =
"# Sample code\n\n"
"Object subclass: TestObject as: {\n"
"  property p1\n\n"
"  [object message]\n"
"  10 3.1415\n"
"}\n"
"'Hello' \"World\"\n";

static std::string to_string(meat::grinder::Token::token_t t_type) {
  switch (t_type) {
    case meat::grinder::Token::WORD:
      return "WORD";
    case meat::grinder::Token::SUBST_STRING:
    case meat::grinder::Token::LITRL_STRING:
      return "STRING";
    case meat::grinder::Token::INTEGER:
      return "INTEGER";
    case meat::grinder::Token::NUMBER:
      return "NUMBER";
    case meat::grinder::Token::COMMAND:
      return "COMMAND";
    case meat::grinder::Token::BLOCK:
      return "BLOCK";
    case meat::grinder::Token::EOL:
      return "EOF";
  }
}

static void token_match(test::Test &test,
                        meat::grinder::Token &token,
                        meat::grinder::Token::token_t t_type,
                        const std::string &value,
                        unsigned int line,
                        unsigned int offset) {
  if (test)
    test << test::assert(token.is_type(t_type))
         << "Invalid token type, " << to_string(token.type()) << " != "
         << to_string(t_type);
  if (test and not value.empty())
    test << test::assert(token == value)
         << "Invalid token value " << (std::string)(token) << " != "
         << value;
  if (test)
    test << test::assert(token.position().line() == line)
         << "Invalid token line " << (unsigned int)token.position().line()
         << " != " << line;
  if (test)
    test << test::assert(token.position().offset() == offset)
       << "Invalid token line offset "
       << (unsigned int)token.position().offset() << " != " << offset;
}

int main(int argc, char *argv[]) {
  test::TestSuite tests("Parser Tokens");
  meat::grinder::Tokenizer tokens;

  tokens.parse(code);

  test::Test &test_obj = tests.test("Object Token");
  token_match(test_obj, tokens[0],
              meat::grinder::Token::WORD, "Object", 3, 1);
  tokens.next();

  test::Test &test_sc = tests.test("subclass: Token");
  token_match(test_sc, tokens[0],
              meat::grinder::Token::WORD, "subclass:", 3, 8);
  tokens.next();

  test::Test &test_to = tests.test("TestObject Token");
  token_match(test_to, tokens[0],
              meat::grinder::Token::WORD, "TestObject", 3, 18);
  tokens.next();

  test::Test &test_as = tests.test("as: Token");
  token_match(test_as, tokens[0],
              meat::grinder::Token::WORD, "as:", 3, 29);
  tokens.next();

  test::Test &test_body = tests.test("Object Body Token");
  token_match(test_body, tokens[0],
              meat::grinder::Token::BLOCK, "", 3, 33);

  tokens.push();
  test::Test &test_pty = tests.test("Object Body property Token");
  token_match(test_pty, tokens[0],
              meat::grinder::Token::WORD, "property", 4, 3);
  tokens.next();

  test::Test &test_p1 = tests.test("Object Body p1 Token");
  token_match(test_p1, tokens[0],
              meat::grinder::Token::WORD, "p1", 4, 12);
  tokens.next();

  test::Test &test_beol = tests.test("Object Body EOL Token");
  token_match(test_beol, tokens[0],
              meat::grinder::Token::EOL, "", 4, 14);
  tokens.next();

  test::Test &test_cmd = tests.test("Object Body command Token");
  token_match(test_cmd, tokens[0],
              meat::grinder::Token::COMMAND, "", 6, 3);

  tokens.push();
  test::Test &test_cobj = tests.test("Command object Token");
  token_match(test_cobj, tokens[0],
              meat::grinder::Token::WORD, "object", 6, 4);
  tokens.next();

  test::Test &test_cmsg = tests.test("Command message Token");
  token_match(test_cmsg, tokens[0],
              meat::grinder::Token::WORD, "message", 6, 11);
  tokens.next();

  test::Test &test_ceol = tests.test("Command EOL Token");
  token_match(test_ceol, tokens[0],
              meat::grinder::Token::EOL, "", 6, 18);
  tokens.pop();
  tokens.next(); // EOL

  test::Test &test_ten = tests.test("Object 10 Token");
  token_match(test_ten, tokens[0],
              meat::grinder::Token::INTEGER, "10", 7, 3);
  tokens.next();

  test::Test &test_pi = tests.test("Object 3.1415 Token");
  token_match(test_pi, tokens[0],
              meat::grinder::Token::NUMBER, "3.1415", 7, 6);
  tokens.next();

  std::cout << tests << std::flush;
  return ((bool)tests ? 0 : 1);
}
