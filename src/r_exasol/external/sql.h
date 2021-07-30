#ifndef R_EXASOL_SQL_H
#define R_EXASOL_SQL_H


#ifdef _WIN32
//Need to include windows.h for varios typedefs
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


#ifdef ERROR
#undef ERROR
#endif

#include <sql.h>
#include <sqlext.h>


#endif //R_EXASOL_SQL_H
