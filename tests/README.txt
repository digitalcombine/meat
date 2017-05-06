The Testing Framework

 Here we test as much of the base functionallity of the language as we can.
There are several things we don't test, because it would be difficult to do
so and the language wouldn't work if these areas failed. Context and 
BlockContext are perfect examples of this.

 Also with what is provided here, many of the internals can be tested by the
configure option --enable-testing. This should not be enabled for a release
build.
