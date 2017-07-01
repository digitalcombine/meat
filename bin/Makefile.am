#                                                           -*- Makefile.am -*-

bin_PROGRAMS = meat grinder

meat_SOURCES = meat.cpp
if TESTING
meat_SOURCES += testing.cpp misc.cpp
endif
meat_CPPFLAGS = -I@top_srcdir@/include
meat_LDADD = @top_srcdir@/src/libmeat.la
meat_LDFLAGS = -rdynamic

grinder_SOURCES = grinder.cpp
if TESTING
grinder_SOURCES += testing.cpp misc.cpp
endif
grinder_CPPFLAGS = -I@top_srcdir@/include
grinder_LDADD = @top_srcdir@/src/libmeat.la @top_srcdir@/lib/Grinder.la
grinder_LDFLAGS = -rdynamic
