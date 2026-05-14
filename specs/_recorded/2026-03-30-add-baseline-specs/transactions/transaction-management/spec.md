# Feature: Transaction Management

Enables R users to control database transactions explicitly, overriding the default autocommit behavior for operations that require atomicity.

## Background

Exasol connections default to autocommit mode. Transaction management temporarily disables autocommit and restores it when the transaction ends. Transaction control uses RODBC's `odbcEndTran` and `odbcSetAutoCommit` internally.

## Scenarios

### Scenario: Begin a transaction

* *GIVEN* an active EXAConnection exists with autocommit enabled
* *WHEN* the user calls `dbBegin(conn)`
* *THEN* autocommit SHALL be disabled
* *AND* the function SHALL return TRUE

### Scenario: Commit a transaction

* *GIVEN* a transaction has been started with `dbBegin`
* *AND* one or more SQL statements have been executed
* *WHEN* the user calls `dbCommit(conn)`
* *THEN* all changes since `dbBegin` SHALL be committed to the database
* *AND* the function SHALL return TRUE

### Scenario: Rollback a transaction

* *GIVEN* a transaction has been started with `dbBegin`
* *AND* one or more SQL statements have been executed
* *WHEN* the user calls `dbRollback(conn)`
* *THEN* all changes since `dbBegin` SHALL be reverted
* *AND* the function SHALL return TRUE

### Scenario: End transaction with commit

* *GIVEN* a transaction has been started with `dbBegin`
* *WHEN* the user calls `dbEnd(conn, commit = TRUE)`
* *THEN* all changes SHALL be committed
* *AND* autocommit SHALL be restored to its original setting

### Scenario: End transaction with rollback

* *GIVEN* a transaction has been started with `dbBegin`
* *WHEN* the user calls `dbEnd(conn, commit = FALSE)`
* *THEN* all changes SHALL be rolled back
* *AND* autocommit SHALL be restored to its original setting
