#                                                           -*- Makefile.am -*-

# Shared libraries
lib_LTLIBRARIES = libmeat.la

libmeat_la_SOURCES = meat.cpp utilities.cpp builtin.cpp datastore.cpp \
	execute.cpp
libmeat_la_CPPFLAGS = -I@top_srcdir@/include
libmeat_la_LDFLAGS = -release $(VERSION)
