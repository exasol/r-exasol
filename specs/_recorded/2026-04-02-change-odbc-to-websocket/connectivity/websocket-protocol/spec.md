# Feature: WebSocket Protocol

The WebSocket protocol layer handles low-level communication with Exasol databases using the JSON-over-WebSocket protocol (v3, with optional v4 negotiation). All commands are serialized to JSON and sent over WebSocket frames. Responses are deserialized and validated. Connections use secure WebSocket (wss://) when TLS is enabled and plain WebSocket (ws://) otherwise.

## Background

The WebSocket connection targets Exasol 7.1+ using protocol version 3. Authentication uses RSA public key exchange with PKCS#1 v1.5 padding. All messages follow a JSON request/response pattern with `command` and `status` fields.

## Scenarios

### Scenario: Establish WebSocket connection

* *GIVEN* an Exasol database is running and reachable
* *WHEN* the client opens a WebSocket connection to the database host and port
* *THEN* a WebSocket connection SHALL be established
* *AND* the connection SHALL use wss:// when TLS is enabled
* *AND* the connection SHALL use ws:// when TLS is disabled

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
* *THEN* the command SHALL contain a "command" field
* *AND* the response SHALL contain a "status" field with value "ok" or "error"
* *AND* on success the response MAY contain a "responseData" field

### Scenario: Handle error response

* *GIVEN* an authenticated WebSocket session exists
* *WHEN* the server responds with status "error"
* *THEN* the client SHALL parse the "exception" object
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
* *THEN* the WebSocket connection SHALL be closed cleanly
