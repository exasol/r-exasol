#include "file_logger.h"
#include <r_exasol/debug_print/debug_printer.h>

bool exa::debug::FileLogger::enableLogging(const char *traceFile) {
    outStream.open(traceFile);
    setLogger([this](const char* msg) {
        if (outStream.is_open()) {
        outStream << msg;
        outStream.flush();
    }});
    return outStream.is_open();
}
