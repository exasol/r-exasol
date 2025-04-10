AC_PREREQ([2.69])


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
            AS_HELP_STRING([--with-unit-tests],[wether unit tests should be included]),
            [with_unit_tests=$withval])
UNIT_TESTFLAGS_CPPFLAGS=""
if test [ -n "$with_unit_tests" ] ; then
   UNIT_TESTFLAGS_CPPFLAGS="-DWITH_UNIT_TESTS"
fi

##Setup OpenSSL
m4_include([./tools/ax_check_openssl.m4])
#AX_CHECK_OPENSSL(, [AC_MSG_ERROR([Cannot find OpenSSL])])
AX_CHECK_OPENSSL(, [

if test "`uname -s`" = Darwin; then
  dnl On MacOsX
  PKG_BREW_NAME="openssl@1.1"
  brew --version 2>/dev/null
  if [[ $? -eq 0 ]]; then
    BREWDIR="`brew --prefix`"
    with_openssl="$BREWDIR/opt/openssl"
    AX_CHECK_OPENSSL(, AC_MSG_ERROR([Cannot find OpenSSL. Please install with "brew install $PKG_BREW_NAME"]))
  else
    AC_MSG_ERROR([Cannot find Homebrew])
  fi  
else 
  AC_MSG_ERROR([Cannot find OpenSSL])
fi
])

echo "OpenSSL: ${OPENSSL_INCLUDES}"

## Setup ODBC
AC_CHECK_PROG(ODBC,odbc_config,yes)
if test x"${ODBC}" == x"yes" ; then
    ODBC_CFLAGS=`odbc_config --cflags`
    ODBC_LIBS=`odbc_config --libs`
else
    ODBC_CFLAGS=""
    ODBC_LIBS="-lodbc"
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
CPPFLAGS="${CPPFLAGS} -I. ${UNIT_TESTFLAGS_CPPFLAGS} ${OPENSSL_INCLUDES} ${ODBC_CFLAGS}"
echo "CPPFLAGS:${CPPFLAGS}"

# honor PKG_xx overrides
LIBS="${ODBC_LIBS} ${LIBS} ${PKG_LIBS} ${OPENSSL_LDFLAGS} ${OPENSSL_LIBS}"

## We are using C++
AC_LANG(C++)
AC_REQUIRE_CPP

## Check the C++ compiler using the CXX value set
AC_PROG_CXX

# substitute externalized source list
PKG_SOURCES=$(cat src/sources.list)
AC_SUBST(PKG_SOURCES)

PKG_SOURCE_OBJ_FILES=$(sed  's/.cpp/.o/g' src/sources.list)
PKG_SOURCE_OBJ_FILES+=" exasol.o"
PKG_TEST_OBJ_FILES=$(sed 's/.cpp/.o/g' src/tests/sources.list)
PKG_OBJ_FILES="$PKG_SOURCE_OBJ_FILES $PKG_TEST_OBJ_FILES"
AC_SUBST(PKG_OBJ_FILES)

# substitute unit test source list
PKG_TEST_SOURCES=$(cat src/tests/sources.list)
AC_SUBST(PKG_TEST_SOURCES)

AC_SUBST(LIBS)
AC_SUBST(CPPFLAGS)

dnl and do substitution in the src/Makevars.in
AC_CONFIG_FILES([src/Makevars])
AC_OUTPUT
