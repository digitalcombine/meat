/******************************************************************************
 * Auto-generated by the Grinder Compiler Library
 */

#include <meat.h>
#include <meat/datastore.h>

#include <iostream>

using namespace meat;

#define null (meat::Null())

/******************************************************************************
 * Console Class
 */

static meat::vtable_entry_t ConsoleMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00379f78, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x05c30872, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x331152ee, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x331156ce, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x34003578, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x48dbf560, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

// class method error:
static Reference Console_cm_error_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference message = cast<Context>(context).parameter(0);

		std::cerr << meat::cast<meat::Text>(message);
		return null;
	}

// class method errorLn:
static Reference Console_cm_errorLn_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference message = cast<Context>(context).parameter(0);

		std::cerr << meat::cast<meat::Text>(message) << std::endl;
		return null;
	}

// class method input
static Reference Console_cm_input(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();

		std::string result;
		std::cin >> result;
		return new meat::Text(result);
	}

// class method write:
static Reference Console_cm_write_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference message = cast<Context>(context).parameter(0);

		std::cout << meat::cast<meat::Text>(message);
		return null;
	}

// class method writeLn:
static Reference Console_cm_writeLn_(Reference context) {
  Reference self = cast<Context>(context).self();
  Reference klass = cast<Context>(context).klass();
  Reference message = cast<Context>(context).parameter(0);

		std::cout << meat::cast<meat::Text>(message) << std::endl;
		return null;
	}

static meat::vtable_entry_t ConsoleCMethods[] = {
  {0x00000782, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x000007a0, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00019850, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x00368f3a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x0462f057, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x05c30872, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x05fb57ca, 0x1bef7d54, VTM_NATIVE  , 0, {(meat::method_ptr_t)Console_cm_input}},
  {0x068b6f7b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x11f0d8be, 0x1bef7d54, VTM_NATIVE  , 1, {(meat::method_ptr_t)Console_cm_writeLn_}},
  {0x1b6d6881, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x1b6d68ba, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x20be875b, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x24ab71da, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x32d56f33, 0x1bef7d54, VTM_NATIVE  , 1, {(meat::method_ptr_t)Console_cm_error_}},
  {0x39a68c12, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x39a6a1d2, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x5188a43c, 0x1bef7d54, VTM_NATIVE  , 1, {(meat::method_ptr_t)Console_cm_write_}},
  {0x5336c10e, 0x1bef7d54, VTM_NATIVE  , 1, {(meat::method_ptr_t)Console_cm_errorLn_}},
  {0x54aa30e6, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x6b2d9a7a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7a8e569a, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7b840562, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}},
  {0x7d180801, 0x00000000, VTM_SUPER   , 0, {(meat::method_ptr_t)0}}
};

static std::uint8_t Symbols[] = {
  "Console\0error:\0errorLn:\0input\0write:\0writeLn:\0\0"
};

// We need to declare init_Console as a C function.
extern "C" {
  DECLSPEC void init_Console(meat::data::Library &library);
}

void init_Console(meat::data::Library &library) {
  meat::Class *cls;

  // Import required libraries.

  cls = new meat::Class(meat::Class::resolve(0x419df72b), 0, 0);
  cls->set_vtable(18, ConsoleMethods, meat::STATIC);
  cls->set_class_vtable(23, ConsoleCMethods, meat::STATIC);
  library.add(cls, "Console");

  library.set_symbols(Symbols);
}
