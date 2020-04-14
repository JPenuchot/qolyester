# Copyright (C) 2003-2008 Laboratoire de Recherche en Informatique

# This file is part of Qolyester.

# Qolyester is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.

# Qolyester is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA  02110-1301, USA.

# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(qolyester, 0.2, qolsr@lri.fr)
AM_INIT_AUTOMAKE([dist-bzip2])
AC_CONFIG_SRCDIR(src/daemon/main.cc)
AC_CONFIG_HEADERS(src/include/config.hh)

#AM_MAINTAINER_MODE

if test -r $srcdir/.version; then
	snapshot_version=`cat $srcdir/.version`
	AC_DEFINE_UNQUOTED(QOLYESTER_SNAPSHOT_VERSION,
	                   "$snapshot_version",
			   [Qolyester snapshot])
fi

AC_ARG_ENABLE(ipv4,
        AC_HELP_STRING([--enable-ipv4],
                       [use IPv4 instead of IPv6 (default=no)]),
        [if test "x$enableval" = xyes; then
		WITH_FAMILY=4
        elif test "x$enableval" = xno; then
		WITH_FAMILY=6
	else
		AC_MSG_ERROR([--enable-ipv4 expects yes or no])
        fi],
        [WITH_FAMILY=6])

AC_ARG_ENABLE(numprefix,
	AC_HELP_STRING([--enable-numprefix],
		       [use numerical prefix for HNA in IPv6 (default=no)]),
	[if test "x$enableval" = xyes; then
		WITH_NUMPREFIX=yes
	elif test "x$enableval" = xno; then
		WITH_NUMPREFIX=no
	else
		AC_MSG_ERROR([--enable-numprefix expects yes or no])
	fi],
	[WITH_NUMPREFIX=no])

AC_ARG_ENABLE(virtual,
        AC_HELP_STRING([--enable-virtual],
                       [create a virtual network to test (default=no)]),
        [if test "x$enableval" = xyes; then
		WITH_VIRTUAL=yes
	elif test "x$enableval" = xno; then
		WITH_VIRTUAL=no
	else
		AC_MSG_ERROR([--enable-virtual expects yes or no])
        fi],
	[WITH_VIRTUAL=no])

AC_ARG_ENABLE(debug,
	AC_HELP_STRING([--enable-debug],
		       [enable debugging features (default=yes)]),
	[if test "x$enableval" = xyes; then
		WITH_DEBUG=yes
	elif test "x$enableval" = xno; then
		WITH_DEBUG=no
	else
		AC_MSG_ERROR([--enable-debug expects yes or no])
	fi],
	[WITH_DEBUG=yes])

#
# Message emission intervals
#

AC_ARG_WITH(hello-interval,
	AC_HELP_STRING([--with-hello-interval=MILLISEC],
		       [default HELLO emission interval in ms (default = 2000)]),
	[if echo "$withval" | grep -v ['^[0-9]\+$'] > /dev/null; then
		AC_MSG_ERROR([--with-hello-interval expects a number])
	else
		AC_DEFINE_UNQUOTED(QOLYESTER_HELLO_INTERVAL, $withval, [HELLO emission interval])
	fi],
	[])

AC_ARG_WITH(tc-interval,
	AC_HELP_STRING([--with-tc-interval=MILLISEC],
		       [default TC emission interval in ms (default = 5000)]),
	[if echo "$withval" | grep -v ['^[0-9]\+$'] > /dev/null; then
		AC_MSG_ERROR([--with-tc-interval expects a number])
	else
		AC_DEFINE_UNQUOTED(QOLYESTER_TC_INTERVAL, $withval, [TC emission interval])
	fi],
	[])

AC_ARG_WITH(mid-interval,
	AC_HELP_STRING([--with-mid-interval=MILLISEC],
		       [default MID emission interval in ms (default = TC emission interval)]),
	[if echo "$withval" | grep -v ['^[0-9]\+$'] > /dev/null; then
		AC_MSG_ERROR([--with-mid-interval expects a number])
	else
		AC_DEFINE_UNQUOTED(QOLYESTER_MID_INTERVAL, $withval, [MID emission interval])
	fi],
	[])

AC_ARG_WITH(hna-interval,
	AC_HELP_STRING([--with-hna-interval=MILLISEC],
		       [default HNA emission interval in ms (default = TC emission interval)]),
	[if echo "$withval" | grep -v ['^[0-9]\+$'] > /dev/null; then
		AC_MSG_ERROR([--with-hna-interval expects a number])
	else
		AC_DEFINE_UNQUOTED(QOLYESTER_HNA_INTERVAL, $withval, [HNA emission interval])
	fi],
	[])

#
# Information holding times
#

AC_ARG_WITH(refresh-interval,
	AC_HELP_STRING([--with-refresh-interval=MILLISEC],
		       [default information refresh interval in ms (default = 2000)]),
	[if echo "$withval" | grep -v ['^[0-9]\+$'] > /dev/null; then
		AC_MSG_ERROR([--with-refresh-interval expects a number])
	else
		AC_DEFINE_UNQUOTED(QOLYESTER_REFRESH_INTERVAL, $withval, [Information refresh interval])
	fi],
	[])

AC_ARG_WITH(mid-hold-time,
	AC_HELP_STRING([--with-mid-hold-time=MILLISEC],
		       [default MID information holding time in ms (default = MID emission interval * 3)]),
	[if echo "$withval" | grep -v ['^[0-9]\+$'] > /dev/null; then
		AC_MSG_ERROR([--with-mid-hold-time expects a number])
	else
		AC_DEFINE_UNQUOTED(QOLYESTER_MID_HOLD_TIME, $withval, [MID holding time])
	fi],
	[])

AC_ARG_WITH(hna-hold-time,
	AC_HELP_STRING([--with-hna-hold-time=MILLISEC],
		       [default HNA information holding time in ms (default = HNA emission interval * 3)]),
	[if echo "$withval" | grep -v ['^[0-9]\+$'] > /dev/null; then
		AC_MSG_ERROR([--with-hna-hold-time expects a number])
	else
		AC_DEFINE_UNQUOTED(QOLYESTER_HNA_HOLD_TIME, $withval, [HNA holding time])
	fi],
	[])

AC_ARG_WITH(neighbor-hold-time,
	AC_HELP_STRING([--with-neighbor-hold-time=MILLISEC],
		       [default neighbor holding time in ms (default = refresh interval * 3)]),
	[if echo "$withval" | grep -v ['^[0-9]\+$'] > /dev/null; then
		AC_MSG_ERROR([--with-neighbor-hold-time expects a number])
	else
		AC_DEFINE_UNQUOTED(QOLYESTER_NEIGHB_HOLD_TIME, $withval, [Neighbor holding time])
	fi],
	[])

AC_ARG_WITH(topology-hold-time,
	AC_HELP_STRING([--with-topology-hold-time=MILLISEC],
		       [default topology holding time in ms (default = TC emission interval * 3)]),
	[if echo "$withval" | grep -v ['^[0-9]\+$'] > /dev/null; then
		AC_MSG_ERROR([--with-topology-hold-time expects a number])
	else
		AC_DEFINE_UNQUOTED(QOLYESTER_TOP_HOLD_TIME, $withval, [Topology holding time])
	fi],
	[])

AC_ARG_WITH(duplicates-hold-time,
	AC_HELP_STRING([--with-duplicates-hold-time=MILLISEC],
		       [default duplicates holding time in ms (default = 30000)]),
	[if echo "$withval" | grep -v ['^[0-9]\+$'] > /dev/null; then
		AC_MSG_ERROR([--with-duplicates-hold-time expects a number])
	else
		AC_DEFINE_UNQUOTED(QOLYESTER_DUP_HOLD_TIME, $withval, [Duplicates holding time])
	fi],
	[])

AC_ARG_WITH(tc-redundancy,
	AC_HELP_STRING([--with-tc-redundancy=mprsel|mprsel+mpr|all],
		       [default TC redundancy (default = mprsel)]),
	[if test x"$withval" != xmprsel     && \
	    test x"$withval" != xmprsel+mpr && \
            test x"$withval" != xall; then
		AC_MSG_ERROR([--with-tc-redundancy expects either one of mprsel, mprsel+mpr and all])
	else
		case x$withval in
			xmpr)		withval=mprselset;;
			xmprsel+mpr)	withval=mprselset_mprset;;
			xall)		withval=wholeset;;
		esac
		AC_DEFINE_UNQUOTED(QOLYESTER_TC_DEFAULT_REDUNDANCY, $withval, [Default TC redundancy])
	fi],
	AC_DEFINE(QOLYESTER_TC_DEFAULT_REDUNDANCY, [mprselset], [Default TC redundancy]))

AC_ARG_WITH(default-willingness,
	AC_HELP_STRING([--with-default-willingness=never|low|medium|high|always|NUM],
		       [default MPR willingness (default = medium)]),
	[if test x"$withval" != never   && \
            test x"$withval" != low     && \
            test x"$withval" != medium  && \
            test x"$withval" != high    && \
            test x"$withval" != always  && \
            echo "$withval" | grep -v '^[0-7]$'; then
		AC_MSG_ERROR([--with-default-willingness expects either never|low|medium|high|always or a number between 0 and 7])
	else
		case x$withval in
			xnever)		withval=WILL_NEVER;;
			xlow)		withval=WILL_LOW;;
			xmedium)	withval=WILL_DEFAULT;;
			xhigh)		withval=WILL_HIGH;;
			xalways)	withval=WILL_ALWAYS;;
		esac
		AC_DEFINE_UNQUOTED(QOLYESTER_DEFAULT_WILLINGNESS, $withval, [Default MPR willingness])
	fi],
	AC_DEFINE(QOLYESTER_DEFAULT_WILLINGNESS, [WILL_DEFAULT], [Default MPR willingness]))

AC_ARG_WITH(mpr-coverage,
	AC_HELP_STRING([--with-mpr-coverage=NUM],
		       [default MPR coverage (default = 1)]),
	[if echo "$withval" | grep -v ['^[0-9]\+$'] > /dev/null; then
		AC_MSG_ERROR([--with-mpr-coverage expects a number])
	else
		AC_DEFINE_UNQUOTED(QOLYESTER_MPR_DEFAULT_COVERAGE, $withval, [Default MPR coverage])
	fi],
	AC_DEFINE(QOLYESTER_MPR_DEFAULT_COVERAGE, [1], [Default MPR coverage]))

#
# Other internal default times
#

AC_ARG_WITH(max-jitter-time,
	AC_HELP_STRING([--with-max-jitter-time=MILLISEC],
		       [default maximum jitter time in ms (default = HELLO emission interval / 4)]),
	[if echo "$withval" | grep -v ['^[0-9]\+$'] > /dev/null; then
		AC_MSG_ERROR([--with-max-jitter-time expects a number])
	else
		AC_DEFINE_UNQUOTED(QOLYESTER_MAX_JITTER_TIME, $withval, [Maximum jitter time])
	fi],
	[])

AC_ARG_WITH(dump-interval,
	AC_HELP_STRING([--with-dump-interval=MILLISEC],
		       [default state dump interval in ms (default = 1000)]),
	[if echo "$withval" | grep -v ['^[0-9]\+$'] > /dev/null; then
		AC_MSG_ERROR([--with-dump-interval expects a number])
	else
		AC_DEFINE_UNQUOTED(QOLYESTER_DUMP_INTERVAL, $withval, [State dump interval])
	fi],
	[])

AC_ARG_WITH(all-routers,
	AC_HELP_STRING([--with-all-routers],
		       [use "all routers" IPv6 multicast addressing instead of "all nodes"]),
	[if test "x$withval" = xyes; then
		AC_DEFINE(QOLYESTER_ALL_ROUTERS, [], [Use "all routers" IPv6 multicast type])
	elif test "x$withval" != xno; then
		AC_MSG_ERROR([--with-all-routers expects yes or no])
	fi],
	[])

AC_ARG_WITH(ll-mcast,
	AC_HELP_STRING([--with-ll-mcast],
		       [use link-local IPv6 multicast address in any case]),
	[if test "x$withval" = xyes; then
		AC_DEFINE(QOLYESTER_LL_MCAST, [], [Use link-local IPv6 multicast address in any case])
	elif test "x$withval" != xno; then
		AC_MSG_ERROR([--with-ll-mcast expects yes or no])
	fi],
	[])

# AC_ARG_ENABLE(mid,
# 	AC_HELP_STRING([--enable-mid],
# 		       [enable multiple interfaces support (default=yes)]),
# 	[if test "x$enableval" = xyes; then
# 		WITH_MID=yes
# 	else
# 		WITH_MID=no
# 	fi],
# 	[WITH_MID=yes])

WITH_MID=yes

# AC_ARG_ENABLE(hna,
# 	AC_HELP_STRING([--enable-hna],
# 		       [enable external networks support (default=yes)]),
# 	[if test "x$enableval" = xyes; then
# 		WITH_HNA=yes
# 	else
# 		WITH_HNA=no
# 	fi],
# 	[WITH_HNA=yes])

WITH_HNA=yes

# AC_ARG_ENABLE(tcred,
# 	AC_HELP_STRING([--enable-tcred],
# 		       [enable TC redundancy (default=yes)]),
# 	[if test "x$enableval" = xyes; then
# 		WITH_TC_RED=yes
# 	else
# 		WITH_TC_RED=no
# 	fi],
# 	[WITH_TC_RED=yes])

WITH_TC_RED=yes

# AC_ARG_ENABLE(mprred,
# 	AC_HELP_STRING([--enable-mprred],
# 		       [enable MPR redundancy (default=yes)]),
# 	[if test "x$enableval" = xyes; then
# 		WITH_MPR_RED=yes
# 	else
# 		WITH_MPR_RED=no
# 	fi],
# 	[WITH_MPR_RED=yes])

WITH_MPR_RED=yes

AC_ARG_WITH(queue-size,
	AC_HELP_STRING([--with-queue-size=N],
		       [default size of waiting packets for output per interface (default = 10)]),
	[if echo "$withval" | grep -v ['^[0-9]\+$'] > /dev/null; then
		AC_MSG_ERROR([--with-queue-size expects a number])
	else
		AC_DEFINE_UNQUOTED(QOLYESTER_QUEUE_SIZE, $withval, [Default output queue size per interface])
	fi],
	[])

AC_ARG_ENABLE(linkhys,
	[AC_HELP_STRING([--enable-linkhys],
			[enable Link Hysteresis (default=yes)])],
	[if test "x$enableval" = xyes; then
		WITH_LINKHYS=yes
	elif test "x$enableval" = xno; then
		WITH_LINKHYS=no
	else
		AC_MSG_ERROR([--enable-linkhys expects yes or no])
	fi],
	[WITH_LINKHYS=yes])

AC_ARG_WITH(hello-grace,
	AC_HELP_STRING([--with-hello-grace=FLOAT],
		       [default HELLO interval grace (default = 1.2)]),
	[if echo "$withval" | grep -v ['^\(\(\([0-9]*\.\)\?[0-9]\+\)\|\([0-9]\+\(\.[0-9]*\)\?\)\)$'] > /dev/null; then
		AC_MSG_ERROR([--with-hello-grace expects a number])
	else
		AC_DEFINE_UNQUOTED(QOLYESTER_HELLO_GRACE, $withval, [HELLO interval grace])
	fi],
	[])

AC_ARG_WITH(hyst-threshold-high,
	AC_HELP_STRING([--with-hyst-threshold-high=FLOAT],
		       [default high threshold for link hysteresis (default = 0.8)]),
	[if echo "$withval" | grep -v ['^\(\(\([0-9]*\.\)\?[0-9]\+\)\|\([0-9]\+\(\.[0-9]*\)\?\)\)$'] > /dev/null; then
		AC_MSG_ERROR([--with-hyst-threshold-high expects a number])
	else
		AC_DEFINE_UNQUOTED(QOLYESTER_HYST_THRESHOLD_HIGH, $withval, [link hysteresis high threshold])
	fi],
	[])

AC_ARG_WITH(hyst-threshold-low,
	AC_HELP_STRING([--with-hyst-threshold-low=FLOAT],
		       [default low threshold for link hysteresis (default = 0.3)]),
	[if echo "$withval" | grep -v ['^\(\(\([0-9]*\.\)\?[0-9]\+\)\|\([0-9]\+\(\.[0-9]*\)\?\)\)$'] > /dev/null; then
		AC_MSG_ERROR([--with-hyst-threshold-low expects a number])
	else
		AC_DEFINE_UNQUOTED(QOLYESTER_HYST_THRESHOLD_LOW, $withval, [link hysteresis low threshold])
	fi],
	[])

AC_ARG_WITH(hyst-scaling,
	AC_HELP_STRING([--with-hyst-scaling=FLOAT],
		       [default scaling for link hysteresis (default = 0.5)]),
	[if echo "$withval" | grep -v ['^\(\(\([0-9]*\.\)\?[0-9]\+\)\|\([0-9]\+\(\.[0-9]*\)\?\)\)$'] > /dev/null; then
		AC_MSG_ERROR([--with-hyst-scaling expects a number])
	else
		AC_DEFINE_UNQUOTED(QOLYESTER_HYST_SCALING, $withval, [link hysteresis scaling])
	fi],
	[])

AC_SUBST_FILE(man_mid1)
AC_SUBST_FILE(man_mid2)

AC_SUBST_FILE(man_hna1)
AC_SUBST_FILE(man_hna2)

AC_SUBST_FILE(man_tcred)

AC_SUBST_FILE(man_mprred)

AC_SUBST_FILE(man_virtual1)
AC_SUBST_FILE(man_virtual2)

AC_SUBST_FILE(man_dirtypromischack)

AC_SUBST_FILE(man_tables)

AC_SUBST_FILE(man_linkhys)

if test x$WITH_FAMILY = x4; then
	AC_DEFINE(QOLYESTER_FAMILY_INET, 4, [INET family])
else
	AC_DEFINE(QOLYESTER_FAMILY_INET, 6, [INET family])
	man_dirtypromischack=$srcdir/man/qolsrd-dirtypromischack.man
fi

if test x$WITH_NUMPREFIX != xno; then
	AC_DEFINE([QOLYESTER_ENABLE_NUMPREFIX], [], [Use numerical prefix in HNA])
fi

if test x$WITH_VIRTUAL != xno; then
	AC_DEFINE([QOLYESTER_ENABLE_VIRTUAL], [], [Use virtual interfaces])
	man_virtual1=$srcdir/man/qolsrd-virtual1.man
	man_virtual2=$srcdir/man/qolsrd-virtual2.man
	AC_SUBST(virtual_version, ["(Virtual version)"])
	AC_SUBST(virtual_description, ["  This is the version with virtual interfaces, to be used with the Qolyester switch."])
else
	man_tables=$srcdir/man/qolsrd-tables.man
fi

if test x$WITH_DEBUG != xno; then
	AC_DEFINE(QOLYESTER_ENABLE_DEBUG, [], [Debugging features])
fi

if test x$WITH_LINKHYS != xno; then
	AC_DEFINE(QOLYESTER_ENABLE_LINKHYS, [], [Link Hysteresis feature])
	man_linkhys=$srcdir/man/qolsrd-linkhys.man
fi

if test x$WITH_MID != xno; then
	AC_DEFINE(QOLYESTER_ENABLE_MID, [], [Use multiple interfaces])
	man_mid1=$srcdir/man/qolsrd-mid1.man
	man_mid2=$srcdir/man/qolsrd-mid2.man
	man_mid3=$srcdir/man/qolsrd-mid3.man
	man_mid4=$srcdir/man/qolsrd-mid4.man
else
	man_mid1=$srcdir/man/qolsrd-nomid1.man
fi

if test x$WITH_HNA != xno; then
	AC_DEFINE(QOLYESTER_ENABLE_HNA, [], [Use external networks])
	man_hna1=$srcdir/man/qolsrd-hna1.man
	man_hna2=$srcdir/man/qolsrd-hna2.man
	man_hna3=$srcdir/man/qolsrd-hna3.man
	man_hna4=$srcdir/man/qolsrd-hna4.man
fi

if test x$WITH_TC_RED != xno; then
	AC_DEFINE(QOLYESTER_ENABLE_TCRED, [], [Use TC redundancy])
	man_tcred=$srcdir/man/qolsrd-tcred.man
fi

if test x$WITH_MPR_RED != xno; then
	AC_DEFINE(QOLYESTER_ENABLE_MPRRED, [], [Use MPR redundancy])
	man_mprred=$srcdir/man/qolsrd-mprred.man
fi


AC_ARG_WITH(doxygen,
	[AC_HELP_STRING([--with-doxygen=PATH],
		        [path to doxygen (default=autodetect)])],
	[if test "x$withval" = xno; then
		DOXYGEN=""
	elif test "x$withval" = xyes; then
		AC_PATH_PROGS([DOXYGEN], [doxygen], [], [$PATH])
		if test "x$DOXYGEN" = x; then
			AC_MSG_WARN([doxygen not found, you won't be able to regenerate the internal documentation.])
		fi
	else
		DOXYGEN="$withval"
	fi],
	[])

AM_CONDITIONAL([COND_DOC], [test "x$DOXYGEN" != x])

# Disable switch building
AM_CONDITIONAL([COND_VIRTUAL], [test x$WITH_VIRTUAL = xyes])

# Checks for programs.
AC_LANG_PUSH([C++])
AC_LANG_COMPILER
AC_PROG_RANLIB
AC_PROG_LEX
if test x"$LEX" != xflex; then
	LEX="$MISSING flex"
	AC_SUBST(LEX_OUTPUT_ROOT, lex.yy)
	AC_SUBST(LEXLIB, '')
fi

AC_PROG_YACC
if test x"$YACC" != x"bison -y"; then
	YACC="$MISSING bison"
fi

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h sys/un.h sys/socket.h sys/time.h linux/types.h syslog.h])

AC_CHECK_HEADERS([linux/netlink.h], [], [AC_MSG_FAILURE([cannot find linux/netlink.h which is needed for the netlink interface])], [#include <sys/socket.h>
#include <asm/types.h>])
AC_CHECK_HEADERS([linux/rtnetlink.h], [], [AC_MSG_FAILURE([cannot find linux/rtnetlink.h which is needed for route management])], [#include <sys/socket.h>
#include <asm/types.h>])

AC_CHECK_HEADERS([linux/unistd.h])
AC_CHECK_HEADERS([linux/sysctl.h], [], [], [#if HAVE_LINUX_UNISTD_H
# include <linux/unistd.h>
#endif])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_STRUCT_TM

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
#AC_HEADER_STDC
AC_TYPE_SIGNAL

dnl  C++ type checking rejects us when using GCC for testing strftime()
dnl  and vfprintf(), most probably because they are builtin functons. As a
dnl  simple workaround, lets test the precense of these fuctions using a
dnl  C compiler.
AC_LANG_PUSH([C])
AC_CHECK_FUNCS([strftime], [], AC_MSG_FAILURE([function strftime not found]))
AC_CHECK_FUNCS([vfprintf], [], AC_MSG_FAILURE([function vfprintf not found]))
AC_LANG_POP

AC_CHECK_FUNCS([gettimeofday], [], AC_MSG_FAILURE([function gettimeofday not found]))
AC_CHECK_FUNCS([inet_ntoa], [], AC_MSG_FAILURE([function inet_ntoa not found]))
AC_CHECK_FUNCS([localtime_r], [], AC_MSG_FAILURE([function localtime_r not found]))
AC_CHECK_FUNCS([memset], [], AC_MSG_FAILURE([function memset not found]))
AC_CHECK_FUNCS([socket], [], AC_MSG_FAILURE([function socket not found]))
AC_CHECK_FUNCS([strerror], [], AC_MSG_FAILURE([function strerror not found]))
AC_CHECK_FUNCS([strtoul], [], AC_MSG_FAILURE([function strtoul not found]))

AC_CHECK_FUNCS([floorf ceilf roundf])

AC_LANG([C++])

AC_DEFUN([AC_LIB_STL_STD],
	 [AC_CACHE_CHECK([for a standard STL implementation], [ac_cv_lib_stl_std],
	       		 [AC_COMPILE_IFELSE([#include <set>
                                             namespace {
	    				       namespace set {}
	      				       template <template <class, class, class> class S>
					       struct foo {
						 typedef S<int, int, int>	set_t;
					       };
					       foo<std::set>	bar;
					     }], [ac_cv_lib_stl_std=yes],
					    [ac_cv_lib_stl_std=no])])
	  if test $ac_cv_lib_stl_std = no; then
	    AC_MSG_ERROR([You are using a compiler version that does not include STL declarations in namespace ::std.  Please use a standard-compliant C++ compiler.])
	  fi])

AC_LIB_STL_STD

AC_LANG_POP

AC_SUBST([CHECK_CXXFLAGS])

AC_CONFIG_FILES([Makefile
	src/Makefile
	man/Makefile
	doc/Makefile
	src/tests/Makefile
	src/tests/sanity/Makefile])

AC_CONFIG_FILES([man/qolsrd.8])

if test "x$DOXYGEN" != x; then
  AC_CONFIG_FILES([doc/Doxyfile-daemon])
  if test x$WITH_VIRTUAL = xyes; then
    AC_CONFIG_FILES([doc/Doxyfile-switch])
  fi
fi

AH_TOP([#ifndef QOLYESTER_CONFIG_HH
# define QOLYESTER_CONFIG_HH 1])

AH_BOTTOM([# ifdef QOLYESTER_ENABLE_DEBUG
#  define DEBUG
# else
#  define NDEBUG
# endif

# if defined __GNUC__ && __GNUC__ >= 3
#  include <ext/hash_set>
#  include <ext/hash_map>
#  if __GNUC__ >= 4 || __GNUC_MINOR__ >= 2
namespace std {
  using ::__gnu_cxx::hash;
  using ::__gnu_cxx::hash_set;
  using ::__gnu_cxx::hash_multiset;
  using ::__gnu_cxx::hash_map;
  using ::__gnu_cxx::hash_multimap;
}
#  endif
# else
#  include <hash_set>
#  include <hash_map>
# endif

# define MAX(a, b) (((a) < (b)) ? (b) : (a))

# if QOLYESTER_FAMILY_INET == 6
#  define QOLYESTER_TWO_SOCKETS
# elif QOLYESTER_FAMILY_INET == 4
#  define QOLYESTER_TWO_SOCKETS
# endif

# if QOLYESTER_FAMILY_INET == 6
#  ifdef QOLYESTER_ALL_ROUTERS
#   define QOLYESTER_LINKLOCAL_MCAST	"ff02::2"
#   define QOLYESTER_SITELOCAL_MCAST	"ff05::2"
#   define QOLYESTER_GLOBAL_MCAST	"ff0e::2"
#  else
#   define QOLYESTER_LINKLOCAL_MCAST	"ff02::1"
#   define QOLYESTER_SITELOCAL_MCAST	"ff05::1"
#   define QOLYESTER_GLOBAL_MCAST	"ff0e::1"
#  endif
# endif

# if !defined HAVE_ROUNDF || !defined HAVE_FLOORF || !defined HAVE_CEILF

#  include <cmath>

#  ifndef HAVE_FLOORF
inline
float
floorf(float f)
{
  return (float) floor(f);
}
#  endif

#  ifndef HAVE_CEILF
inline
float
ceilf(float f)
{
  return (float) ceil(f);
}
#  endif

#  ifndef HAVE_ROUNDF
inline
float
roundf(float f)
{
  if (f >= 0)
    return floorf(f + .5f);
  return ceilf(f - .5f);
}
#  endif

# endif

#endif // !QOLYESTER_CONFIG_HH])

AC_OUTPUT

A=IPv$WITH_FAMILY
# B=$WITH_MID
# C=$WITH_HNA
# D=$WITH_TC_RED
# E=$WITH_MPR_RED
F=$WITH_LINKHYS
G=$WITH_DEBUG
if test x$WITH_VIRTUAL = xyes; then
  H=virtual
else
  H=actual
fi

if test x$WITH_NUMPREFIX = xyes; then
  I=numerical
else
  I=masks
fi

echo ""
echo "Qolyester has been configured with the following features:"
echo "           Address family: $A"
echo "               HNA prefix: $I"
# echo "      Multiple interfaces: $B"
# echo "        External networks: $C"
# echo "            TC Redundancy: $D"
# echo "           MPR Redundancy: $E"
echo "          Link Hysteresis: $F"
echo "       Debugging features: $G"
echo "               Interfaces: $H"
echo ""
