#                                                              -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#
# Copyright (c) 2017 Ron R Wills <ron.rwsoft@gmail.com>
#
# This file is part of Meat.
#
# Meat is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Meat is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Meat.  If not, see <http://www.gnu.org/licenses/>.

AC_PREREQ([2.68])
AC_INIT([meat], [0.2.0], [ron.rwsoft@gmail.com])
AM_INIT_AUTOMAKE
AM_SILENT_RULES
AC_CONFIG_SRCDIR([src/meat.cpp])

# Platform detection.
AC_CANONICAL_HOST

# Some build options.
AC_ARG_ENABLE([testing],
  [AS_HELP_STRING([--enable-testing],
    [compile in testing code (default is no)])],
      [case "${enableval}" in
        yes) testing=true; AC_DEFINE(TESTING) ;;
        no)  testing=false ;;
        *) AC_MSG_ERROR([bad value ${enableval} for --enable-testing]) ;;
       esac], [testing=false])
AM_CONDITIONAL([TESTING], [test x$testing = xtrue])

AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug],
    [compile in debugging code (default is no)])],
      [case "${enableval}" in
        yes) debug=true; AC_DEFINE(DEBUG) ;;
        no)  debug=false ;;
        *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
       esac], [debug=false])
AM_CONDITIONAL([DEBUG], [test x$debug = xtrue])

AC_DEFINE([MEAT_BUILD])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_CHECK_PROG([lmk], [latexmk], [yes], [no])
AM_CONDITIONAL([LATEXMK], [test x$lmk = xyes])

# Checks for libraries.
AC_CHECK_LIB(m, pow)
case $host_os in
  linux*)
    AC_CHECK_LIB([dl], [dlopen], ,
         [AC_MSG_ERROR([Library dl missing for your platform])])
    ;;
  mingw*)
    ;;
  *)
    # Default Case
    AC_MSG_ERROR([Your platform is not currently supported])
    ;;
esac

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_STRTOD
AC_CHECK_FUNCS([pow sqrt])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 bin/Makefile
                 lib/Makefile
                 tests/Makefile
                 include/Makefile
	               docs/Makefile])
AC_OUTPUT
