#ifndef R_EXASOL_WEBSOCKET_WEBSOCKET_CLIENT_H
#define R_EXASOL_WEBSOCKET_WEBSOCKET_CLIENT_H

#include <string>
#include <mutex>
#include <condition_variable>
#include <memory>

namespace ix {
    class WebSocket;
}

namespace exa {

    /// Synchronous WebSocket client wrapping ixwebsocket's async API.
    class WebSocketClient {
    public:
        WebSocketClient();
        ~WebSocketClient();

        /// Connect to an Exasol WebSocket endpoint.
        void connect(const std::string& host, int port, bool useTls,
                     int timeoutSecs = 10);

        /// Send a JSON message and wait for the response.
        std::string sendAndReceive(const std::string& jsonMessage);

        /// Send a JSON message without waiting for a response.
        void sendOnly(const std::string& jsonMessage);

        /// Close the WebSocket connection.
        void close();

        /// Check whether the connection is open.
        bool isConnected() const;

    private:
        std::unique_ptr<ix::WebSocket> mWebSocket;

        std::mutex mResponseMutex;
        std::condition_variable mResponseCv;
        bool mResponseReady;
        std::string mResponse;
        std::string mConnectionError;

        std::condition_variable mConnectCv;
        bool mConnectDone;
    };

} // namespace exa

#endif // R_EXASOL_WEBSOCKET_WEBSOCKET_CLIENT_H
