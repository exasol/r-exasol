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

##Setup OpenSSL

### check RSA/crypto
AC_CHECK_HEADER([openssl/rsa.h],
    [AC_SEARCH_LIBS(RSA_generate_key_ex, [crypto ssl openssl],,
        AC_MSG_ERROR([Cannot find usable crypto library. Please install openssl-dev or equivalent and/or set PKG_LIBS if not in default location.]))
    ],
[AC_MSG_ERROR([Failed to find usable rsa headers. Please install openssl-dev or equivalent and/or set PKG_LIBS/PKG_CPPFLAGS if not in default location.])]
)

# ##check SSL support
AC_CHECK_HEADER([openssl/ssl.h],
[AC_SEARCH_LIBS(SSL_CTX_load_verify_locations, [ssl openssl],,
  [AC_MSG_ERROR([Cannot find usable SSL library])])],
[AC_MSG_ERROR([Failed to find usable SSL library. Please install openssl-dev or equivalent and/or set PKG_LIBS if not in default location.])]
)


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

# honor PKG_xx overrides
LIBS="-lodbc ${LIBS} ${PKG_LIBS}"

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

AC_SUBST(LIBS)
AC_SUBST(CPPFLAGS)

dnl and do substitution in the src/Makevars.in
AC_CONFIG_FILES([src/Makevars])
AC_OUTPUT
