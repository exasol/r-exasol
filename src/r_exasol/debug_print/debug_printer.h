#ifndef R_EXASOL_DEBUG_PRINTER_H
#define R_EXASOL_DEBUG_PRINTER_H

#include <functional>
#include <sstream>

namespace exa {

    namespace debug {
        typedef std::function<void (const char *)> tLogFunction;
        void setLogger(tLogFunction logFunction);

        bool isLoggingEnabled();

        void logImpl(const char* caller, const std::string && msg);
    }

    template<typename TCaller>
    class DebugPrinter {
    public:
        template<typename ...Args>
        static void print(const char* msg, Args...args) {
            if (debug::isLoggingEnabled()) {
                std::ostringstream o;
                printToStream(o, msg, args...);
                debug::logImpl(typeid(TCaller).name(), o.str());
            }
        }

    private:
        template<typename T> //end of recursion
        static void printToStream(std::ostringstream & o, T t) {
            o << t;
        }

        template<typename T, typename ...Args> //recursively print all template arguments to the stringstream
        static void printToStream(std::ostringstream & o, T t, Args...args) {
            printToStream(o, t);
            printToStream(o, args...);
        }

    };

    template<typename TCaller>
    class StackTraceLogger {
    public:
        explicit StackTraceLogger(const std::string fName) : funcName(fName) {
            DebugPrinter<TCaller>::print("entering ", typeid(TCaller).name(), "::", funcName.c_str());
        }
        ~StackTraceLogger() {
            DebugPrinter<TCaller>::print("exiting ", typeid(TCaller).name(), "::", funcName.c_str());
        }

    private:
        std::string funcName;
    };



    template<typename TCaller>
    struct ObjectLifecycleLogger {
        ObjectLifecycleLogger() {
            DebugPrinter<TCaller>::print("entering ", typeid(TCaller).name(), "::<ctor>");
        }

        ~ObjectLifecycleLogger() {
            DebugPrinter<TCaller>::print("entering ", typeid(TCaller).name(), "::<dtor>");
        }
    };
}
#endif //R_EXASOL_DEBUG_PRINTER_H
