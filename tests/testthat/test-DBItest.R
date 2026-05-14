if (Sys.getenv("HAS_LOCAL_EXASOL_TEST_DB") == "true") {
  DBItest::make_context(
    exasol_driver(),
    list(
      exahost        = Sys.getenv("EXAHOST",           "exasol-test-database:8888"),
      uid            = Sys.getenv("EXAUID",            "sys"),
      pwd            = Sys.getenv("EXAPWD",            "exasol"),
      sslcertificate = Sys.getenv("EXASSLCERTIFICATE", "/certificate/rootCA.crt")
    ),
    tweaks = list(dummy_table = "DUAL"))

  source("run-dbi-tests.R")
}