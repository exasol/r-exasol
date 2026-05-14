# Feature: WebSocket Protocol

The WebSocket protocol layer handles low-level communication with Exasol databases using the JSON-over-WebSocket protocol (v3, with optional v4 negotiation). All commands are serialized to JSON and sent over WebSocket frames. Responses are deserialized and validated. Connections use secure WebSocket (wss://) when TLS is enabled and plain WebSocket (ws://) otherwise. The transport uses Boost.Beast synchronous API. This delta changes the JSON serialization library from nlohmann/json to Boost.JSON.

## Background

The WebSocket connection targets Exasol 7.1+ using protocol version 3. Authentication uses RSA public key exchange with PKCS#1 v1.5 padding. All messages follow a JSON request/response pattern with `command` and `status` fields. Base64 encoding uses OpenSSL EVP functions. JSON serialization and deserialization uses Boost.JSON.

## Scenarios

### Scenario: Establish WebSocket connection

* *GIVEN* an Exasol database is running and reachable
* *WHEN* the client opens a WebSocket connection to the database host and port
* *THEN* a WebSocket connection SHALL be established using Boost.Beast synchronous API
* *AND* the connection SHALL use `boost::beast::websocket::stream<boost::beast::ssl_stream<tcp::socket>>` when TLS is enabled
* *AND* the connection SHALL use `boost::beast::websocket::stream<tcp::socket>` when TLS is disabled
* *AND* TLS connections SHALL skip certificate verification by default for self-signed certificates

### Scenario: Authenticate with credentials

* *GIVEN* a WebSocket connection is established
* *WHEN* the client sends a login command with protocolVersion 3
* *THEN* the server SHALL respond with an RSA public key
* *AND* the client SHALL encrypt the password using PKCS#1 v1.5 padding
* *AND* the client SHALL send the encrypted password encoded in Base64
* *AND* the server SHALL respond with session metadata on success

### Scenario: Handle authentication failure

* *GIVEN* a WebSocket connection is established
* *WHEN* the client sends invalid credentials
* *THEN* the server SHALL respond with status "error"
* *AND* the response SHALL contain an exception with text and sqlCode
* *AND* the client SHALL raise an R error with the server's error message

### Scenario: Send command and receive response

* *GIVEN* an authenticated WebSocket session exists
* *WHEN* the client sends a JSON command
* *THEN* the client SHALL use `boost::beast::websocket::stream::write` to send text frames
* *AND* the client SHALL use `boost::beast::websocket::stream::read` to receive the response synchronously
* *AND* a read timeout of 300 seconds SHALL apply

### Scenario: Serialize command and deserialize response

* *GIVEN* an authenticated WebSocket session exists
* *WHEN* the client builds an outgoing command and the server returns a response
* *THEN* the client SHALL serialize the command to text using `boost::json::serialize`
* *AND* the client SHALL deserialize the response text using `boost::json::parse`

### Scenario: Handle error response

* *GIVEN* an authenticated WebSocket session exists
* *WHEN* the server responds with status "error"
* *THEN* the client SHALL extract the "exception" object from the response
* *AND* the error SHALL include the "text" and "sqlCode" fields
* *AND* the client SHALL raise an R error with the parsed message

### Scenario: Negotiate protocol version

* *GIVEN* an Exasol database is running
* *WHEN* the client sends a login command with protocolVersion 3
* *AND* the server supports a different version
* *THEN* the client SHALL use the protocol version returned by the server

### Scenario: Close WebSocket connection

* *GIVEN* an authenticated WebSocket session exists
* *WHEN* the client sends a disconnect command
* *THEN* the client SHALL send the disconnect JSON without waiting for a response
* *AND* the client SHALL close the WebSocket stream via `boost::beast::websocket::stream::close`
