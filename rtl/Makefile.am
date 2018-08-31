#                                                           -*- Makefile.am -*-
#
#  This builds optional libraries that are included with the script virtual
# machine.
#
# Written by Ron R Wills

# Shared libraries
pkglib_LTLIBRARIES = Console.la Math.la

Console_la_SOURCES = Console.meat
Console_la_CPPFLAGS = -I@top_srcdir@/include
Console_la_LIBADD = @top_srcdir@/src/libmeat.la
Console_la_LDFLAGS = -module -avoid-version -shared

Math_la_SOURCES = Math.meat
Math_la_CPPFLAGS = -I@top_srcdir@/include
Math_la_LIBADD = @top_srcdir@/src/libmeat.la
Math_la_LDFLAGS = -module -avoid-version -shared

CLEANFILES = Console.cpp Math.cpp

.meat.cpp:
	@top_srcdir@/bin/meat-grinder -i ../lib/.libs/ $<
