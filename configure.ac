#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([guile-databuf], [0.1], [BUGREPORT@ADDRESS])
AC_CONFIG_SRCDIR([databuf/databuf.c])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE
LT_INIT

# Checks for programs.
AC_PROG_CC

PKG_CHECK_MODULES([GUILE], [guile-2.2])
GUILE_SITE_DIR

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 databuf/Makefile])
AC_OUTPUT
