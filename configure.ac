
#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.57])
AC_INIT([hello], [0.1], [diego_hachmann@hotmail.com])
AM_INIT_AUTOMAKE([-Werror foreign])
AC_CONFIG_SRCDIR([src/attach/attach.c])
AC_CONFIG_HEADERS([config.h])

PATH_ATTACH=/home/brtablet/attach
export DEST=/usr/X11R6

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
PKG_CHECK_MODULES(GTK, [gtk+-2.0 >= 2.12])

# Defines Example
#AC_DEFINE(SYSFS_DATA, "/sys/devices/platform/vms/sysfs_data", "Data from user to driver)

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h sys/time.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([clock_gettime gettimeofday memmove memset regcomp])

AC_CONFIG_FILES([Makefile
                src/attach/Makefile
					 src/calibrator/Makefile
					 scripts/Makefile])

AC_OUTPUT

