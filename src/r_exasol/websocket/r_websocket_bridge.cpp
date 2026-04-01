#include <r_exasol/websocket/r_websocket_bridge.h>

// R headers define macros (length, error, etc.) that clash with C++ stdlib.
// Undef them before including any C++ headers.
#undef length
#undef error
#undef WARNING
#undef STRICT_R_HEADERS

#include <r_exasol/websocket/websocket_client.h>
#include <r_exasol/websocket/exasol_auth.h>
#include <r_exasol/websocket/exasol_commands.h>
#include <r_exasol/websocket/exasol_error.h>
#include <r_exasol/external/nlohmann/json.hpp>

#include <memory>
#include <string>

/* ------------------------------------------------------------------ */
/*  WsSession: holds the C++ objects that back one R connection       */
/* ------------------------------------------------------------------ */

struct WsSession {
    std::unique_ptr<exa::WebSocketClient> ws;
    std::unique_ptr<exa::ExasolCommands>  cmds;
    exa::LoginResponse                    loginInfo;
};

/* Invoked by the R garbage collector when the external pointer is freed. */
static void wsSessionFinalizer(SEXP extPtr) {
    WsSession *sess = static_cast<WsSession*>(R_ExternalPtrAddr(extPtr));
    if (sess != nullptr) {
        try {
            if (sess->ws && sess->ws->isConnected()) {
                sess->cmds->disconnect();
            }
        } catch (...) { /* swallow */ }
        try {
            if (sess->ws) {
                sess->ws->close();
            }
        } catch (...) { /* swallow */ }
        delete sess;
        R_ClearExternalPtr(extPtr);
    }
}

/* Extract the WsSession* from an R external pointer, raising an
 * error if the pointer has already been cleared (disconnected). */
static WsSession* unwrapSession(SEXP connPtr) {
    WsSession *sess = static_cast<WsSession*>(R_ExternalPtrAddr(connPtr));
    if (sess == nullptr) {
        Rf_error("WebSocket session has been disconnected or garbage-collected");
    }
    return sess;
}

/* ------------------------------------------------------------------ */
/*  Helper: convert a nlohmann::json column-major data array to an    */
/*  R list of vectors (one vector per column).                        */
/* ------------------------------------------------------------------ */

static SEXP jsonDataToRList(const nlohmann::json& data,
                            const std::vector<std::string>& colTypes,
                            int numColumns, int numRows) {
    SEXP result = PROTECT(Rf_allocVector(VECSXP, numColumns));

    for (int col = 0; col < numColumns; ++col) {
        const nlohmann::json& colData = data[col];
        const std::string& ctype = (col < static_cast<int>(colTypes.size()))
                                       ? colTypes[col] : std::string("VARCHAR");

        /* Decide R vector type based on Exasol column type.
         * Numeric types: DECIMAL, DOUBLE, INTEGER, BIGINT, SMALLINT, TINYINT
         * String types: everything else (VARCHAR, CHAR, DATE, TIMESTAMP, etc.)
         */
        bool isNumeric = (ctype == "DECIMAL" || ctype == "DOUBLE PRECISION" ||
                          ctype == "DOUBLE" || ctype == "INTEGER" ||
                          ctype == "BIGINT" || ctype == "SMALLINT" ||
                          ctype == "TINYINT" || ctype == "BOOLEAN");

        if (isNumeric) {
            SEXP vec = PROTECT(Rf_allocVector(REALSXP, numRows));
            double *ptr = REAL(vec);
            for (int row = 0; row < numRows; ++row) {
                if (row < static_cast<int>(colData.size()) && !colData[row].is_null()) {
                    ptr[row] = colData[row].get<double>();
                } else {
                    ptr[row] = NA_REAL;
                }
            }
            SET_VECTOR_ELT(result, col, vec);
            UNPROTECT(1);
        } else {
            SEXP vec = PROTECT(Rf_allocVector(STRSXP, numRows));
            for (int row = 0; row < numRows; ++row) {
                if (row < static_cast<int>(colData.size()) && !colData[row].is_null()) {
                    std::string val = colData[row].get<std::string>();
                    SET_STRING_ELT(vec, row, Rf_mkChar(val.c_str()));
                } else {
                    SET_STRING_ELT(vec, row, NA_STRING);
                }
            }
            SET_VECTOR_ELT(result, col, vec);
            UNPROTECT(1);
        }
    }
    UNPROTECT(1); /* result */
    return result;
}

/* ------------------------------------------------------------------ */
/*  Bridge functions                                                  */
/* ------------------------------------------------------------------ */

extern "C" {

SEXP exaWsConnect(SEXP host, SEXP port, SEXP useTls,
                  SEXP username, SEXP password, SEXP protocolVersion) {
    try {
        const char *hostStr = CHAR(STRING_ELT(host, 0));
        int portInt         = Rf_asInteger(port);
        int tls             = Rf_asLogical(useTls);
        const char *user    = CHAR(STRING_ELT(username, 0));
        const char *pwd     = CHAR(STRING_ELT(password, 0));
        int protoVer        = Rf_asInteger(protocolVersion);

        WsSession *sess = new WsSession();
        sess->ws = std::unique_ptr<exa::WebSocketClient>(new exa::WebSocketClient());
        sess->ws->connect(hostStr, portInt, tls != 0);

        sess->loginInfo = exa::ExasolAuth::login(*(sess->ws), user, pwd, protoVer);

        sess->cmds = std::unique_ptr<exa::ExasolCommands>(
            new exa::ExasolCommands(*(sess->ws)));

        /* Wrap in R external pointer */
        SEXP extPtr = PROTECT(R_MakeExternalPtr(sess, R_NilValue, R_NilValue));
        R_RegisterCFinalizerEx(extPtr, wsSessionFinalizer, TRUE);

        /* Build return list with handle + metadata */
        const char *names[] = {
            "handle", "sessionId", "dbName", "dbVersion",
            "prodName", "protocolVersion", "timeZone",
            "maxIdentifierLength", ""
        };
        SEXP result = PROTECT(Rf_mkNamed(VECSXP, names));

        SET_VECTOR_ELT(result, 0, extPtr);
        SET_VECTOR_ELT(result, 1, Rf_ScalarReal(static_cast<double>(sess->loginInfo.sessionId)));
        SET_VECTOR_ELT(result, 2, Rf_mkString(sess->loginInfo.databaseName.c_str()));
        SET_VECTOR_ELT(result, 3, Rf_mkString(sess->loginInfo.releaseVersion.c_str()));
        SET_VECTOR_ELT(result, 4, Rf_mkString(sess->loginInfo.productName.c_str()));
        SET_VECTOR_ELT(result, 5, Rf_ScalarInteger(sess->loginInfo.protocolVersion));
        SET_VECTOR_ELT(result, 6, Rf_mkString(sess->loginInfo.timeZone.c_str()));
        SET_VECTOR_ELT(result, 7, Rf_ScalarInteger(sess->loginInfo.maxIdentifierLength));

        UNPROTECT(2);
        return result;

    } catch (const exa::ExasolException& ex) {
        Rf_error("Exasol WebSocket connect failed [%s]: %s",
                 ex.sqlCode().c_str(), ex.what());
    } catch (const std::exception& ex) {
        Rf_error("WebSocket connect failed: %s", ex.what());
    }
    return R_NilValue; /* unreachable */
}


SEXP exaWsExecute(SEXP connPtr, SEXP sql) {
    try {
        WsSession *sess = unwrapSession(connPtr);
        const char *sqlStr = CHAR(STRING_ELT(sql, 0));

        exa::ExecuteResult er = sess->cmds->execute(sqlStr);

        /* Build return list */
        const char *names[] = {
            "numResults", "rowCount", "resultSetHandle",
            "numColumns", "numRows", "numRowsInMessage",
            "columnNames", "columnTypes", "data", ""
        };
        SEXP result = PROTECT(Rf_mkNamed(VECSXP, names));

        SET_VECTOR_ELT(result, 0, Rf_ScalarInteger(er.numResults));
        SET_VECTOR_ELT(result, 1, Rf_ScalarReal(static_cast<double>(er.rowCount)));
        SET_VECTOR_ELT(result, 2, Rf_ScalarInteger(er.resultSetHandle));
        SET_VECTOR_ELT(result, 3, Rf_ScalarInteger(er.numColumns));
        SET_VECTOR_ELT(result, 4, Rf_ScalarReal(static_cast<double>(er.numRows)));
        SET_VECTOR_ELT(result, 5, Rf_ScalarReal(static_cast<double>(er.numRowsInMessage)));

        /* Column names */
        int nCols = static_cast<int>(er.columnNames.size());
        SEXP colNames = PROTECT(Rf_allocVector(STRSXP, nCols));
        for (int i = 0; i < nCols; ++i) {
            SET_STRING_ELT(colNames, i, Rf_mkChar(er.columnNames[i].c_str()));
        }
        SET_VECTOR_ELT(result, 6, colNames);
        UNPROTECT(1);

        /* Column types */
        int nTypes = static_cast<int>(er.columnTypes.size());
        SEXP colTypes = PROTECT(Rf_allocVector(STRSXP, nTypes));
        for (int i = 0; i < nTypes; ++i) {
            SET_STRING_ELT(colTypes, i, Rf_mkChar(er.columnTypes[i].c_str()));
        }
        SET_VECTOR_ELT(result, 7, colTypes);
        UNPROTECT(1);

        /* Data (column-major JSON array of arrays) */
        if (!er.data.is_null() && er.data.is_array() && er.numColumns > 0) {
            int nRows = static_cast<int>(er.numRowsInMessage);
            SET_VECTOR_ELT(result, 8,
                jsonDataToRList(er.data, er.columnTypes, er.numColumns, nRows));
        } else {
            SET_VECTOR_ELT(result, 8, R_NilValue);
        }

        UNPROTECT(1); /* result */
        return result;

    } catch (const exa::ExasolException& ex) {
        Rf_error("Exasol execute failed [%s]: %s",
                 ex.sqlCode().c_str(), ex.what());
    } catch (const std::exception& ex) {
        Rf_error("Execute failed: %s", ex.what());
    }
    return R_NilValue;
}


SEXP exaWsFetch(SEXP connPtr, SEXP resultSetHandle,
                SEXP startPosition, SEXP numBytes) {
    try {
        WsSession *sess = unwrapSession(connPtr);
        int handle   = Rf_asInteger(resultSetHandle);
        int startPos = Rf_asInteger(startPosition);
        int nBytes   = Rf_asInteger(numBytes);

        nlohmann::json fetchResult = sess->cmds->fetch(handle, startPos, nBytes);

        /* Return the JSON as a string for R-level parsing,
         * or convert to list if structure is known. For now return as string. */
        std::string jsonStr = fetchResult.dump();
        return Rf_mkString(jsonStr.c_str());

    } catch (const exa::ExasolException& ex) {
        Rf_error("Exasol fetch failed [%s]: %s",
                 ex.sqlCode().c_str(), ex.what());
    } catch (const std::exception& ex) {
        Rf_error("Fetch failed: %s", ex.what());
    }
    return R_NilValue;
}


SEXP exaWsCloseResultSet(SEXP connPtr, SEXP resultSetHandle) {
    try {
        WsSession *sess = unwrapSession(connPtr);
        int handle = Rf_asInteger(resultSetHandle);
        sess->cmds->closeResultSet(handle);
        return Rf_ScalarLogical(TRUE);

    } catch (const exa::ExasolException& ex) {
        Rf_error("Exasol closeResultSet failed [%s]: %s",
                 ex.sqlCode().c_str(), ex.what());
    } catch (const std::exception& ex) {
        Rf_error("closeResultSet failed: %s", ex.what());
    }
    return R_NilValue;
}


SEXP exaWsDisconnect(SEXP connPtr) {
    WsSession *sess = static_cast<WsSession*>(R_ExternalPtrAddr(connPtr));
    if (sess != nullptr) {
        try {
            if (sess->ws && sess->ws->isConnected()) {
                sess->cmds->disconnect();
            }
        } catch (...) {
            /* best-effort: swallow errors during disconnect command */
        }
        try {
            if (sess->ws) {
                sess->ws->close();
            }
        } catch (...) {
            /* swallow */
        }
        delete sess;
        R_ClearExternalPtr(connPtr);
    }
    return Rf_ScalarLogical(TRUE);
}


SEXP exaWsSetAttributes(SEXP connPtr, SEXP attrJson) {
    try {
        WsSession *sess = unwrapSession(connPtr);
        const char *jsonStr = CHAR(STRING_ELT(attrJson, 0));
        nlohmann::json attrs = nlohmann::json::parse(jsonStr);
        sess->cmds->setAttributes(attrs);
        return Rf_ScalarLogical(TRUE);

    } catch (const exa::ExasolException& ex) {
        Rf_error("Exasol setAttributes failed [%s]: %s",
                 ex.sqlCode().c_str(), ex.what());
    } catch (const std::exception& ex) {
        Rf_error("setAttributes failed: %s", ex.what());
    }
    return R_NilValue;
}


SEXP exaWsGetAttributes(SEXP connPtr) {
    try {
        WsSession *sess = unwrapSession(connPtr);
        nlohmann::json attrs = sess->cmds->getAttributes();
        std::string jsonStr = attrs.dump();
        return Rf_mkString(jsonStr.c_str());

    } catch (const exa::ExasolException& ex) {
        Rf_error("Exasol getAttributes failed [%s]: %s",
                 ex.sqlCode().c_str(), ex.what());
    } catch (const std::exception& ex) {
        Rf_error("getAttributes failed: %s", ex.what());
    }
    return R_NilValue;
}


SEXP exaWsIsConnected(SEXP connPtr) {
    WsSession *sess = static_cast<WsSession*>(R_ExternalPtrAddr(connPtr));
    if (sess == nullptr || !sess->ws) {
        return Rf_ScalarLogical(FALSE);
    }
    return Rf_ScalarLogical(sess->ws->isConnected() ? TRUE : FALSE);
}

void* exaWsGetCommandsPtr(SEXP connPtr) {
    WsSession *sess = static_cast<WsSession*>(R_ExternalPtrAddr(connPtr));
    if (sess == nullptr) {
        return nullptr;
    }
    return static_cast<void*>(sess->cmds.get());
}

} /* extern "C" */
