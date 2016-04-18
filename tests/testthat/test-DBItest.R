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

DBItest::test_result(skip = c("stale_result_warning",
                              "fetch_premature_close",
                              "data_logical_int",
                              "data_logical_int_null_below",
                              "data_logical_int_null_above",
# following tests are TODOs
                              "data_64_bit",
                              "data_64_bit_null_below",
                              "data_64_bit_null_above",
                              "data_character",
                              "data_character_null_below",
                              "data_character_null_above",
                              "data_raw",
                              "data_raw_null_below",
                              "data_raw_null_above",
                              "data_date",
                              "data_date_null_below",
                              "data_date_null_above",
                              "data_time",
                              "data_time_null_below",
                              "data_time_null_above",
                              "data_time_parens",
                              "data_time_parens_null_below",
                              "data_time_parens_null_above",
                              "data_timestamp",
                              "data_timestamp_null_below",
                              "data_timestamp_null_above",
                              "data_timestamp_utc",
                              "data_timestamp_utc_null_below",
                              "data_timestamp_utc_null_above",
                              "data_timestamp_parens",
                              "data_timestamp_parens_null_below",
                              "data_timestamp_parens_null_above"
                              ))

#DBItest::test_sql()
#DBItest::test_meta()
#DBItest::test_compliance()

context("Additional tests")
test_that("dbCurrentSchema", {
  ctx <- DBItest::get_default_context()
  con <- DBItest:::connect(ctx)
  expect_is(dbCurrentSchema(con), "EXAConnection")
})

