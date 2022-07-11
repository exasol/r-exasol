# r-exasol 7.1.0, released 2022-07-11

Code name: Fix for R4.2 on Windows

## Summary 

This release fixes several bugs, mainly a linker error which occurred on R 4.2 under Windows. Also, there were some improvements in the documentation and the Github action for the release job was changed, so that the binary R packages for all supported platforms (Windows/Linux/MacOsX) is now stored in the Github release. 

## Features / Enhancements

 - #81: Removed dummy workflows and added upload release artifacts workflows

## Bug Fixes
 - #120: Added job name to remaining GH workflows
 - #115: Added job-names to GH Workflows
 - #106: Fixed installation issue with OpenSSL already installed message
 - #93: Fixed cloned connection
 - #91: Fixed issue with encoding passwords correctly
 - #125: Fixed linker error on windows with R4.2
 - #123: Fixed nightly builds

## Documentation

 - #52: Cleaned up README
 - #105: Changed install documentation for Linux
 - #127: Prepared release 7.1.0

### Compile Dependency Updates

#### Linux
n/a

#### Windows
n/a

### Runtime Dependency Updates
n/a

### Test Dependency Updates
n/a
