#                                                           -*- Makefile.am -*-
#
#  This builds optional libraries that are included with the script virtual
# machine.
#
# Written by Ron R Wills

# Shared libraries
pkglib_LTLIBRARIES = Compiler.la Console.la

Console_la_SOURCES = console.cpp
Console_la_CPPFLAGS = -I@top_srcdir@/include
Console_la_LIBADD = @top_srcdir@/src/libmeat.la
Console_la_LDFLAGS = -module -avoid-version -shared

Compiler_la_SOURCES = compiler.cpp comp_utils.cpp lcompiler.cpp \
	acompiler.cpp ast.cpp ast.h
Compiler_la_CPPFLAGS = -I@top_srcdir@/include
Compiler_la_LIBADD = @top_srcdir@/src/libmeat.la
Compiler_la_LDFLAGS = -module -avoid-version -shared
