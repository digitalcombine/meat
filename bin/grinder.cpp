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
 * TODO
 *  Various compiler options for compiling c++ shared libraries.
 *    -c c++ compiler
 *    -x c++ options
 *    -l linker
 *    -z linker options
 */

#include <meat.h>
#include <meat/compiler.h>
#include <meat/utilities.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#ifdef TESTING
#include <testing.h>
#endif

meat::Reference library;

#define LIBRARY ((meat::compiler::LibraryBuilder &)(*(library)))

// This is found in src/meat.cpp
extern void (*class_compiler)(meat::Reference &super, const char *cls_name,
															const char *cls_body);

/** Display simple help for the -h option
 */
static void help() {
	std::cout << "Meat Compiler v" << VERSION << "\n\n";
	std::cout << "grinder [-l libname|-a archname] [-i path] source ...\n";
	std::cout << "grinder -h\n";
	std::cout << "  -i path      Include path to the library search\n";
	std::cout << "  -l libname   Compile a library\n";
	std::cout << "  -a archname  Compile an archive\n";
	std::cout << "  -# text      Return a hash value for the text\n";
	std::cout << "  -h           Displays this help" << std::endl;
}

/** Used to add functionallity to the Class subClass method when the compiler
 * is initialized. If the compiler is not initialized then the Class subClass
 * method should just raise an exception.
 */
static void classbuilder_int(meat::Reference &super, const char *cls_name,
														 const char *cls_body) {
	meat::Reference cb = LIBRARY.new_class(super, cls_name);
	std::string body(cls_body);
	std::istringstream is(body);

	((meat::compiler::ClassBuilder &)(*cb)).execute(is);

	((meat::compiler::ClassBuilder &)(*cb)).create_class();
}

/** Used for the Library import method when the compiler is initialized.
 */
static void libcomp_import(const char *name) {
	LIBRARY.add_import(name);
}

/******************************************************************************
 * Script Compiler Entry
 */
int main(int argc, const char *argv[]) {

	std::deque<const char *> meat_files;
	std::string lib_name;
	meat::compiler::ArchiveBuilder arch_builder;

	/* The compiler state specified by the command line options. */
	static enum {
		UNSET, BUILD_LIB, BUILD_ARCHIVE
	} state = UNSET;

#ifdef DEBUG
	std::cout << "Meat Compiler" << std::endl;
	std::cout << "  Debugging enabled" << std::endl;
#ifdef TESTING
	std::cout << "  Testing Build" << std::endl;
#endif
#endif

	meat::initialize(argc, argv);

#ifdef TESTING
	meat::Test::run_tests();
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
					std::cerr << "FATAL: option -l/-a cannot be used more that once."
										<< std::endl;
					return 1;
				}

#ifdef DEBUG
				std::cout << "DEBUG: Compiling library file " << argv[c + 1] << ".lib"
									<< std::endl;
#endif /* DEBUG */
				lib_name = argv[c + 1];
				state = BUILD_LIB;
				c++;
				break;

			case 'a': // Build archive option
				if (state != UNSET) {
					std::cerr << "FATAL: option -l/-a cannot be used more that once."
										<< std::endl;
					return 1;
				}

#ifdef DEBUG
				std::cout << "DEBUG: Compiling archive " << argv[c + 1]
									<< std::endl;
#endif /* DEBUG */
				arch_builder.create(argv[c + 1]);
				state = BUILD_ARCHIVE;
				c++;
				break;

			case '#': // Display a hash value used in the vtables.
				std::cout << "Hash value for \"" << argv[c + 1] << "\":\n  ";
				std::cout << std::setw(8) << std::setfill('0') << std::setbase(16)
									<< std::showbase;
				std::cout << hash(argv[c + 1]) << std::endl;
				return 0;

			case 'h': // Help option
				help();
#ifdef TESTING
				meat::Test::summary();
#endif
				return 0;

			default: // Unknown option
				std::cerr << argv[0] << ": unknown option -" << argv[c][1]
									<< std::endl;
				return 1;
			}
		} else {
			meat_files.push_back(argv[c]);
		}
	}

	/***************************
	 * Compile the meat files.
	 */
	try{
		// Make sure we have files to compile
		if (meat_files.size() == 0)
			throw meat::Exception("No files specified to compile");

		meat::data::Library::import("Compiler");

		// Go through each file and compile them
		for (std::deque<const char *>::iterator it = meat_files.begin();
				 it != meat_files.end();
				 ++it) {

#ifdef DEBUG
			std::cout << "DEBUG: Compiling " << *it << std::endl;
#endif /* DEBUG */

			std::ifstream meat_file;

			meat_file.open(*it, std::ios::in);

			if (meat_file.is_open()) {
				switch (state) {
				case BUILD_LIB: {
					library = new meat::compiler::LibraryBuilder(lib_name.c_str());
					meat::data::compiler_import = libcomp_import;
					class_compiler = classbuilder_int;

					LIBRARY.execute(meat_file);
					break;
				}
				case BUILD_ARCHIVE:
					arch_builder.execute(meat_file);
					break;
				case UNSET:
					throw meat::Exception("Option -l or -a not specified, "
																 "I don't know what to compile");
					break;
				}

				meat_file.close();
			} else {
				std::cerr << "ERROR: Unable to open " << *it << std::endl;
				return 1;
			}
		}

		// Write all information to the files.
		switch (state) {
		case BUILD_LIB: {
			LIBRARY.write();
			/*  Remove the reference here or bad things happen if we leave it to
			 * the end of the program.
			 */
			library = NULL;
			break;
		}
		case BUILD_ARCHIVE:
			arch_builder.write();
			break;
		case UNSET: // Can't happen but we'll keep the compiler from complaining.
			break;
		}

	} catch (std::exception &err) {
		std::cerr << "UNCAUGHT EXCEPTION: " << err.what() << std::endl;

#ifdef TESTING
		meat::Test::summary();
#endif

		return 1;
	}

#ifdef TESTING
	meat::Test::summary();
#endif

	return 0;
}
