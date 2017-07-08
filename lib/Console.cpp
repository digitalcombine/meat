/******************************************************************************
 * Auto-generated by the Grinder Compiler Library
 */

#include <meat.h>
#include <meat/datastore.h>

#include <iostream>

#define null (meat::Null())

/******************************************************************************
 * Console Class
 */

static meat::vtable_entry_t ConsoleMethods[] = {
  {0x0000043c, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x05c30872, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {.offset = 0}}
};

// class method error:
static meat::Reference Console_cm_error_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference klass = CONTEXT(context).get_class();
  meat::Reference message = CONTEXT(context).get_param(0);

		std::cerr << TEXT(message);
		return null;
	}

// class method errorLn:
static meat::Reference Console_cm_errorLn_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference klass = CONTEXT(context).get_class();
  meat::Reference message = CONTEXT(context).get_param(0);

		std::cerr << TEXT(message) << std::endl;
		return null;
	}

// class method input
static meat::Reference Console_cm_input(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference klass = CONTEXT(context).get_class();

		std::string result;
		std::cin >> result;
		return new meat::Text(result);
	}

// class method write:
static meat::Reference Console_cm_write_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference klass = CONTEXT(context).get_class();
  meat::Reference message = CONTEXT(context).get_param(0);

		std::cout << TEXT(message);
		return null;
	}

// class method writeLn:
static meat::Reference Console_cm_writeLn_(meat::Reference context) {
  meat::Reference self = CONTEXT(context).get_self();
  meat::Reference klass = CONTEXT(context).get_class();
  meat::Reference message = CONTEXT(context).get_param(0);

		std::cout << TEXT(message) << std::endl;
		return null;
	}

static meat::vtable_entry_t ConsoleCMethods[] = {
  {0x0000043c, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x0462f057, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x05fb57ca, 0x1bef7d54, VTM_NATIVE  , 0, Console_cm_input},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x11f0d8be, 0x1bef7d54, VTM_NATIVE  , 1, Console_cm_writeLn_},
  {0x1b6d6881, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x1b6d68ba, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x2c296348, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x32d56f33, 0x1bef7d54, VTM_NATIVE  , 1, Console_cm_error_},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x50b30a5d, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x5188a43c, 0x1bef7d54, VTM_NATIVE  , 1, Console_cm_write_},
  {0x5336c10e, 0x1bef7d54, VTM_NATIVE  , 1, Console_cm_errorLn_},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {.offset = 0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {.offset = 0}}
};

static meat::uint8_t Symbols[] = {
  "Console\0error:\0errorLn:\0input\0write:\0writeLn:\0\0"
};

// We need to declare init_Console as a C function.
extern "C" {
  DECLSPEC void init_Console(meat::data::Library &library);
}

void init_Console(meat::data::Library &library) {
  meat::Class *cls;

  cls = new meat::Class(meat::Class::resolve(0x419df72b), 0, 0);
  cls->set_vtable(7, ConsoleMethods, meat::STATIC);
  cls->set_class_vtable(18, ConsoleCMethods, meat::STATIC);
  library.add(cls, "Console");

  library.set_symbols(Symbols, meat::STATIC);
}
