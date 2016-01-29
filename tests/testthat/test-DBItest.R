DBItest::make_context(exasol(),
                      list(dsn = "exasolution-uo2214lv1_64", schema = "PUB864"),
                      tweaks = list(dummy_table = "DUAL"))

#DBItest::make_context(exasol(), list(dsn="EXASolo", schema="TEST"), tweaks = list(dummy_table = "DUAL"))
DBItest::test_getting_started()

# some tests stay skipped for now
# stress_load_unload and stress_load_connect_unload are also skipped on travis
DBItest::test_driver(skip = c("constructor_strict",
                              "stress_load_unload"))
DBItest::test_connection(skip = c("stress_load_connect_unload"))

DBItest::test_result(skip="stale_result_warning")

#DBItest::test_sql()
#DBItest::test_meta()
#DBItest::test_compliance()

context("Additional tests")
test_that("dbCurrentSchema", {
  ctx <- DBItest::get_default_context()
  con <- DBItest:::connect(ctx)
  expect_is(dbCurrentSchema(con), "EXAConnection")
})
