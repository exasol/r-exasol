#ifndef R_EXASOL_WEBSOCKET_R_WEBSOCKET_BRIDGE_H
#define R_EXASOL_WEBSOCKET_R_WEBSOCKET_BRIDGE_H

#include <R.h>
#include <Rinternals.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Extract the ExasolCommands pointer from a WsSession external pointer.
 * Returns NULL if the pointer has been cleared. Used by the bulk transfer path. */
void* exaWsGetCommandsPtr(SEXP connPtr);

#ifdef __cplusplus
}
#endif

#endif /* R_EXASOL_WEBSOCKET_R_WEBSOCKET_BRIDGE_H */
