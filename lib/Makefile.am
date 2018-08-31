#                                                           -*- Makefile.am -*-
#
#  This builds optional libraries that are included with the script virtual
# machine.
#
# Written by Ron R Wills

# Shared libraries
pkglib_LTLIBRARIES = Grinder.la
EXTRA_DIST = Console.cpp Math.cpp

Grinder_la_SOURCES = compiler.cpp comp_utils.cpp library.cpp \
	interpreter.cpp Grinder.cpp ast.cpp ast.h compiler.h
Grinder_la_CPPFLAGS = -I@top_srcdir@/include
Grinder_la_LIBADD = @top_srcdir@/src/libmeat.la
Grinder_la_LDFLAGS = -module -avoid-version -shared

.meat.cpp:
	@top_srcdir@/bin/meat-grinder -i ../lib/.libs/ $<
