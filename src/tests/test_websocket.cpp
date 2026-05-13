#ifdef WITH_UNIT_TESTS

#include <testthat.h>
#include <r_exasol/websocket/exasol_exception.h>
#include <r_exasol/websocket/exasol_response.h>
#include <boost/json.hpp>
#include <string>

context("parseResponse") {

    test_that("parseResponse returns json on status ok") {
        boost::json::object okResponse;
        okResponse["status"] = "ok";
        boost::json::object respData;
        respData["sessionId"] = 42;
        okResponse["responseData"] = respData;

        boost::json::value result = exa::parseResponse(boost::json::serialize(okResponse));
        expect_true(result.as_object().at("status").as_string() == "ok");
        expect_true(result.as_object().at("responseData").as_object().at("sessionId").as_int64() == 42);
    }

    test_that("parseResponse throws ExasolException on status error") {
        boost::json::object errResponse;
        errResponse["status"] = "error";
        boost::json::object exception;
        exception["text"] = "table not found";
        exception["sqlCode"] = "42000";
        errResponse["exception"] = exception;

        bool caught = false;
        try {
            exa::parseResponse(boost::json::serialize(errResponse));
        } catch (const exa::ExasolException& e) {
            caught = true;
            expect_true(std::string(e.what()) == "table not found");
            expect_true(e.sqlCode() == "42000");
        }
        expect_true(caught);
    }

    test_that("parseResponse throws on invalid json") {
        bool caught = false;
        try {
            exa::parseResponse("not valid json{{{");
        } catch (const exa::ExasolException& e) {
            caught = true;
            expect_true(e.sqlCode() == "00000");
        }
        expect_true(caught);
    }

    test_that("parseResponse throws on missing status field") {
        boost::json::object noStatus;
        noStatus["data"] = "something";

        bool caught = false;
        try {
            exa::parseResponse(boost::json::serialize(noStatus));
        } catch (const exa::ExasolException& e) {
            caught = true;
            expect_true(std::string(e.what()) == "Response missing 'status' field");
        }
        expect_true(caught);
    }

    test_that("parseResponse handles error with missing exception details") {
        boost::json::object errResponse;
        errResponse["status"] = "error";

        bool caught = false;
        try {
            exa::parseResponse(boost::json::serialize(errResponse));
        } catch (const exa::ExasolException& e) {
            caught = true;
            expect_true(std::string(e.what()) == "Unknown error");
            expect_true(e.sqlCode() == "00000");
        }
        expect_true(caught);
    }
}

context("ExasolException") {

    test_that("ExasolException stores sqlCode") {
        exa::ExasolException ex("test message", "42X01");
        expect_true(std::string(ex.what()) == "test message");
        expect_true(ex.sqlCode() == "42X01");
    }
}

#endif // WITH_UNIT_TESTS
