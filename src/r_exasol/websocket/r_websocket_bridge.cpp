#include <r_exasol/websocket/r_websocket_bridge.h>

// R headers define macros that clash with C++ stdlib.
#undef length
#undef error
#undef WARNING
#undef STRICT_R_HEADERS

#include <r_exasol/websocket/ws_session.h>

/* ------------------------------------------------------------------ */
/*  Internal helper used by bulk transfer code                        */
/* ------------------------------------------------------------------ */

extern "C" {

void* exaWsGetCommandsPtr(SEXP connPtr) {
    WsSession *sess = static_cast<WsSession*>(R_ExternalPtrAddr(connPtr));
    if (sess == nullptr) {
        return nullptr;
    }
    return static_cast<void*>(sess->cmds.get());
}

} /* extern "C" */
