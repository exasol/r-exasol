# Feature: Transaction Management

Exasol connections default to autocommit mode. Transaction management temporarily disables autocommit and restores it when the transaction ends. Transaction control uses WebSocket execute and setAttributes commands.

## Background

All transaction operations require an authenticated WebSocket session.

## Scenarios

### Scenario: Begin a transaction

* *GIVEN* an active EXAConnection exists with autocommit enabled
* *WHEN* the user calls dbBegin
* *THEN* autocommit SHALL be disabled via WebSocket setAttributes command
* *AND* the function SHALL return TRUE

### Scenario: Commit a transaction

* *GIVEN* a transaction has been started with dbBegin
* *AND* one or more SQL statements have been executed
* *WHEN* the user calls dbCommit
* *THEN* a COMMIT statement SHALL be executed via WebSocket execute command
* *AND* the function SHALL return TRUE

### Scenario: Rollback a transaction

* *GIVEN* a transaction has been started with dbBegin
* *AND* one or more SQL statements have been executed
* *WHEN* the user calls dbRollback
* *THEN* a ROLLBACK statement SHALL be executed via WebSocket execute command
* *AND* the function SHALL return TRUE

### Scenario: End transaction with commit

* *GIVEN* a transaction has been started with dbBegin
* *WHEN* the user calls dbEnd with commit=TRUE
* *THEN* a COMMIT statement SHALL be executed via WebSocket
* *AND* autocommit SHALL be restored via WebSocket setAttributes command

### Scenario: End transaction with rollback

* *GIVEN* a transaction has been started with dbBegin
* *WHEN* the user calls dbEnd with commit=FALSE
* *THEN* a ROLLBACK statement SHALL be executed via WebSocket
* *AND* autocommit SHALL be restored via WebSocket setAttributes command
