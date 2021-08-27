if (Sys.getenv("HAS_LOCAL_EXASOL_TEST_DB") == "true") {
  DBItest::make_context(exasol(),
                        list(exahost = "localhost:8888", uid="sys", pwd="exasol"),
                        tweaks = list(dummy_table = "DUAL"))

  source("run-dbi-tests.R")

}