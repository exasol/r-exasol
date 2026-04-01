#include <r_exasol/websocket/websocket_client.h>
#include <r_exasol/websocket/exasol_error.h>
#include <r_exasol/external/ixwebsocket/IXWebSocket.h>
#include <r_exasol/external/ixwebsocket/IXNetSystem.h>
#include <sstream>
#include <chrono>

namespace exa {

    WebSocketClient::WebSocketClient()
        : mWebSocket(std::make_unique<ix::WebSocket>())
        , mResponseReady(false)
        , mConnectDone(false)
    {
        ix::initNetSystem();
    }

    WebSocketClient::~WebSocketClient() {
        close();
        ix::uninitNetSystem();
    }

    void WebSocketClient::connect(const std::string& host, int port, bool useTls,
                                   int timeoutSecs) {
        std::string scheme = useTls ? "wss" : "ws";
        std::ostringstream url;
        url << scheme << "://" << host << ":" << port;
        mWebSocket->setUrl(url.str());

        if (useTls) {
            ix::SocketTLSOptions tlsOptions;
            tlsOptions.tls = true;
            // Exasol docker/self-signed certs: disable peer verification
            tlsOptions.caFile = "NONE";
            mWebSocket->setTLSOptions(tlsOptions);
        }

        mWebSocket->disableAutomaticReconnection();
        mWebSocket->setHandshakeTimeout(timeoutSecs);

        mWebSocket->setOnMessageCallback(
            [this](const ix::WebSocketMessagePtr& msg) {
                if (msg->type == ix::WebSocketMessageType::Message) {
                    std::lock_guard<std::mutex> lock(mResponseMutex);
                    mResponse = msg->str;
                    mResponseReady = true;
                    mResponseCv.notify_one();
                } else if (msg->type == ix::WebSocketMessageType::Open) {
                    std::lock_guard<std::mutex> lock(mResponseMutex);
                    mConnectDone = true;
                    mConnectCv.notify_one();
                } else if (msg->type == ix::WebSocketMessageType::Error) {
                    std::lock_guard<std::mutex> lock(mResponseMutex);
                    std::string reason = msg->errorInfo.reason;
                    if (msg->errorInfo.http_status != 0) {
                        reason += " (HTTP " + std::to_string(msg->errorInfo.http_status) + ")";
                    }
                    if (reason.empty()) {
                        reason = "unknown error";
                    }
                    mConnectionError = reason;
                    mResponseReady = true;
                    mResponseCv.notify_one();
                    mConnectDone = true;
                    mConnectCv.notify_one();
                } else if (msg->type == ix::WebSocketMessageType::Close) {
                    std::lock_guard<std::mutex> lock(mResponseMutex);
                    mConnectionError = "Connection closed by server: " +
                                       std::to_string(msg->closeInfo.code) +
                                       " " + msg->closeInfo.reason;
                    mResponseReady = true;
                    mResponseCv.notify_one();
                    mConnectDone = true;
                    mConnectCv.notify_one();
                }
            }
        );

        mWebSocket->start();

        // Wait for Open or Error/Close callback, with timeout.
        {
            std::unique_lock<std::mutex> lock(mResponseMutex);
            bool gotSignal = mConnectCv.wait_for(
                lock,
                std::chrono::seconds(timeoutSecs),
                [this]() { return mConnectDone; }
            );

            if (!gotSignal) {
                mWebSocket->stop();
                throw ExasolException(
                    "WebSocket connection timed out to " + url.str(),
                    "08001"
                );
            }

            if (!mConnectionError.empty()) {
                std::string error = mConnectionError;
                mWebSocket->stop();
                throw ExasolException(
                    "WebSocket connection failed: " + error,
                    "08001"
                );
            }
        }
    }

    std::string WebSocketClient::sendAndReceive(const std::string& jsonMessage) {
        if (!isConnected()) {
            throw ExasolException("WebSocket is not connected", "08003");
        }

        {
            std::lock_guard<std::mutex> lock(mResponseMutex);
            mResponseReady = false;
            mResponse.clear();
            mConnectionError.clear();
        }

        auto sendInfo = mWebSocket->sendText(jsonMessage);
        if (!sendInfo.success) {
            throw ExasolException("Failed to send WebSocket message", "08S01");
        }

        std::unique_lock<std::mutex> lock(mResponseMutex);
        if (!mResponseCv.wait_for(lock, std::chrono::seconds(300),
                                   [this]() { return mResponseReady; })) {
            throw ExasolException("WebSocket response timed out after 300 seconds", "08S01");
        }

        if (!mConnectionError.empty()) {
            throw ExasolException(
                "WebSocket error during receive: " + mConnectionError,
                "08S01"
            );
        }

        return mResponse;
    }

    void WebSocketClient::sendOnly(const std::string& jsonMessage) {
        if (!isConnected()) {
            return;
        }
        mWebSocket->sendText(jsonMessage);
    }

    void WebSocketClient::close() {
        if (mWebSocket) {
            mWebSocket->stop();
        }
    }

    bool WebSocketClient::isConnected() const {
        return mWebSocket &&
               mWebSocket->getReadyState() == ix::ReadyState::Open;
    }

} // namespace exa
