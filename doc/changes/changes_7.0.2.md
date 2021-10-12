# r-exasol 7.0.2, released 2021-10-12

Code name: Fix socket error under Windows.

## Summary 

This release fixes a critical error under Windows which caused the 
high speed data channel to abort the connection with an error.

## Features / Enhancements
n/a

## Bug Fixes
- #103: exa.ReadData not working correctly under Windows

### Compile Dependency Updates

#### Linux
n/a

#### Windows
n/a

### Runtime Dependency Updates
n/a

### Test Dependency Updates
n/a