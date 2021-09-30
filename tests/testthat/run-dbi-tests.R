#DBItest::make_context(exasol(), list(dsn="EXASolo", schema="TEST"), tweaks = list(dummy_table = "DUAL"))
DBItest::test_getting_started(skip = c("package_name"))

# some tests stay skipped for now
# stress_load_unload and stress_load_connect_unload are also skipped on github actions
DBItest::test_driver(skip = c("constructor_strict",
                              "constructor",
                              "stress_load_unload"
))
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
                              "data_timestamp_parens_null_above",
                              # TODO Test error or failure, needs to be checked
                              "command_query",
                              "fetch_no_return_value",
                              "table_visible_in_other_connection"

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

test_that("test_dbHasCompleted", {
  ctx <- DBItest::get_default_context()
  con <- DBItest:::connect(ctx)
  rs <- dbSendQuery(con, "SELECT *")
  completed <- dbHasCompleted(rs)
  expect_equal(completed,FALSE)
  dbDisconnect(con)
})

test_that("test_dbListTables_with_schema_filter", {
  ctx <- DBItest::get_default_context()
  con <- DBItest:::connect(ctx)
  schema=toupper("test_dbListTables_with_schema_filter")
  table=toupper("test_table")
  drop_schema=paste0("DROP SCHEMA IF EXISTS ",schema," CASCADE")
  rs=dbSendQuery(con, drop_schema)
  dbClearResult(rs)
  create_schema = paste0("CREATE SCHEMA ",schema)
  rs=dbSendQuery(con, create_schema)
  dbClearResult(rs)
  create_table = paste0("CREATE OR REPLACE TABLE ",schema,".",table," (i int)")
  rs=dbSendQuery(con, create_table)
  dbClearResult(rs)
  tables <- dbListTables(con,schema=schema)
  expect_equal(tables,c(paste0(schema,".",table)))
  dbGetQuery(con, paste0("DROP SCHEMA IF EXISTS ",schema," CASCADE"))
  dbDisconnect(con)
})

# TODO need to be disabled for Travis, because they fail in Travis. Probably, due to the special version of DBI.
# test_that("dbExecute_comment_before_select_stmt", {
#   ctx <- DBItest::get_default_context()
#   con <- DBItest:::connect(ctx)
#   dbExecute(con,"CREATE SCHEMA if not exists test;")
#   dbExecute(con,"/* */select 1")
#   dbExecute(con,"/* */ select 1")
#   dbExecute(con," /* */select 1")
#   dbExecute(con," /* */ select 1")
#   dbExecute(con," /* comment */ select 1")
#   dbDisconnect(con)
# })
# test_that("dbExecute_comment_before_select_stmt_with_CTE", {
#   ctx <- DBItest::get_default_context()
#   con <- DBItest:::connect(ctx)
#   dbExecute(con,"CREATE SCHEMA if not exists test;")
#   dbExecute(con,"/* */with t as (select 1 as x) select x from t")
#   dbDisconnect(con)
# })

context("data mapping tests")

test_that("dbDataType converts empty characters to CLOB data type", {
  ctx <- DBItest::get_default_context()
  con <- DBItest:::connect(ctx)
  df <- data.frame(name=character())
  expect_equal(dbDataType(con, df), "CLOB")
  dbDisconnect(con)
})

test_that("dbDataType converts non-empty characters to data type", {
  ctx <- DBItest::get_default_context()
  con <- DBItest:::connect(ctx)
  df <- data.frame(name=c("a", "b"))
  expect_equal(dbDataType(con, df), "VARCHAR(1)")
  dbDisconnect(con)
})

test_that("dbWriteTable skips importing empty dataframe", {
  ctx <- DBItest::get_default_context()
  con <- DBItest:::connect(ctx)
  df <- data.frame(name=character())
  expect_true(dbWriteTable(con, "TESTTABLE", df, "TESTSCHEMA"))
  dbDisconnect(con)
})

test_that("dbReadTable", {
  ctx <- DBItest::get_default_context()
  con <- DBItest:::connect(ctx)
  df <- data.frame(c(1, 2, 3), c("StringA", "StringB", "StringC"))
  colnames(df) <- c("Id", "Val")
  expect_true(dbWriteTable(con, "TESTTABLE_READ", df, "TESTSCHEMA"))
  res <- dbReadTable(con, "TESTTABLE_READ", "TESTSCHEMA")

  print (res)
  expect_equal(nrow(res), 3)
  expect_equal(res$Id, df$Id)
  expect_equal(res$Val, as.character(df$Val))
  dbDisconnect(con)
})


test_that("dbWriteTable imports dataframe", {
  ctx <- DBItest::get_default_context()
  con <- DBItest:::connect(ctx)
  df <- data.frame(name=c("a", "b"))
  dbWriteTable(con, "TESTTABLE", df, "TESTSCHEMA")
  result <- dbGetQuery(con, "select * from testschema.testtable")
  expect_equal(nrow(result), 2)
  dbDisconnect(con)
})

test_that("dbWriteTable imports dataframe with all <NA>", {
  ctx <- DBItest::get_default_context()
  con <- DBItest:::connect(ctx)
  df <- data.frame(name=c(NA, NA, NA))
  dbWriteTable(con, "TESTTABLE_NA", df, "TESTSCHEMA")
  result <- dbGetQuery(con, "select * from testschema.testtable_na")
  expect_equal(nrow(result), 3)
  expect_type(result$name, "logical")
  dbDisconnect(con)
})

test_that("dbWriteTable imports dataframe with all empty characters", {
  ctx <- DBItest::get_default_context()
  con <- DBItest:::connect(ctx)
  df <- data.frame(name=c("", ""))
  dbWriteTable(con, "TESTTABLE_EMPTY", df, "TESTSCHEMA")
  result <- dbGetQuery(con, "select * from testschema.testtable_empty")
  expect_equal(nrow(result), 2)
  expect_type(result$name, "logical")
  dbDisconnect(con)
})

test_that("dbWriteTable imports dataframe with mixed characters", {
  ctx <- DBItest::get_default_context()
  con <- DBItest:::connect(ctx)
  df <- data.frame(name=c("a", NA, "b", ""))
  dbWriteTable(con, "TESTTABLE_MIXED", df, "TESTSCHEMA")
  result <- dbGetQuery(con, "select * from testschema.testtable_mixed")
  expect_equal(nrow(result), 4)
  expect_type(result$name, "character")
  dbDisconnect(con)
})

# TODO Implement dbAppend
# test_that("dbAppend ", {
#   ctx <- DBItest::get_default_context()
#   con <- DBItest:::connect(ctx)
#   df <- data.frame(name=c(1, 2, 3))
#   dbWriteTable(con, "TESTTABLE_NA", df, "TESTSCHEMA")
#   df <- data.frame(name=c(4, 5, 6))
#   dbAppendTable(con, "TESTTABLE_NA", df , "TESTSCHEMA")
#   result <- dbGetQuery(con, "select * from testschema.testtable_na")
#   expect_equal(nrow(result), 6)
#   expect_type(result$name, "integer")
#   dbDisconnect(con)
# })



#CLEANUP
ctx <- DBItest::get_default_context()
con <- DBItest:::connect(ctx)
dbSendQuery(con, "DROP SCHEMA IF EXISTS TESTSCHEMA CASCADE")
schemas <- dbGetQuery(con, "SELECT SCHEMA_NAME FROM SYS.EXA_ALL_SCHEMAS WHERE SCHEMA_NAME LIKE '%CREATED_BY_R'")
for(schema in schemas[,1]) {
  q <- paste0("DROP SCHEMA IF EXISTS ", schema, " CASCADE")
  dbSendQuery(con, q)
}
dbDisconnect(con)

