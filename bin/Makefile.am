#                                                           -*- Makefile.am -*-

bin_PROGRAMS = meat meat-grinder

meat_SOURCES = meat.cpp getopt.c getopt.h
if TESTING
meat_SOURCES += testing.cpp misc.cpp
endif
meat_CPPFLAGS = -I@top_srcdir@/include
meat_LDADD = @top_srcdir@/src/libmeat.la
meat_LDFLAGS = -rdynamic

meat_grinder_SOURCES = grinder.cpp getopt.c getopt.h
if TESTING
meat_grinder_SOURCES += testing.cpp misc.cpp
endif
meat_grinder_CPPFLAGS = -I@top_srcdir@/include
meat_grinder_LDADD = @top_srcdir@/src/libmeat.la
meat_grinder_LDFLAGS = -rdynamic
