#ifndef R_EXASOL_TEST_UTILS_H
#define R_EXASOL_TEST_UTILS_H

#include <string>
#include <sstream>
#include <iterator>

namespace test_utils {

    static const char host[] = "localhost";
    enum {
        PORT = 5000
    };

    inline std::string createTestString() {
        std::ostringstream os;
        std::fill_n(std::ostream_iterator<std::string>(os), 20, "CHUNK DATA;");
        return os.str();
    }
}


#endif //R_EXASOL_TEST_UTILS_H
