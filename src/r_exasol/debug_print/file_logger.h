#ifndef R_EXASOL_FILE_LOGGER_H
#define R_EXASOL_FILE_LOGGER_H

#include <fstream>

namespace exa {
    namespace debug {
        class FileLogger {
        public:
            bool enableLogging(const char* traceFile);
        private:
            std::ofstream outStream;
        };
    }
}

#endif //R_EXASOL_FILE_LOGGER_H
