#ifdef WITH_UNIT_TESTS

#include <testthat.h>
#include <r_exasol/websocket/exasol_error.h>
#include <r_exasol/external/nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

context("parseResponse") {

    test_that("parseResponse returns json on status ok") {
        json okResponse;
        okResponse["status"] = "ok";
        okResponse["responseData"] = {{"sessionId", 42}};

        json result = exa::parseResponse(okResponse.dump());
        expect_true(result["status"] == "ok");
        expect_true(result["responseData"]["sessionId"] == 42);
    }

    test_that("parseResponse throws ExasolException on status error") {
        json errResponse;
        errResponse["status"] = "error";
        errResponse["exception"]["text"] = "table not found";
        errResponse["exception"]["sqlCode"] = "42000";

        bool caught = false;
        try {
            exa::parseResponse(errResponse.dump());
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
        json noStatus;
        noStatus["data"] = "something";

        bool caught = false;
        try {
            exa::parseResponse(noStatus.dump());
        } catch (const exa::ExasolException& e) {
            caught = true;
            expect_true(std::string(e.what()) == "Response missing 'status' field");
        }
        expect_true(caught);
    }

    test_that("parseResponse handles error with missing exception details") {
        json errResponse;
        errResponse["status"] = "error";

        bool caught = false;
        try {
            exa::parseResponse(errResponse.dump());
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
