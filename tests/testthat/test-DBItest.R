if (Sys.getenv("HAS_LOCAL_EXASOL_TEST_DB") == "true") {
  DBItest::make_context(exasol_driver(),
                        list(exahost = "exasol-test-database:8888", uid="sys", pwd="exasol", sslcertificate="/certificate/rootCA.crt"),
                        tweaks = list(dummy_table = "DUAL"))

  source("run-dbi-tests.R")
}