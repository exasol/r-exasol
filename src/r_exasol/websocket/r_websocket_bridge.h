#ifndef R_EXASOL_WEBSOCKET_R_WEBSOCKET_BRIDGE_H
#define R_EXASOL_WEBSOCKET_R_WEBSOCKET_BRIDGE_H

#include <R.h>
#include <Rinternals.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Connect to Exasol via WebSocket and authenticate.
 * Returns an R list with:
 *   $handle      - external pointer wrapping WsSession
 *   $sessionId   - integer session ID
 *   $dbName      - database name
 *   $dbVersion   - release version
 *   $prodName    - product name
 *   $protocolVersion - negotiated protocol version
 *   $timeZone    - session time zone
 *   $maxIdentifierLength - max identifier length
 */
SEXP exaWsConnect(SEXP host, SEXP port, SEXP useTls,
                  SEXP username, SEXP password, SEXP protocolVersion);

/* Execute SQL via WebSocket.
 * Returns an R list with: numResults, rowCount, resultSetHandle,
 * numColumns, numRows, columnNames, columnTypes, data
 */
SEXP exaWsExecute(SEXP connPtr, SEXP sql);

/* Fetch rows from a result set.
 * Returns an R list with the data.
 */
SEXP exaWsFetch(SEXP connPtr, SEXP resultSetHandle,
                SEXP startPosition, SEXP numBytes);

/* Close a result set. */
SEXP exaWsCloseResultSet(SEXP connPtr, SEXP resultSetHandle);

/* Disconnect and clean up. */
SEXP exaWsDisconnect(SEXP connPtr);

/* Set session attributes (JSON string). */
SEXP exaWsSetAttributes(SEXP connPtr, SEXP attrJson);

/* Get session attributes (returns JSON string). */
SEXP exaWsGetAttributes(SEXP connPtr);

/* Check if WebSocket is connected. */
SEXP exaWsIsConnected(SEXP connPtr);

/* Extract the ExasolCommands pointer from a WsSession external pointer.
 * Returns NULL if the pointer has been cleared. Used by the bulk transfer path. */
void* exaWsGetCommandsPtr(SEXP connPtr);

#ifdef __cplusplus
}
#endif

#endif /* R_EXASOL_WEBSOCKET_R_WEBSOCKET_BRIDGE_H */
