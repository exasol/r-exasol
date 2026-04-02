# Feature: WebSocket Protocol

The WebSocket protocol layer handles low-level communication with Exasol databases using the JSON-over-WebSocket protocol (v3). This delta changes the transport implementation from ixwebsocket to Boost.Beast synchronous API.

## Background

The WebSocket connection targets Exasol 7.1+ using protocol version 3. Authentication uses RSA public key exchange with PKCS#1 v1.5 padding. All messages follow a JSON request/response pattern with `command` and `status` fields. Base64 encoding uses OpenSSL EVP functions.

## Scenarios

<!-- DELTA:CHANGED -->
### Scenario: Establish WebSocket connection

* *GIVEN* an Exasol database is running and reachable
* *WHEN* the client opens a WebSocket connection to the database host and port
* *THEN* a WebSocket connection SHALL be established using Boost.Beast synchronous API
* *AND* the connection SHALL use `boost::beast::websocket::stream<boost::beast::ssl_stream<tcp::socket>>` when TLS is enabled
* *AND* the connection SHALL use `boost::beast::websocket::stream<tcp::socket>` when TLS is disabled
* *AND* TLS connections SHALL skip certificate verification by default for self-signed certificates
<!-- /DELTA:CHANGED -->

<!-- DELTA:CHANGED -->
### Scenario: Send command and receive response

* *GIVEN* an authenticated WebSocket session exists
* *WHEN* the client sends a JSON command
* *THEN* the client SHALL use `boost::beast::websocket::stream::write` to send text frames
* *AND* the client SHALL use `boost::beast::websocket::stream::read` to receive the response synchronously
* *AND* a read timeout of 300 seconds SHALL apply
<!-- /DELTA:CHANGED -->

<!-- DELTA:CHANGED -->
### Scenario: Close WebSocket connection

* *GIVEN* an authenticated WebSocket session exists
* *WHEN* the client sends a disconnect command
* *THEN* the client SHALL send the disconnect JSON without waiting for a response
* *AND* the client SHALL close the WebSocket stream via `boost::beast::websocket::stream::close`
<!-- /DELTA:CHANGED -->
