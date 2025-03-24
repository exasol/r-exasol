if (Sys.getenv("HAS_LOCAL_EXASOL_TEST_DB") == "true") {
  DBItest::make_context(exasol_driver(),
                        list(exahost = "localhost/NOCERTCHECK:8888", uid="sys", pwd="exasol", encryption = "Y"),
                        tweaks = list(dummy_table = "DUAL"))

  source("run-dbi-tests.R")
}