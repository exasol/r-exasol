#ifndef R_EXASOL_WEBSOCKET_EXASOL_RESPONSE_H
#define R_EXASOL_WEBSOCKET_EXASOL_RESPONSE_H

#include <string>
#include <cstdint>
#include <boost/json.hpp>

namespace exa {

    /// Parse a JSON response string from Exasol.
    /// Throws ExasolException if the response status is "error".
    boost::json::value parseResponse(const std::string& responseStr);

    /// Helper: extract a value from a JSON object with a default fallback.
    template<typename T>
    T jsonValueOr(const boost::json::object& obj, const char* key, const T& defaultVal);

    template<>
    inline int jsonValueOr<int>(const boost::json::object& obj, const char* key, const int& defaultVal) {
        auto it = obj.find(key);
        if (it == obj.end() || it->value().is_null()) return defaultVal;
        return static_cast<int>(it->value().to_number<int64_t>());
    }

    template<>
    inline int64_t jsonValueOr<int64_t>(const boost::json::object& obj, const char* key, const int64_t& defaultVal) {
        auto it = obj.find(key);
        if (it == obj.end() || it->value().is_null()) return defaultVal;
        return it->value().to_number<int64_t>();
    }

    template<>
    inline double jsonValueOr<double>(const boost::json::object& obj, const char* key, const double& defaultVal) {
        auto it = obj.find(key);
        if (it == obj.end() || it->value().is_null()) return defaultVal;
        return it->value().to_number<double>();
    }

    template<>
    inline std::string jsonValueOr<std::string>(const boost::json::object& obj, const char* key, const std::string& defaultVal) {
        auto it = obj.find(key);
        if (it == obj.end() || it->value().is_null()) return defaultVal;
        return std::string(it->value().as_string());
    }

} // namespace exa

#endif // R_EXASOL_WEBSOCKET_EXASOL_RESPONSE_H
