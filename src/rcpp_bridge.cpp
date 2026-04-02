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
#include <boost/json.hpp>
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
/*  Helper: convert a boost::json::value column-major data array to   */
/*  an R list of vectors (one vector per column).                     */
/* ------------------------------------------------------------------ */

static Rcpp::List jsonDataToRList(const boost::json::value& data,
                                   const std::vector<std::string>& colTypes,
                                   int numColumns, int numRows) {
    Rcpp::List result(numColumns);
    const boost::json::array& dataArr = data.as_array();

    for (int col = 0; col < numColumns; ++col) {
        const boost::json::array& colData = dataArr[col].as_array();
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
                    vec[row] = colData[row].to_number<double>();
                } else {
                    vec[row] = NA_REAL;
                }
            }
            result[col] = vec;
        } else {
            Rcpp::CharacterVector vec(numRows);
            for (int row = 0; row < numRows; ++row) {
                if (row < static_cast<int>(colData.size()) && !colData[row].is_null()) {
                    vec[row] = std::string(colData[row].as_string());
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
Rcpp::List exaWsConnect(std::string host, int port, bool useTls,
                        std::string username, std::string password,
                        int protocolVersion) {
    try {
        WsSession *sess = new WsSession();
        sess->ws = std::unique_ptr<exa::WebSocketClient>(new exa::WebSocketClient());
        sess->ws->connect(host, port, useTls);

        sess->loginInfo = exa::ExasolAuth::login(*(sess->ws), username, password, protocolVersion);

        sess->cmds = std::unique_ptr<exa::ExasolCommands>(
            new exa::ExasolCommands(*(sess->ws)));

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
    return Rcpp::List(); /* unreachable */
}


// [[Rcpp::export]]
Rcpp::List exaWsExecute(SEXP connPtr, std::string sql) {
    try {
        WsSession *sess = unwrapSession(connPtr);

        exa::ExecuteResult er = sess->cmds->execute(sql);

        Rcpp::CharacterVector colNames(er.columnNames.size());
        for (size_t i = 0; i < er.columnNames.size(); ++i) {
            colNames[i] = er.columnNames[i];
        }

        Rcpp::CharacterVector colTypes(er.columnTypes.size());
        for (size_t i = 0; i < er.columnTypes.size(); ++i) {
            colTypes[i] = er.columnTypes[i];
        }

        SEXP dataVal;
        if (!er.data.is_null() && er.data.is_array() && er.numColumns > 0) {
            int nRows = static_cast<int>(er.numRowsInMessage);
            dataVal = Rcpp::wrap(jsonDataToRList(er.data, er.columnTypes, er.numColumns, nRows));
        } else {
            dataVal = R_NilValue;
        }

        return Rcpp::List::create(
            Rcpp::Named("numResults") = er.numResults,
            Rcpp::Named("rowCount") = static_cast<double>(er.rowCount),
            Rcpp::Named("resultSetHandle") = er.resultSetHandle,
            Rcpp::Named("numColumns") = er.numColumns,
            Rcpp::Named("numRows") = static_cast<double>(er.numRows),
            Rcpp::Named("numRowsInMessage") = static_cast<double>(er.numRowsInMessage),
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
    return Rcpp::List(); /* unreachable */
}


// [[Rcpp::export]]
std::string exaWsFetch(SEXP connPtr, int resultSetHandle,
                       int startPosition, int numBytes) {
    try {
        WsSession *sess = unwrapSession(connPtr);
        boost::json::value fetchResult = sess->cmds->fetch(resultSetHandle, startPosition, numBytes);
        return boost::json::serialize(fetchResult);

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
    WsSession *sess = static_cast<WsSession*>(R_ExternalPtrAddr(connPtr));
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
        R_ClearExternalPtr(connPtr);
    }
    return true;
}


// [[Rcpp::export]]
bool exaWsSetAttributes(SEXP connPtr, std::string attrJson) {
    try {
        WsSession *sess = unwrapSession(connPtr);
        boost::json::value attrs = boost::json::parse(attrJson);
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
        boost::json::value attrs = sess->cmds->getAttributes();
        return boost::json::serialize(attrs);

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
    WsSession *sess = static_cast<WsSession*>(R_ExternalPtrAddr(connPtr));
    if (sess == nullptr || !sess->ws) {
        return false;
    }
    return sess->ws->isConnected();
}


/* ================================================================== */
/*  Bulk transfer functions                                           */
/* ================================================================== */

// [[Rcpp::export]]
int asyncRODBCIOStart(std::string host, int port, std::string protocol) {
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
SEXP asyncRODBCQueryStart(SEXP chan, std::string query, std::string protocol, int writer) {
    void *cmdsPtr = exaWsGetCommandsPtr(chan);
    if (cmdsPtr != NULL && query.size() > 0) {
        if (writer) {
            return createWriteConnectionWs(cmdsPtr, query.c_str(), protocol.c_str());
        } else {
            return createReadConnectionWs(cmdsPtr, query.c_str(), protocol.c_str());
        }
    } else {
        Rcpp::stop("Could not get WebSocket session from channel");
    }
    return R_NilValue; /* unreachable */
}

// [[Rcpp::export]]
int asyncRODBCQueryFinish(int checkWasDone) {
    return destroyConnection(checkWasDone);
}

// [[Rcpp::export]]
int asyncEnableTracing(std::string tracefile) {
    return enableTracing(tracefile.c_str());
}
