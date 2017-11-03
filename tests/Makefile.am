#                                                           -*- Makefile.am -*-

TESTS = memory_tests tests.mapp libtests.mlib hello.mlib
LOG_COMPILER = @top_srcdir@/bin/meat
AM_LOG_FLAGS = -i ../lib/.libs/

SUFFIXES = .meat .mlib

EXTRA_DIST = UnitTest.meat Test.Characters.meat \
			 Test.Object.meat Test.Class.meat \
			 Test.BlockContext \
			 Test.Null.meat Test.Exception.meat \
			 Test.Boolean.meat Test.Numeric.meat \
			 Test.Integer.meat Test.Number.meat \
			 Test.Text.meat Test.Index.meat Test.List.meat Test.Data.meat \
			 Test.Application.meat Test.Compiler.meat \
			 tests.meat data.meat Console.meat builtin.meat Datastore.meat \
			 grinder.meat
CLEANFILES = UnitTest.mlib Test.Characters.mlib \
			 Test.Object.mlib Test.Class.mlib \
			 Test.BlockContext.mlib \
			 Test.Null.mlib Test.Exception.mlib \
			 Test.Boolean.mlib Test.Numeric.mlib \
			 Test.Number.mlib Test.Integer.mlib \
			 Test.Text.mlib Test.Index.mlib Test.List.mlib Test.Data.mlib \
			 Test.Application.mlib Test.Compiler.mlib \
			 builtin.cpp grinder.cpp Datastore.cpp \
			 data.mar tests.mapp hello.mlib libtests.mlib
MEATDEPS = @top_srcdir@/bin/meat-grinder @top_srcdir@/bin/meat \
	@top_srcdir@/src/libmeat.la

check_PROGRAMS = memory_tests

memory_tests_SOURCES = memory_tests.cpp
memory_tests_CPPFLAGS = -I@top_srcdir@/include
memory_tests_LDADD = @top_srcdir@/src/libmeat.la
memory_tests_LDFLAGS = -rdynamic

.meat.mlib:
	@top_srcdir@/bin/meat-grinder -i ../lib/.libs/ $<

.meat.cpp:
	@top_srcdir@/bin/meat-grinder -i ../lib/.libs/ $<
	@./fixnames.sh $*.cpp

UnitTest.mlib: $(MEATDEPS) @top_srcdir@/lib/Console.la
Test.Characters.mlib: UnitTest.mlib $(MEATDEPS)
Test.Object.mlib: UnitTest.mlib $(MEATDEPS)
Test.Class.mlib: UnitTest.mlib $(MEATDEPS)
Test.BlockContext.mlib: UnitTest.mlib $(MEATDEPS)
Test.Null.mlib: UnitTest.mlib $(MEATDEPS)
Test.Boolean.mlib: UnitTest.mlib $(MEATDEPS)
Test.Exception.mlib: UnitTest.mlib $(MEATDEPS)
Test.Numeric.mlib: UnitTest.mlib $(MEATDEPS)
Test.Integer.mlib: UnitTest.mlib $(MEATDEPS)
Test.Number.mlib: UnitTest.mlib $(MEATDEPS)
Test.Text.mlib: UnitTest.mlib $(MEATDEPS)
Test.Index.mlib: UnitTest.mlib $(MEATDEPS)
Test.List.mlib: UnitTest.mlib $(MEATDEPS)
Test.Data.mlib: UnitTest.mlib $(MEATDEPS)
Test.Application.mlib: UnitTest.mlib $(MEATDEPS)
Test.Compiler.mlib: UnitTest.mlib $(MEATDEPS)
Console.cpp: $(MEATDEPS)
builtin.cpp: $(MEATDEPS)
Datastore.cpp: $(MEATDEPS)
grinder.cpp: $(MEATDEPS)

data.mar: Test.Data.mlib
	@top_srcdir@/bin/meat-grinder -i ../lib/.libs/ -s data.meat

tests.mapp: $(MEATDEPS)
tests.mapp: UnitTest.mlib
tests.mapp: Test.Number.mlib Test.Text.mlib
tests.mapp: Test.Index.mlib Test.Data.mlib Test.Compiler.mlib
tests.mapp: builtin.cpp Datastore.cpp grinder.cpp
tests.mapp: data.mar
tests.mapp: tests.meat
	@top_srcdir@/bin/meat-grinder -i ../lib/.libs/ -s tests.meat

libtests.mlib: $(MEATDEPS)
libtests.mlib: UnitTest.mlib
libtests.mlib: Test.Characters.mlib
libtests.mlib: Test.Object.mlib Test.Class.mlib
libtests.mlib: Test.BlockContext.mlib
libtests.mlib: Test.Null.mlib Test.Exception.mlib
libtests.mlib: Test.Boolean.mlib Test.Numeric.mlib
libtests.mlib: Test.Integer.mlib
libtests.mlib: Test.List.mlib
libtests.mlib: Test.Application.mlib
libtests.mlib: libtests.meat
	@top_srcdir@/bin/meat-grinder -i ../lib/.libs/ libtests.meat

hello.mlib: hello.meat
	@top_srcdir@/bin/meat-grinder -i ../lib/.libs/ -c Hello hello.meat
