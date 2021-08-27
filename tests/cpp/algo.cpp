#include "catch2/catch.hpp"

#include <r_exasol/algorithm/repeating_buffer_fill.h>

/**
 * Fill a buffer with integers of 0..99, using exa::algo::repeatingBufferFill,
 * where in each step (bufferFiller) at most 12 elements are added.
 */
TEST_CASE( "repeatingBufferFill", "[algo]" ) {
    typedef std::vector<int> tBuffer;
    tBuffer buffer(100);
    static int counter = 0;
    auto bufferFiller = [] (tBuffer::iterator it, int len) {
        const int currentLen = std::min(12, len);
        for(int idx(0); idx < currentLen; idx++) {
            *it++ = counter++;
        }
        return currentLen;
    };
    const bool result =
            exa::algo::repeatingBufferFill<tBuffer::iterator>(buffer.begin(), buffer.end(), bufferFiller);
    REQUIRE(result);
    counter = 0;
    for (auto c : buffer) {
        REQUIRE(c == counter++);
    }
}

/**
 * Fill a buffer with integers of 0..10, using exa::algo::repeatingBufferFill,
 * where in each step (bufferFiller) at most 12 elements are added.
 */
TEST_CASE( "repeatingBufferFillSmallerBuffer", "[algo]" ) {
    typedef std::vector<int> tBuffer;
    tBuffer buffer(10);
    static int counter = 0;
    auto bufferFiller = [] (tBuffer::iterator it, int len) {
        const int currentLen = std::min(12, len);
        for(int idx(0); idx < currentLen; idx++) {
            *it++ = counter++;
        }
        return currentLen;
    };
    const bool result =
            exa::algo::repeatingBufferFill<tBuffer::iterator>(buffer.begin(), buffer.end(), bufferFiller);
    REQUIRE(result);
    counter = 0;
    for (auto c : buffer) {
        REQUIRE(c == counter++);
    }
}