#ifndef R_EXASOL_R_CONN_H
#define R_EXASOL_R_CONN_H

#define class clss //need to hide class as it is reserved in C++, but used as variable name in Connections.h
#define private priv //need to hide private as it is reserved in C++, but used as variable name in Connections.h
#include <R_ext/Connections.h>
#undef class
#undef private

#endif //R_EXASOL_R_CONN_H
