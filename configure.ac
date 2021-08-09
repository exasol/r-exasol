AC_PREREQ(2.61)


AC_INIT(exasol, m4_esyscmd_s([awk '/^Version:/ {print $2}' DESCRIPTION]))
AC_COPYRIGHT(Copyright (C) 2021 Exasol)


## Determine Install Location of R
: ${R_HOME=$(R RHOME)}
if test -z "${R_HOME}"; then
    AC_MSG_ERROR([Could not determine R_HOME.])
fi


dnl Select if unit tests are enabled or not, from a configure option
dnl or from an environment variable.
AC_ARG_WITH([unit-tests],
            AC_HELP_STRING([--with-unit-tests],
                           [wether unit tests should be included]),
            [with_unit_tests=$withval])
UNIT_TESTFLAGS_CPPFLAGS=""
if test [ -n "$with_unit_tests" ] ; then
   UNIT_TESTFLAGS_CPPFLAGS="-DWITH_UNIT_TESTS"
fi

## Setup RBin
RBIN="${R_HOME}/bin/R"

CXX14=`"${RBIN}" CMD config CXX14`
if test -z "$CXX14"; then
  AC_MSG_ERROR([No C++14 compiler is available])
fi
CXX14STD=`"${RBIN}" CMD config CXX14STD`
CPPFLAGS=`"${RBIN}" CMD config CPPFLAGS`
CXXFLAGS=`"${RBIN}" CMD config CXX14FLAGS`
CXX="${CXX14} ${CXX14STD}"
echo "CPPFLAGS:${CPPFLAGS}"
CPPFLAGS="${CPPFLAGS} -I. ${UNIT_TESTFLAGS_CPPFLAGS}"
echo "CPPFLAGS:${CPPFLAGS}"

## We are using C++
AC_LANG(C++)
AC_REQUIRE_CPP

## Check the C++ compiler using the CXX value set
AC_PROG_CXX

# substitute externalized source list
PKG_SOURCES=$(cat src/sources.list)
AC_SUBST(PKG_SOURCES)

# substitute unit test source list
PKG_TEST_SOURCES=$(cat src/tests/sources.list)
AC_SUBST(PKG_TEST_SOURCES)


dnl substitute UNIT_TESTFLAGS_CPPFLAGS
AC_SUBST(UNIT_TESTFLAGS_CPPFLAGS)
dnl and do substitution in the src/Makevars.in and src/config.h
AC_CONFIG_FILES([src/Makevars])
AC_OUTPUT
