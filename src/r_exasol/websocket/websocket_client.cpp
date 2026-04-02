#include <r_exasol/websocket/websocket_client.h>
#include <r_exasol/websocket/exasol_error.h>

namespace exa {

    using namespace ws_detail;

    static constexpr int kIdleTimeoutSecs = 300;

    WebSocketClient::WebSocketClient()
        : mConnected(false)
    {
    }

    WebSocketClient::~WebSocketClient() {
        close();
    }

    void WebSocketClient::connect(const std::string& host, int port, bool useTls,
                                   int timeoutSecs) {
        try {
            tcp::resolver resolver(mIoc);
            auto const results = resolver.resolve(host, std::to_string(port));

            if (useTls) {
                ssl::context sslCtx(ssl::context::tlsv12_client);
                sslCtx.set_verify_mode(ssl::verify_none);

                mStream.emplace<SslStream>(mIoc, sslCtx);
                auto& wss = std::get<SslStream>(mStream);

                beast::get_lowest_layer(wss).expires_after(
                    std::chrono::seconds(timeoutSecs));
                beast::get_lowest_layer(wss).connect(results);

                beast::get_lowest_layer(wss).expires_after(
                    std::chrono::seconds(timeoutSecs));
                wss.next_layer().handshake(ssl::stream_base::client);

                websocket::stream_base::timeout wsTimeout =
                    websocket::stream_base::timeout::suggested(
                        beast::role_type::client);
                wsTimeout.idle_timeout = std::chrono::seconds(kIdleTimeoutSecs);
                wss.set_option(wsTimeout);

                beast::get_lowest_layer(wss).expires_never();
                wss.handshake(host + ":" + std::to_string(port), "/");

            } else {
                mStream.emplace<PlainStream>(mIoc);
                auto& wsp = std::get<PlainStream>(mStream);

                beast::get_lowest_layer(wsp).expires_after(
                    std::chrono::seconds(timeoutSecs));
                beast::get_lowest_layer(wsp).connect(results);

                websocket::stream_base::timeout wsTimeout =
                    websocket::stream_base::timeout::suggested(
                        beast::role_type::client);
                wsTimeout.idle_timeout = std::chrono::seconds(kIdleTimeoutSecs);
                wsp.set_option(wsTimeout);

                beast::get_lowest_layer(wsp).expires_never();
                wsp.handshake(host + ":" + std::to_string(port), "/");
            }

            mConnected = true;

        } catch (const std::exception& e) {
            mStream = std::monostate{};
            throw ExasolException(
                std::string("WebSocket connection failed: ") + e.what(),
                "08001");
        }
    }

    std::string WebSocketClient::sendAndReceive(const std::string& jsonMessage) {
        if (!isConnected()) {
            throw ExasolException("WebSocket is not connected", "08003");
        }

        try {
            return visitStream([&](auto& stream) -> std::string {
                stream.write(net::buffer(jsonMessage));
                beast::flat_buffer buffer;
                stream.read(buffer);
                return beast::buffers_to_string(buffer.data());
            });
        } catch (const ExasolException&) {
            throw;
        } catch (const std::exception& e) {
            mConnected = false;
            throw ExasolException(
                std::string("WebSocket communication error: ") + e.what(),
                "08S01");
        }
    }

    void WebSocketClient::sendOnly(const std::string& jsonMessage) {
        if (!isConnected()) {
            return;
        }

        try {
            visitStream([&](auto& stream) {
                stream.write(net::buffer(jsonMessage));
            });
        } catch (...) { // NOLINT(bugprone-empty-catch)
        }
    }

    void WebSocketClient::close() {
        if (!mConnected) {
            return;
        }

        try {
            visitStream([](auto& stream) {
                stream.close(websocket::close_code::normal);
            });
        } catch (...) { // NOLINT(bugprone-empty-catch)
        }

        mConnected = false;
    }

    bool WebSocketClient::isConnected() const {
        return mConnected;
    }

} // namespace exa
