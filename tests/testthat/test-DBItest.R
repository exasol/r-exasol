DBItest::make_context(exasol(), list(dsn="exasolution-uo2214lv1_64", schema="PUB864"), tweaks = list(dummy_table = "DUAL"))
#DBItest::make_context(exasol(), list(dsn="EXASolo", schema="TEST"), tweaks = list(dummy_table = "DUAL"))
DBItest::test_getting_started()
DBItest::test_driver(skip=c("constructor_strict" # stays skipped for now
                            ))
DBItest::test_connection()
#DBItest::test_result(skip="stale_result_warning")

#DBItest::test_sql()
#DBItest::test_meta()
#DBItest::test_compliance()
