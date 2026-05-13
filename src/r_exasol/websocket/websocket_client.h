#ifndef R_EXASOL_WEBSOCKET_WEBSOCKET_CLIENT_H
#define R_EXASOL_WEBSOCKET_WEBSOCKET_CLIENT_H

#include <string>
#include <variant>

#include <r_exasol/websocket/exasol_exception.h>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>

namespace exa {
namespace ws_detail {
    namespace net = boost::asio;
    namespace beast = boost::beast;
    namespace websocket = beast::websocket;
    namespace ssl = net::ssl;
    using tcp = net::ip::tcp;
} // namespace ws_detail

    using PlainStream = ws_detail::websocket::stream<ws_detail::beast::tcp_stream>;
    using SslStream = ws_detail::websocket::stream<ws_detail::beast::ssl_stream<ws_detail::beast::tcp_stream>>;

    /// Synchronous WebSocket client using Boost.Beast.
    class WebSocketClient {
    public:
        WebSocketClient();
        ~WebSocketClient();

        /// Connect to an Exasol WebSocket endpoint.
        ///
        /// `sslCertificate` controls TLS verification:
        ///   * empty string or "SSL_VERIFY_SERVER" — verify the server certificate against
        ///     the system trust store (default).
        ///   * "SSL_VERIFY_NONE" — disable certificate verification.
        ///   * any other value — treated as a filesystem path to a PEM trust file.
        void connect(const std::string& host, int port, bool useTls,
                     const std::string& sslCertificate = "",
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
        using StreamVariant = std::variant<std::monostate, PlainStream, SslStream>;

        template<typename F>
        auto visitStream(F&& f) -> decltype(f(std::declval<PlainStream&>())) {
            return std::visit([&](auto& s) -> decltype(f(std::declval<PlainStream&>())) {
                if constexpr (std::is_same_v<std::decay_t<decltype(s)>, std::monostate>)
                    throw ExasolException("WebSocket is not connected", "08003");
                else
                    return f(s);
            }, mStream);
        }

        ws_detail::net::io_context mIoc;
        StreamVariant mStream;
        bool mConnected;
    };

} // namespace exa

#endif // R_EXASOL_WEBSOCKET_WEBSOCKET_CLIENT_H
