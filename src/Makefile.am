#                                                           -*- Makefile.am -*-

# Shared libraries
lib_LTLIBRARIES = libmeat.la

libmeat_la_SOURCES = meat.cpp utilities.cpp builtin.cpp datastore.cpp \
	execute.cpp memory.cpp
libmeat_la_CPPFLAGS = -I@top_srcdir@/include
libmeat_la_LDFLAGS = -release $(VERSION)

# Testing
TESTS = memory-tests

check_PROGRAMS = memory-tests

memory_tests_SOURCES = memory-tests.cpp
memory_tests_CPPFLAGS = -I@top_srcdir@/include
memory_tests_LDADD = @top_srcdir@/src/libmeat.la
memory_tests_LDFLAGS = -rdynamic
