#                                                           -*- Makefile.am -*-

TESTS = tests.mapp libtests.mlib hello.mlib
LOG_COMPILER = @top_srcdir@/bin/meat
AM_LOG_FLAGS = -i ../lib/.libs/

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

.meat.mlib:
	@top_srcdir@/bin/meat-grinder -i ../lib/.libs/ $<

.meat.cpp:
	@top_srcdir@/bin/meat-grinder -i ../lib/.libs/ $<
	@./fixnames.sh $*.cpp

Test.Characters.mlib: UnitTest.mlib @top_srcdir@/bin/meat-grinder @top_srcdir@/bin/meat
Test.Object.mlib: UnitTest.mlib @top_srcdir@/bin/meat-grinder @top_srcdir@/bin/meat
Test.Class.mlib: UnitTest.mlib @top_srcdir@/bin/meat-grinder @top_srcdir@/bin/meat
Test.BlockContext.mlib: UnitTest.mlib @top_srcdir@/bin/meat-grinder @top_srcdir@/bin/meat
Test.Null.mlib: UnitTest.mlib @top_srcdir@/bin/meat-grinder @top_srcdir@/bin/meat
Test.Boolean.mlib: UnitTest.mlib @top_srcdir@/bin/meat-grinder @top_srcdir@/bin/meat
Test.Exception.mlib: UnitTest.mlib @top_srcdir@/bin/meat-grinder @top_srcdir@/bin/meat
Test.Numeric.mlib: UnitTest.mlib @top_srcdir@/bin/meat-grinder @top_srcdir@/bin/meat
Test.Integer.mlib: UnitTest.mlib @top_srcdir@/bin/meat-grinder @top_srcdir@/bin/meat
Test.Number.mlib: UnitTest.mlib @top_srcdir@/bin/meat-grinder @top_srcdir@/bin/meat
Test.Text.mlib: UnitTest.mlib @top_srcdir@/bin/meat-grinder @top_srcdir@/bin/meat
Test.Index.mlib: UnitTest.mlib @top_srcdir@/bin/meat-grinder @top_srcdir@/bin/meat
Test.List.mlib: UnitTest.mlib @top_srcdir@/bin/meat-grinder @top_srcdir@/bin/meat
Test.Data.mlib: UnitTest.mlib @top_srcdir@/bin/meat-grinder @top_srcdir@/bin/meat
Test.Application.mlib: UnitTest.mlib @top_srcdir@/bin/meat-grinder
Test.Application.mlib: @top_srcdir@/bin/meat
Test.Compiler.mlib: UnitTest.mlib @top_srcdir@/bin/meat-grinder
Test.Compiler.mlib: @top_srcdir@/bin/meat
Console.cpp: @top_srcdir@/bin/meat-grinder @top_srcdir@/bin/meat
builtin.cpp: @top_srcdir@/bin/meat-grinder @top_srcdir@/bin/meat
Datastore.cpp: @top_srcdir@/bin/meat-grinder @top_srcdir@/bin/meat
grinder.cpp: @top_srcdir@/bin/meat-grinder @top_srcdir@/bin/meat

data.mar: Test.Data.mlib
	@top_srcdir@/bin/meat-grinder -i ../lib/.libs/ -s data.meat

tests.mapp: @top_srcdir@/bin/meat-grinder @top_srcdir@/bin/meat
tests.mapp: UnitTest.mlib Test.Application.mlib
tests.mapp: Test.Number.mlib Test.Text.mlib
tests.mapp: Test.Index.mlib Test.Data.mlib Test.Compiler.mlib
tests.mapp: builtin.cpp Datastore.cpp grinder.cpp
tests.mapp: data.mar
tests.mapp: tests.meat
	@top_srcdir@/bin/meat-grinder -i ../lib/.libs/ -s tests.meat

libtests.mlib: @top_srcdir@/bin/meat-grinder @top_srcdir@/bin/meat
libtests.mlib: Test.Characters.mlib
libtests.mlib: Test.Object.mlib Test.Class.mlib
libtests.mlib: Test.BlockContext.mlib
libtests.mlib: Test.Null.mlib Test.Exception.mlib
libtests.mlib: Test.Boolean.mlib Test.Numeric.mlib
libtests.mlib: Test.Integer.mlib
libtests.mlib: Test.List.mlib
libtests.mlib: libtests.meat
	@top_srcdir@/bin/meat-grinder -i ../lib/.libs/ -c Tests libtests.meat

hello.mlib: hello.meat
	@top_srcdir@/bin/meat-grinder -i ../lib/.libs/ -c Hello hello.meat
