#include <Rcpp.h>

// R headers define macros (length, error, etc.) that clash with C++ stdlib.
// Undef them before including any C++ headers.
#undef length
#undef error
#undef WARNING
#undef STRICT_R_HEADERS
#undef FALSE
#undef TRUE
#undef Realloc
#undef Free

#include <r_exasol/websocket/ws_session.h>
#include <r_exasol/websocket/exasol_error.h>
#include <r_exasol/external/nlohmann/json.hpp>
#include <r_exasol/websocket/r_websocket_bridge.h>
#include <connection.h>

#include <memory>
#include <string>

/* ------------------------------------------------------------------ */
/*  Helper: unwrap WsSession from external pointer                    */
/* ------------------------------------------------------------------ */

static WsSession* unwrapSession(SEXP connPtr) {
    Rcpp::XPtr<WsSession> xptr(connPtr);
    WsSession *sess = xptr.get();
    if (sess == nullptr) {
        Rcpp::stop("WebSocket session has been disconnected or garbage-collected");
    }
    return sess;
}

/* ------------------------------------------------------------------ */
/*  Helper: convert a nlohmann::json column-major data array to an    */
/*  R list of vectors (one vector per column).                        */
/* ------------------------------------------------------------------ */

static Rcpp::List jsonDataToRList(const nlohmann::json& data,
                                   const std::vector<std::string>& colTypes,
                                   int numColumns, int numRows) {
    Rcpp::List result(numColumns);

    for (int col = 0; col < numColumns; ++col) {
        const nlohmann::json& colData = data[col];
        const std::string& ctype = (col < static_cast<int>(colTypes.size()))
                                       ? colTypes[col] : std::string("VARCHAR");

        bool isNumeric = (ctype == "DECIMAL" || ctype == "DOUBLE PRECISION" ||
                          ctype == "DOUBLE" || ctype == "INTEGER" ||
                          ctype == "BIGINT" || ctype == "SMALLINT" ||
                          ctype == "TINYINT" || ctype == "BOOLEAN");

        if (isNumeric) {
            Rcpp::NumericVector vec(numRows);
            for (int row = 0; row < numRows; ++row) {
                if (row < static_cast<int>(colData.size()) && !colData[row].is_null()) {
                    vec[row] = colData[row].get<double>();
                } else {
                    vec[row] = NA_REAL;
                }
            }
            result[col] = vec;
        } else {
            Rcpp::CharacterVector vec(numRows);
            for (int row = 0; row < numRows; ++row) {
                if (row < static_cast<int>(colData.size()) && !colData[row].is_null()) {
                    vec[row] = colData[row].get<std::string>();
                } else {
                    vec[row] = NA_STRING;
                }
            }
            result[col] = vec;
        }
    }
    return result;
}

/* ================================================================== */
/*  WebSocket bridge functions                                        */
/* ================================================================== */

// [[Rcpp::export]]
Rcpp::List exaWsConnect(const std::string& host, int port, bool useTls,
                        const std::string& username, const std::string& password,
                        int protocolVersion) {
    try {
        auto *sess = new WsSession();
        sess->ws = std::make_unique<exa::WebSocketClient>();
        sess->ws->connect(host, port, useTls);

        sess->loginInfo = exa::ExasolAuth::login(*(sess->ws), username, password, protocolVersion);

        sess->cmds = std::make_unique<exa::ExasolCommands>(*(sess->ws));

        Rcpp::XPtr<WsSession> xptr(sess, true);

        return Rcpp::List::create(
            Rcpp::Named("handle") = xptr,
            Rcpp::Named("sessionId") = static_cast<double>(sess->loginInfo.sessionId),
            Rcpp::Named("dbName") = sess->loginInfo.databaseName,
            Rcpp::Named("dbVersion") = sess->loginInfo.releaseVersion,
            Rcpp::Named("prodName") = sess->loginInfo.productName,
            Rcpp::Named("protocolVersion") = sess->loginInfo.protocolVersion,
            Rcpp::Named("timeZone") = sess->loginInfo.timeZone,
            Rcpp::Named("maxIdentifierLength") = sess->loginInfo.maxIdentifierLength
        );

    } catch (const exa::ExasolException& ex) {
        Rcpp::stop("Exasol WebSocket connect failed [%s]: %s",
                   ex.sqlCode().c_str(), ex.what());
    } catch (const std::exception& ex) {
        Rcpp::stop("WebSocket connect failed: %s", ex.what());
    }
    return {}; /* unreachable */
}


// [[Rcpp::export]]
Rcpp::List exaWsExecute(SEXP connPtr, const std::string& sql) {
    try {
        WsSession *sess = unwrapSession(connPtr);

        exa::ExecuteResult execResult = sess->cmds->execute(sql);

        Rcpp::CharacterVector colNames(static_cast<R_xlen_t>(execResult.columnNames.size()));
        for (R_xlen_t i = 0; i < static_cast<R_xlen_t>(execResult.columnNames.size()); ++i) {
            colNames[i] = execResult.columnNames[static_cast<size_t>(i)];
        }

        Rcpp::CharacterVector colTypes(static_cast<R_xlen_t>(execResult.columnTypes.size()));
        for (R_xlen_t i = 0; i < static_cast<R_xlen_t>(execResult.columnTypes.size()); ++i) {
            colTypes[i] = execResult.columnTypes[static_cast<size_t>(i)];
        }

        SEXP dataVal;
        if (!execResult.data.is_null() && execResult.data.is_array() && execResult.numColumns > 0) {
            int nRows = static_cast<int>(execResult.numRowsInMessage);
            dataVal = Rcpp::wrap(jsonDataToRList(execResult.data, execResult.columnTypes, execResult.numColumns, nRows));
        } else {
            dataVal = R_NilValue;
        }

        return Rcpp::List::create(
            Rcpp::Named("numResults") = execResult.numResults,
            Rcpp::Named("rowCount") = static_cast<double>(execResult.rowCount),
            Rcpp::Named("resultSetHandle") = execResult.resultSetHandle,
            Rcpp::Named("numColumns") = execResult.numColumns,
            Rcpp::Named("numRows") = static_cast<double>(execResult.numRows),
            Rcpp::Named("numRowsInMessage") = static_cast<double>(execResult.numRowsInMessage),
            Rcpp::Named("columnNames") = colNames,
            Rcpp::Named("columnTypes") = colTypes,
            Rcpp::Named("data") = dataVal
        );

    } catch (const exa::ExasolException& ex) {
        Rcpp::stop("Exasol execute failed [%s]: %s",
                   ex.sqlCode().c_str(), ex.what());
    } catch (const std::exception& ex) {
        Rcpp::stop("Execute failed: %s", ex.what());
    }
    return {}; /* unreachable */
}


// [[Rcpp::export]]
std::string exaWsFetch(SEXP connPtr, int resultSetHandle,
                       int startPosition, int numBytes) {
    try {
        WsSession *sess = unwrapSession(connPtr);
        nlohmann::json fetchResult = sess->cmds->fetch(resultSetHandle, startPosition, numBytes);
        return fetchResult.dump();

    } catch (const exa::ExasolException& ex) {
        Rcpp::stop("Exasol fetch failed [%s]: %s",
                   ex.sqlCode().c_str(), ex.what());
    } catch (const std::exception& ex) {
        Rcpp::stop("Fetch failed: %s", ex.what());
    }
    return ""; /* unreachable */
}


// [[Rcpp::export]]
bool exaWsCloseResultSet(SEXP connPtr, int resultSetHandle) {
    try {
        WsSession *sess = unwrapSession(connPtr);
        sess->cmds->closeResultSet(resultSetHandle);
        return true;

    } catch (const exa::ExasolException& ex) {
        Rcpp::stop("Exasol closeResultSet failed [%s]: %s",
                   ex.sqlCode().c_str(), ex.what());
    } catch (const std::exception& ex) {
        Rcpp::stop("closeResultSet failed: %s", ex.what());
    }
    return false; /* unreachable */
}


// [[Rcpp::export]]
bool exaWsDisconnect(SEXP connPtr) {
    auto *sess = static_cast<WsSession*>(R_ExternalPtrAddr(connPtr));
    if (sess != nullptr) {
        try {
            if (sess->ws && sess->ws->isConnected()) {
                sess->cmds->disconnect();
            }
        } catch (...) { /* swallow */ } // NOLINT(bugprone-empty-catch)
        try {
            if (sess->ws) {
                sess->ws->close();
            }
        } catch (...) { /* swallow */ } // NOLINT(bugprone-empty-catch)
        delete sess;
        R_ClearExternalPtr(connPtr);
    }
    return true;
}


// [[Rcpp::export]]
bool exaWsSetAttributes(SEXP connPtr, const std::string& attrJson) {
    try {
        WsSession *sess = unwrapSession(connPtr);
        nlohmann::json attrs = nlohmann::json::parse(attrJson);
        sess->cmds->setAttributes(attrs);
        return true;

    } catch (const exa::ExasolException& ex) {
        Rcpp::stop("Exasol setAttributes failed [%s]: %s",
                   ex.sqlCode().c_str(), ex.what());
    } catch (const std::exception& ex) {
        Rcpp::stop("setAttributes failed: %s", ex.what());
    }
    return false; /* unreachable */
}


// [[Rcpp::export]]
std::string exaWsGetAttributes(SEXP connPtr) {
    try {
        WsSession *sess = unwrapSession(connPtr);
        nlohmann::json attrs = sess->cmds->getAttributes();
        return attrs.dump();

    } catch (const exa::ExasolException& ex) {
        Rcpp::stop("Exasol getAttributes failed [%s]: %s",
                   ex.sqlCode().c_str(), ex.what());
    } catch (const std::exception& ex) {
        Rcpp::stop("getAttributes failed: %s", ex.what());
    }
    return ""; /* unreachable */
}


// [[Rcpp::export]]
bool exaWsIsConnected(SEXP connPtr) {
    auto *sess = static_cast<WsSession*>(R_ExternalPtrAddr(connPtr));
    if (sess == nullptr || !sess->ws) {
        return false;
    }
    return sess->ws->isConnected();
}


/* ================================================================== */
/*  Bulk transfer functions                                           */
/* ================================================================== */

// [[Rcpp::export]]
int asyncRODBCIOStart(const std::string& host, int port, const std::string& protocol) {
    return initConnection(host.c_str(), port, protocol.c_str());
}

// [[Rcpp::export]]
SEXP asyncRODBCProxyHost() {
    return copyHostName();
}

// [[Rcpp::export]]
SEXP asyncRODBCProxyPort() {
    return copyHostPort();
}

// [[Rcpp::export]]
SEXP asyncRODBCQueryStart(SEXP chan, const std::string& query, const std::string& protocol, int writer) {
    void *cmdsPtr = exaWsGetCommandsPtr(chan);
    if (cmdsPtr != nullptr && !query.empty()) {
        if (writer != 0) {
            return createWriteConnectionWs(cmdsPtr, query.c_str(), protocol.c_str());
        }
        return createReadConnectionWs(cmdsPtr, query.c_str(), protocol.c_str());
    }
    Rcpp::stop("Could not get WebSocket session from channel");
    return R_NilValue; /* unreachable */
}

// [[Rcpp::export]]
int asyncRODBCQueryFinish(int checkWasDone) {
    return destroyConnection(checkWasDone);
}

// [[Rcpp::export]]
int asyncEnableTracing(const std::string& tracefile) {
    return enableTracing(tracefile.c_str());
}
