#ifndef R_EXASOL_R_H
#define R_EXASOL_R_H

#include <R.h>
#include <Rdefines.h>
#undef length //Need to undefine length as it is used in std functions, but defined as macro in Rinternals.h

#endif //R_EXASOL_R_H
