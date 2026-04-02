# Feature: WebSocket Protocol

The WebSocket protocol layer handles low-level communication with Exasol databases using the JSON-over-WebSocket protocol (v3, with optional v4 negotiation). All commands are serialized to JSON and sent over WebSocket frames. Responses are deserialized and validated. Connections use secure WebSocket (wss://) when TLS is enabled and plain WebSocket (ws://) otherwise. The transport uses Boost.Beast synchronous API. This delta changes the JSON serialization library from nlohmann/json to Boost.JSON.

## Background

The WebSocket connection targets Exasol 7.1+ using protocol version 3. Authentication uses RSA public key exchange with PKCS#1 v1.5 padding. All messages follow a JSON request/response pattern with `command` and `status` fields. Base64 encoding uses OpenSSL EVP functions. JSON serialization and deserialization uses Boost.JSON.

## Scenarios

<!-- DELTA:CHANGED -->
### Scenario: Send command and receive response

* *GIVEN* an authenticated WebSocket session exists
* *WHEN* the client sends a JSON command
* *THEN* the client SHALL serialize the command using `boost::json::serialize`
* *AND* the client SHALL deserialize the response using `boost::json::parse`
* *AND* a read timeout of 300 seconds SHALL apply
<!-- /DELTA:CHANGED -->

<!-- DELTA:CHANGED -->
### Scenario: Handle error response

* *GIVEN* an authenticated WebSocket session exists
* *WHEN* the server responds with status "error"
* *THEN* the client SHALL parse the response using `boost::json::parse`
* *AND* the client SHALL extract the "exception" object from the parsed result
* *AND* the error SHALL include the "text" and "sqlCode" fields
* *AND* the client SHALL raise an R error with the parsed message
<!-- /DELTA:CHANGED -->
