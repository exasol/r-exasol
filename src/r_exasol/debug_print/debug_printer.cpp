#include <r_exasol/debug_print/debug_printer.h>
#include <r_exasol/debug_print/date.h>
#include <mutex>
#include <iostream>


namespace exa {
    namespace debug {
        tLogFunction gLogFunction;
        std::mutex gMutex;
    }
}

bool exa::debug::isLoggingEnabled() {
    return static_cast<bool>(gLogFunction); //check if function has been set externally.
}

void exa::debug::setLogger(exa::debug::tLogFunction logFunction) {
    gLogFunction = logFunction;
}

void exa::debug::logImpl(const char* caller, const std::string && msg) {
    if (gLogFunction) {
        using namespace std::chrono;
        auto now = time_point_cast<milliseconds>(system_clock::now());
        std::ostringstream o;
        o << "([" << caller << "] " << date::format("%T", now) << "): " << msg << std::endl;
        const std::lock_guard<std::mutex> lock(exa::debug::gMutex);
        gLogFunction(o.str().c_str());
    }
}
