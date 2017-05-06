#                                                           -*- Makefile.am -*-

# Some common information for the builds
MEAT_SRCS = meat.cpp utilities.cpp builtin.cpp datastore.cpp execute.cpp
COMPILER_SRCS = compiler.cpp comp_utils.cpp lcompiler.cpp acompiler.cpp ast.cpp

# Shared libraries
lib_LTLIBRARIES = libmeat.la

libmeat_la_SOURCES = $(MEAT_SRCS)
libmeat_la_CPPFLAGS = -I@top_srcdir@/include
libmeat_la_LDFLAGS = -release 0.1.0
