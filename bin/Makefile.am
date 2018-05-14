#                                                           -*- Makefile.am -*-

bin_PROGRAMS = meat meat-grinder

meat_SOURCES = meat.cpp getopt.c getopt.h
meat_CPPFLAGS = -I@top_srcdir@/include
meat_LDADD = @top_srcdir@/src/libmeat.la
meat_LDFLAGS = -rdynamic
if TESTING
meat_LDADD += @top_srcdir@/tests/libtesting.a
endif


meat_grinder_SOURCES = grinder.cpp getopt.c getopt.h dump.cpp dump.h
meat_grinder_CPPFLAGS = -I@top_srcdir@/include
meat_grinder_LDADD = @top_srcdir@/src/libmeat.la
meat_grinder_LDFLAGS = -rdynamic
if TESTING
meat_grinder_LDADD += @top_srcdir@/tests/libtesting.a
endif
