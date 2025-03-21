dnl Process this file with autoconf

AC_INIT([rgadtrackerd], m4_esyscmd([echo -n $(git describe --all --tags --long | sed "s/v\([0-9]\.[0-9]-.*\)-.*/\1/")]), [kyzmenko.pavel@gmail.com])

AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([subdir-objects])

AC_PROG_CXX
AC_LANG_CPLUSPLUS
AC_LANG_PUSH([C++])
AC_PROG_MAKE_SET
AC_DISABLE_STATIC


dnl Check for libtool
LT_PREREQ([2.2.6])
LT_INIT


AC_ARG_ENABLE(debug,
AS_HELP_STRING([--enable-debug],
	[enable debugging, default: no]),
		[case "${enableval}" in
		yes) debug=true ;;
		no)  debug=false ;;
		*)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
		esac],
		[debug=false])
AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")


AC_CHECK_LIB([icuuc],[main],[],[AC_MSG_WARN([icuuc library missing])])
AC_CHECK_LIB([icui18n],[main],[],[AC_MSG_ERROR([icui18n library missing])])
AC_CHECK_LIB([icudata],[main],[],[AC_MSG_ERROR([icudata library missing])])
AC_CHECK_LIB([fcgi], [FCGX_FPrintF], [], [AC_MSG_ERROR([fcgi was not found])])
AC_CHECK_LIB([GeoIP], [GeoIP_region_name_by_code], [], [AC_MSG_ERROR([GeoIP was not found])])
AC_CHECK_LIB([tinyxml], [main], [], [AC_MSG_ERROR([tinyxml was not found])])


BOOST_REQUIRE([1.58],AC_MSG_ERROR([boost>=1.58 required]))
BOOST_FILESYSTEM()
BOOST_THREADS()
BOOST_DATE_TIME()
BOOST_REGEX()


AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([
 Makefile
])

AC_OUTPUT
