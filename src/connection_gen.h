//
// Created by thomas on 08/07/2021.
//

#ifndef R_EXASOL_CONNECTION_GEN_H
#define R_EXASOL_CONNECTION_GEN_H

#include <Rdefines.h>
#include <if/ExaTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

    extern void initConnection();
    extern void destroyConnection();
    extern SEXP createReadConnection(tSocket socket);
    extern SEXP createWriteConnection(tSocket socket);

#ifdef __cplusplus
}
#endif

#endif //R_EXASOL_CONNECTION_GEN_H
