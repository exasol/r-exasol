#include <r_exasol/debug_print/debug_printer.h>
#include <r_exasol/debug_print/date.h>
#include <mutex>
#include <iostream>
#include <utility>


namespace exa::debug {
    tLogFunction gLogFunction;
    std::mutex gMutex;
}

bool exa::debug::isLoggingEnabled() {
    return static_cast<bool>(gLogFunction); //check if function has been set externally.
}

void exa::debug::setLogger(exa::debug::tLogFunction logFunction) {
    gLogFunction = std::move(logFunction);
}

void exa::debug::logImpl(const char* caller, const std::string && msg) {
    if (gLogFunction) {
        using namespace std::chrono;
        auto now = time_point_cast<milliseconds>(system_clock::now());
        std::ostringstream oss;
        oss << "([" << caller << "] " << date::format("%T", now) << "): " << msg << '\n';
        const std::lock_guard<std::mutex> lock(exa::debug::gMutex);
        gLogFunction(oss.str().c_str());
    }
}
