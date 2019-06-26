DBItest::make_context(exasol(),
                      list(exahost = "localhost:8888", uid="sys", pwd="exasol"),
                      tweaks = list(dummy_table = "DUAL"))

#DBItest::make_context(exasol(), list(dsn="EXASolo", schema="TEST"), tweaks = list(dummy_table = "DUAL"))
DBItest::test_getting_started(skip = c("package_name"))

# some tests stay skipped for now
# stress_load_unload and stress_load_connect_unload are also skipped on travis
DBItest::test_driver(skip = c("constructor_strict",
                              "constructor",
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
  dbDisconnect(con)
})

test_that("dbGetQuery_comment_before_select_stmt", {
  ctx <- DBItest::get_default_context()
  con <- DBItest:::connect(ctx)
  dbGetQuery(con,"/* */select 1")
  dbGetQuery(con,"/* */ select 1")
  dbGetQuery(con," /* */select 1")
  dbGetQuery(con," /* */ select 1")
  dbGetQuery(con," /* comment */ select 1")
  dbDisconnect(con)
})
test_that("dbGetQuery_comment_before_select_stmt_with_CTE", {
  ctx <- DBItest::get_default_context()
  con <- DBItest:::connect(ctx)
  dbGetQuery(con,"/* */with t as (select 1 as x) select x from t")
  dbDisconnect(con)
})
