#                                                           -*- Makefile.am -*-

TESTS = tests.mlib hello.mlib
LOG_COMPILER = @top_srcdir@/bin/meat
AM_LOG_FLAGS = -i ../lib/.libs/

SUFFIXES = .meat .mlib

EXTRA_DIST = UnitTest.meat Test.Characters.meat \
			 Test.Object.meat Test.Class.meat \
			 Test.BlockContext.meat \
			 Test.Null.meat Test.Exception.meat \
			 Test.Boolean.meat Test.Numeric.meat \
			 Test.Integer.meat Test.Number.meat \
			 Test.Text.meat Test.Index.meat Test.List.meat Test.Data.meat \
			 Test.Application.meat Test.Compiler.meat \
			 tests.meat data.meat \
			 builtin.meat grinder.meat \
			 fix-builtin.pl fix-grinder.pl
CLEANFILES = UnitTest.mlib Test.Characters.mlib \
			 Test.Object.mlib Test.Class.mlib \
			 Test.BlockContext.mlib \
			 Test.Null.mlib Test.Exception.mlib \
			 Test.Boolean.mlib Test.Numeric.mlib \
			 Test.Number.mlib Test.Integer.mlib \
			 Test.Text.mlib Test.Index.mlib Test.List.mlib Test.Data.mlib \
			 Test.Application.mlib Test.Compiler.mlib \
			 builtin.cpp grinder.cpp \
			 data.mar tests.mapp hello.mlib libtests.mlib
MEATDEPS = @top_srcdir@/bin/meat-grinder @top_srcdir@/bin/meat \
	@top_srcdir@/src/libmeat.la

.meat.mlib:
	@top_srcdir@/bin/meat-grinder -i ../lib/.libs/ $<

.meat.cpp:
	@top_srcdir@/bin/meat-grinder -i ../lib/.libs/ $<

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
Test.List.mlib: UnitTest.mlib $(MEATDEPS)
Test.Application.mlib: UnitTest.mlib $(MEATDEPS)
builtin.cpp: $(MEATDEPS)
grinder.cpp: $(MEATDEPS)

data.mar: Test.Data.mlib
	@top_srcdir@/bin/meat-grinder -i ../lib/.libs/ -s data.meat

tests.mlib: $(MEATDEPS)
tests.mlib: UnitTest.mlib
tests.mlib: Test.Characters.mlib
tests.mlib: Test.Object.mlib Test.Class.mlib
tests.mlib: Test.BlockContext.mlib
tests.mlib: Test.Null.mlib Test.Exception.mlib
tests.mlib: Test.Boolean.mlib Test.Numeric.mlib
tests.mlib: Test.Integer.mlib
tests.mlib: Test.Text.mlib
tests.mlib: Test.List.mlib
tests.mlib: Test.Application.mlib
tests.mlib: builtin.cpp grinder.cpp
tests.mlib: tests.meat
	@top_srcdir@/bin/meat-grinder -i ../lib/.libs/ tests.meat

hello.mlib: hello.meat
	@top_srcdir@/bin/meat-grinder -i ../lib/.libs/ hello.meat
