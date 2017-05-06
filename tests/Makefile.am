#                                                           -*- Makefile.am -*-

TESTS = tests.mapp
LOG_COMPILER = @top_srcdir@/bin/meat
AM_LOG_FLAGS = -i ../lib/.libs/

EXTRA_DIST = UnitTest.meat Test.Application.meat Test.Boolean.meat \
			 Test.Exception.meat Test.Object.meat Test.Class.meat \
			 Test.Numeric.meat Test.Number.meat Test.Integer.meat \
			 Test.Text.meat Test.Index.meat Test.List.meat Test.Data.meat \
			 Test.Compiler.meat \
			 tests.meat data.meat Console.meat builtin.meat Datastore.meat
CLEANFILES = UnitTest.mlib Test.Application.mlib Test.Boolean.mlib \
			 Test.Exception.mlib Test.Object.mlib Test.Class.mlib \
			 Test.Numeric.mlib Test.Number.mlib Test.Integer.mlib \
			 Test.Text.mlib Test.Index.mlib Test.List.mlib Test.Data.mlib \
			 Test.Compiler.mlib \
			 Console.cpp builtin.cpp Datastore.cpp \
			 data.mar tests.app

.meat.mlib:
	@top_srcdir@/bin/grinder -i ../lib/.libs/ -l $* $<

.meat.cpp:
	@top_srcdir@/bin/grinder -i ../lib/.libs/ -l $* $<
	@cat $*.cpp | sed -e 's/ᶲ//g' > $*.cpp.fixed
	@mv -f $*.cpp.fixed $*.cpp

Test.Application.mlib: UnitTest.mlib
Test.Boolean.mlib: UnitTest.mlib
Test.Exception.mlib: UnitTest.mlib
Test.Object.mlib: UnitTest.mlib
Test.Class.mlib: UnitTest.mlib
Test.Numeric.mlib: UnitTest.mlib
Test.Number.mlib: UnitTest.mlib
Test.Integer.mlib: UnitTest.mlib
Test.Text.mlib: UnitTest.mlib
Test.Index.mlib: UnitTest.mlib
Test.List.mlib: UnitTest.mlib
Test.Data.mlib: UnitTest.mlib
Test.Compiler.mlib: UnitTest.mlib

data.mar: Test.Data.mlib
	@top_srcdir@/bin/grinder -i ../lib/.libs/ -a data.mar data.meat

tests.mapp: UnitTest.mlib Test.Application.mlib
tests.mapp: Test.Boolean.mlib Test.Exception.mlib Test.Object.mlib
tests.mapp: Test.Class.mlib
tests.mapp: Test.Numeric.mlib Test.Number.mlib Test.Integer.mlib Test.Text.mlib
tests.mapp: Test.Index.mlib Test.List.mlib Test.Data.mlib Test.Compiler.mlib
tests.mapp: Console.cpp builtin.cpp Datastore.cpp
tests.mapp: data.mar
tests.mapp: tests.meat
	@top_srcdir@/bin/grinder -i ../lib/.libs/ -a tests.mapp tests.meat
