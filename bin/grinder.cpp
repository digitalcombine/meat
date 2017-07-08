/*                                                                  -*- c++ -*-
 * A simple Meat compiler for building Libraries and Archives.
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
 *
 */

#include <meat.h>
#include <meat/datastore.h>
#include <meat/utilities.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#ifdef TESTING
#include <testing.h>
#endif

std::deque<const char *> source_files;
std::string out_file;
std::string app_class;

meat::Reference library;

/** Display simple help for the -h option
 */
static void help() {
  std::cout << "Meat Compiler v" << VERSION << "\n\n";
  std::cout << "grinder [-l libname|-a archname] [-i path] source ...\n";
  std::cout << "grinder -h\n";
  std::cout << "  -i path      Include path to the library search\n";
  std::cout << "  -l libname   Compile a library\n";
  std::cout << "  -c appclass  Compile a library\n";
  std::cout << "  -a archname  Compile an archive\n";
  std::cout << "  -# text      Return a hash value for the text\n";
  std::cout << "  -h           Displays this help" << std::endl;
}

typedef void (*exec_class_fn)(meat::Reference, std::istream &);

/** Used to add functionallity to the Class subClass method when the compiler
 * is initialized. If the compiler is not initialized then the Class subClass
 * method should just raise an exception.
 */
static void classbuilder_int(meat::Reference super,
														 const std::string &cls_name,
                             const std::string &cls_body,
														 meat::Reference context) {
	meat::data::Library *grinder = meat::data::Library::get("Grinder");
	exec_class_fn exec_class =
		(exec_class_fn)grinder->dlsymbol("exec_class");

	meat::Reference ctx =
		meat::message(meat::Class::resolve("Grinder.Class"),
									"subClassFrom:as:",
									context);
	CONTEXT(ctx).set_param(0, super);
	CONTEXT(ctx).set_param(1, new meat::Text(cls_name));
	meat::Reference cb = meat::execute(ctx);

	ctx = meat::message(library, "addClass:", context);
	CONTEXT(ctx).set_param(0, cb);
	execute(ctx);

  std::string body(cls_body);
  std::istringstream is(body);

	exec_class(cb, is);
}

/** Used for the Library import method when the compiler is initialized.
 */
static void compiler_import(const std::string &name,
														meat::Reference context) {
	meat::Reference new_ctx = message(library, "import:", context);
	CONTEXT(new_ctx).set_param(0, new meat::Text(name));
	execute(new_ctx);
}

/*********************
 * build_archive_app *
 *********************/

typedef void * interp_t;
typedef interp_t (*create_interp_fn)(const std::string &);
typedef void (*exec_interp_fn)(interp_t, std::istream &);
typedef void (*close_interp_fn)(interp_t);

static void build_archive_app() {
  meat::data::Library *grinder = meat::data::Library::get("Grinder");

	create_interp_fn create_interpreter =
		(create_interp_fn)grinder->dlsymbol("create_interpreter");
	exec_interp_fn exec_interpreter =
		(exec_interp_fn)grinder->dlsymbol("exec_interpreter");
	close_interp_fn close_interpreter =
		(close_interp_fn)grinder->dlsymbol("close_interpreter");

	interp_t interp = create_interpreter(out_file);

	// Go through each file and compile them
	for (std::deque<const char *>::iterator it = source_files.begin();
			 it != source_files.end();
			 ++it) {

#ifdef DEBUG
		std::cout << "DEBUG: Compiling " << *it << std::endl;
#endif /* DEBUG */

		std::ifstream meat_file;

		meat_file.open(*it, std::ios::in);

		if (meat_file.is_open()) {
			exec_interpreter(interp, meat_file);
			meat_file.close();
		} else {
			std::cerr << "ERROR: Unable to open " << *it << std::endl;
			return;
		}
	}

	close_interpreter(interp);
}

/*********************
 * build_library_app *
 *********************/

typedef void (*exec_library_fn)(meat::Reference, std::istream &);

static void build_library_app() {
	meat::data::Library *grinder = meat::data::Library::get("Grinder");
	exec_library_fn exec_library =
		(exec_library_fn)grinder->dlsymbol("exec_library");

	meat::Reference context =
		meat::message(meat::Class::resolve("Grinder.Library"),
									"new:", meat::Null());
	CONTEXT(context).set_param(0, new meat::Text(out_file));
	library = meat::execute(context);

	// Go through each file and compile them
	for (std::deque<const char *>::iterator it = source_files.begin();
			 it != source_files.end();
			 ++it) {

#ifdef DEBUG
		std::cout << "DEBUG: Compiling " << *it << std::endl;
#endif /* DEBUG */

		std::ifstream meat_file;

		meat_file.open(*it, std::ios::in);

		if (meat_file.is_open()) {
			exec_library(library, meat_file);
			meat_file.close();
		} else {
			std::cerr << "ERROR: Unable to open " << *it << std::endl;
			return;
		}
	}

	if (not app_class.empty()) {
		context = message(library, "setApplicationClass:", meat::Null());
		CONTEXT(context).set_param(0, new meat::Text(app_class));
		meat::execute(context);
	}

	context = message(library, "compile", meat::Null());
	meat::execute(context);

	library = NULL;
}

/******************************************************************************
 * Meat Ginder Entry
 */
int main(int argc, const char *argv[]) {

  /* The compiler state specified by the command line options. */
  static enum {
    UNSET, BUILD_LIBRARY, BUILD_ARCHIVE
  } state = UNSET;

#ifdef DEBUG
  std::cout << "Grinder the Meat Compiler" << std::endl;
  std::cout << "  Debugging enabled" << std::endl;
#ifdef TESTING
  std::cout << "  Testing Build" << std::endl;
#endif
#endif

  meat::initialize(argc, argv, compiler_import, classbuilder_int);

#ifdef TESTING
  meat::test::run_tests();
#endif

  /*********************************
   * Parse the command line options.
   */
  for (int c = 1; c < argc; c++) {

    if (argv[c][0] == '-') {
      switch (argv[c][1]) {

      case 'i': // Include library path
        meat::data::Library::add_path(argv[c + 1]);
        c++;
        break;

      case 'l': // Build library option
        if (state != UNSET) {
          std::cerr << "FATAL: options -l/-a cannot be used at the same time."
                    << std::endl;
          return 1;
        }

#ifdef DEBUG
        std::cout << "DEBUG: Compiling library file " << argv[c + 1] << ".lib"
                  << std::endl;
#endif /* DEBUG */
        out_file = argv[c + 1];
        state = BUILD_LIBRARY;
        c++;
        break;

      case 'c':
        app_class = argv[c + 1];
        c++;
        break;

      case 'a': // Build archive option
        if (state != UNSET) {
          std::cerr << "FATAL: option -l/-a cannot be used at the same time."
                    << std::endl;
          return 1;
        }

#ifdef DEBUG
        std::cout << "DEBUG: Compiling archive " << argv[c + 1]
                  << std::endl;
#endif /* DEBUG */
				out_file = argv[c + 1];
        state = BUILD_ARCHIVE;
        c++;
        break;

      case '#': // Display a hash value used in the vtables.
        std::cout << "Hash value for \"" << argv[c + 1] << "\":\n  ";
        std::cout << itohex(hash(argv[c + 1])) << std::endl;
        return 0;

      case '?':
      case 'h': // Help option
        help();
#ifdef TESTING
        meat::test::summary();
#endif
        return 0;

      default: // Unknown option
        std::cerr << argv[0] << ": unknown option -" << argv[c][1]
                  << std::endl;
        return 1;
      }
    } else {
      source_files.push_back(argv[c]);
    }
  }

	/***************************
   * Compile the meat files.
   */
  try{
    // Make sure we have files to compile
    if (source_files.size() == 0)
      throw meat::Exception("No source files specified to compile");

    meat::data::Library::import("Grinder");

		switch (state) {
		case BUILD_LIBRARY:
			build_library_app();
			break;
		case BUILD_ARCHIVE:
			build_archive_app();
			break;
		default:
			throw meat::Exception("Options -l or -a were not specified, "
														"I don't know what to compile");
			break;
		}
  } catch (std::exception &err) {
    library = NULL;
    std::cerr << "UNCAUGHT EXCEPTION: " << err.what() << std::endl;

#ifdef TESTING
    meat::test::summary();
#endif

    return 1;
  }

#ifdef TESTING
  meat::test::summary();
#endif

  return 0;
}
