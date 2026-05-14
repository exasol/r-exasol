#ifndef R_EXASOL_WEBSOCKET_WS_SESSION_H
#define R_EXASOL_WEBSOCKET_WS_SESSION_H

#include <r_exasol/websocket/websocket_client.h>
#include <r_exasol/websocket/exasol_commands.h>
#include <r_exasol/websocket/exasol_auth.h>
#include <memory>

/* Holds the C++ objects that back one R connection */
struct WsSession {
    std::unique_ptr<exa::WebSocketClient> ws;
    std::unique_ptr<exa::ExasolCommands>  cmds;
    exa::LoginResponse                    loginInfo;

    ~WsSession() {
        try {
            if (ws && ws->isConnected() && cmds) {
                cmds->disconnect();
            }
        } catch (...) { /* swallow */ }
        try {
            if (ws) {
                ws->close();
            }
        } catch (...) { /* swallow */ }
    }
};

#endif /* R_EXASOL_WEBSOCKET_WS_SESSION_H */
