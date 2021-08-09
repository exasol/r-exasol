# Test Scripts

These scripts are used to run the CI builds on Github Actions.
They can also be used to run the tests locally.
The test will automatically build a docker image for the specified R version and execute the 
integration tests within the docker container.

## Prerequisites

* Docker
* Bash

## Available R version

First, you need to select the R version.
You can get a list of all available versions by running:
```$r-exasol/tests/scripts/cran/get_R_releases.sh```

This will show something like:
```["bionic-cran35/3.5.3-1bionic","bionic-cran35/3.6.3-1bionic","bionic-cran40/4.0.5-1.1804.0","bionic-cran40/4.1.0-1.1804.0"]```

##Run test

Now you can run the test with
```$r-exasol/tests/scripts/execute_docker_test_env.sh bionic-cran35/3.5.3-1bionic```

In order to run test with some memory analyzer, you can also run the skript with valgrind or ASAN:

| Memory check | Command | Description |
|--------------|---------|-------------|
| No check | $r-exasol/tests/scripts/execute_docker_test_env.sh $cran_repo | Runs without any memory check |
| Valgrind  | $r-exasol/tests/scripts/execute_docker_test_env.sh $cran_repo ***valgrind*** | Runs with valgrind |
| Address Sanitizer | $r-exasol/tests/scripts/execute_docker_test_env.sh $cran_repo ***asan*** | Runs with ASAN |
| Address Sanitizer without leak check| $r-exasol/tests/scripts/execute_docker_test_env.sh $cran_repo ***asan_no_leak*** | Runs with ASAN withou leak checks. This is used in the CI builds as R itself causes memory leaks which would break the CI build|



