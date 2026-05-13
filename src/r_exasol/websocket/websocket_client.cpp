#include <r_exasol/websocket/websocket_client.h>
#include <r_exasol/websocket/exasol_exception.h>

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
                                   const std::string& sslCertificate,
                                   int timeoutSecs) {
        try {
            tcp::resolver resolver(mIoc);
            auto const results = resolver.resolve(host, std::to_string(port));

            if (useTls) {
                ssl::context sslCtx(ssl::context::tlsv12_client);
                if (sslCertificate == "SSL_VERIFY_NONE") {
                    sslCtx.set_verify_mode(ssl::verify_none);
                } else if (sslCertificate.empty() || sslCertificate == "SSL_VERIFY_SERVER") {
                    sslCtx.set_verify_mode(ssl::verify_peer);
                    sslCtx.set_default_verify_paths();
                } else {
                    sslCtx.set_verify_mode(ssl::verify_peer);
                    sslCtx.load_verify_file(sslCertificate);
                }

                mStream.emplace<SslStream>(mIoc, sslCtx);
                auto& ws = std::get<SslStream>(mStream);

                beast::get_lowest_layer(ws).expires_after(
                    std::chrono::seconds(timeoutSecs));
                beast::get_lowest_layer(ws).connect(results);

                beast::get_lowest_layer(ws).expires_after(
                    std::chrono::seconds(timeoutSecs));
                ws.next_layer().handshake(ssl::stream_base::client);

                websocket::stream_base::timeout wsTimeout =
                    websocket::stream_base::timeout::suggested(
                        beast::role_type::client);
                wsTimeout.idle_timeout = std::chrono::seconds(kIdleTimeoutSecs);
                ws.set_option(wsTimeout);

                beast::get_lowest_layer(ws).expires_never();
                ws.handshake(host + ":" + std::to_string(port), "/");

            } else {
                mStream.emplace<PlainStream>(mIoc);
                auto& ws = std::get<PlainStream>(mStream);

                beast::get_lowest_layer(ws).expires_after(
                    std::chrono::seconds(timeoutSecs));
                beast::get_lowest_layer(ws).connect(results);

                websocket::stream_base::timeout wsTimeout =
                    websocket::stream_base::timeout::suggested(
                        beast::role_type::client);
                wsTimeout.idle_timeout = std::chrono::seconds(kIdleTimeoutSecs);
                ws.set_option(wsTimeout);

                beast::get_lowest_layer(ws).expires_never();
                ws.handshake(host + ":" + std::to_string(port), "/");
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
            return visitStream([&](auto& ws) -> std::string {
                ws.write(net::buffer(jsonMessage));
                beast::flat_buffer buffer;
                ws.read(buffer);
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
            visitStream([&](auto& ws) {
                ws.write(net::buffer(jsonMessage));
            });
        } catch (...) {
        }
    }

    void WebSocketClient::close() {
        if (!mConnected) {
            return;
        }

        try {
            visitStream([](auto& ws) {
                ws.close(websocket::close_code::normal);
            });
        } catch (...) {
        }

        mConnected = false;
    }

    bool WebSocketClient::isConnected() const {
        return mConnected;
    }

} // namespace exa
