#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(rogueclone2s, 6.0, [http://homepage2.nifty.com/107steps/])
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h sgtty.h stdlib.h string.h strings.h sys/file.h sys/time.h termio.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_STRUCT_TM

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MEMCMP
AC_TYPE_SIGNAL
AC_FUNC_STAT
AC_FUNC_MALLOC
AC_CHECK_HEADERS([windows.h locale.h])
AC_CHECK_LIB([ncursesw], [initscr])
AC_CHECK_LIB([kernel32], [_sleep])
AC_CHECK_FUNCS([chdir getcwd getenv getlogin memset putenv setenv setlocale strchr sleep])
AC_CHECK_FUNCS([assume_default_colors attr_get])


AC_CONFIG_FILES([Makefile
                 doc/Makefile
                 man/Makefile
                 msg/Makefile
                 src/Makefile])

AC_ARG_ENABLE([color],
              AS_HELP_STRING([--enable-color],
                             [include color(default yes)]), ,)



if test "$enable_color" != "no"; then
AC_DEFINE([COLOR], , [Define if you have the 'COLOR'])
fi

AC_OUTPUT
