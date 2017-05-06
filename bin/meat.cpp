/*                                                                  -*- c++ -*-
 * A Meaty runtime engine.
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
 */

#include <meat.h>
#include <meat/datastore.h>

#include <iostream>
#include <exception>

#ifdef TESTING
#include <testing.h>
#endif

/** Display simple help for the -h option
 */
static void help() {
	std::cout << "Meat v" << VERSION << "\n\n";
	std::cout << "meat [-i path] archive ...\n";
	std::cout << "meatc -h\n";
	std::cout << "  -i path      Include path to the library search\n";
	std::cout << "  -h           Displays this help" << std::endl;
}

/** Here's where is all starts.
 */
int main(int argc, const char *argv[]) {

#ifdef DEBUG
	std::cout << "Meat " << VERSION << std::endl;
	std::cout << "  Debugging enabled" << std::endl;
#ifdef TESTING
	std::cout << "  Testing Build" << std::endl;
#endif
#endif

	const char *filename = NULL;

	meat::initialize(argc, argv); // Initialize the runtime environment.

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

			case 'h': // Help option
				help();
#ifdef TESTING
				meat::Test::summary();
#endif
				return 0;

			default: { // Unknown option
#ifdef DEBUG
				for (int i = 0; i < argc; i++) {
					std::cerr << argv[i] << " ";
				}
				std::cerr << std::endl;
#endif
				std::cerr << "FATAL: unknown option -" << argv[c][1] << std::endl;
				return 1;
			}
			}
		} else {
			filename = argv[c];
		}
	}

	try {

		/* Open the archive and get the object from it. */
		meat::data::Archive archive(filename);
		meat::Reference app = archive.get_object();

		/* Make sure the object is an application. */
		if (!app->is_type("Application")) {
			throw meat::Exception("Store does not contain an Application");
		}

#ifdef DEBUG
		std::cout << "DEBUG: Entering application..." << std::endl;
#endif /* DEBUG */

		/* Message the application and get things rolling. */
		meat::Reference context = meat::message(app, "entry",
																							meat::Null());
		meat::Reference result = meat::execute(context);

		/*  Attempt to return the return value from the entry method. If the
		 * value return from entry is not an integer then we return 1 (error).
		 * If no value was returned then we return 0 (success).
		 */
		if (!result.is_null() || result == meat::Null()) {
#ifdef TESTING
			meat::Test::summary();
#endif
			try { return (result->to_int()); }
			catch (...) { return 1; }
		}

	} catch (std::exception &err) {
		/* Opps an uncaught exception. */
		std::cerr << "UNCAUGHT EXCEPTION: " << err.what() << std::endl;

#ifdef TESTING
		meat::Test::summary();
#endif
		return 1;
	}

	/* We should never really get here, but incase we do... */
#ifdef TESTING
	meat::Test::summary();
#endif
	return 0;
}
