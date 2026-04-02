// Suppress OpenSSL 3.x deprecation warnings for RSA bridge functions
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <r_exasol/websocket/exasol_auth.h>
#include <r_exasol/websocket/exasol_error.h>
#include <boost/json.hpp>

#include <openssl/bn.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/opensslv.h>

#include <vector>
#include <stdexcept>

namespace {

    /// Convert a hex string to an OpenSSL BIGNUM. Caller owns the result.
    BIGNUM* hexToBignum(const std::string& hexStr) {
        BIGNUM* bn = nullptr;
        if (BN_hex2bn(&bn, hexStr.c_str()) == 0) {
            throw exa::ExasolException("Failed to convert hex to BIGNUM", "08004");
        }
        return bn;
    }

    /// RAII wrapper for BIGNUM.
    struct BignumDeleter {
        void operator()(BIGNUM* bn) const { if (bn) BN_free(bn); }
    };
    using UniqueBignum = std::unique_ptr<BIGNUM, BignumDeleter>;

} // anonymous namespace

namespace exa {

    std::string ExasolAuth::encryptPassword(
        const std::string& password,
        const std::string& publicKeyModulus,
        const std::string& publicKeyExponent)
    {
        UniqueBignum modulus(hexToBignum(publicKeyModulus));
        UniqueBignum exponent(hexToBignum(publicKeyExponent));

#if OPENSSL_VERSION_NUMBER >= 0x30000000L
        // OpenSSL 3.x: use EVP_PKEY API
        EVP_PKEY* pkey = EVP_PKEY_new();
        if (!pkey) {
            throw ExasolException("Failed to create EVP_PKEY", "08004");
        }

        // Build RSA key from raw BIGNUM components via OSSL_PARAM_BLD
        // For OpenSSL 3.x, we use the deprecated but still-available RSA route
        // via EVP_PKEY_set1_RSA, or the modern OSSL_PARAM_BLD approach.
        // Using the RSA-to-EVP bridge for broader 3.x compatibility.

        RSA* rsa = RSA_new();
        if (!rsa) {
            EVP_PKEY_free(pkey);
            throw ExasolException("Failed to create RSA structure", "08004");
        }

        // RSA_set0_key takes ownership of the BIGNUMs on success
        BIGNUM* n = BN_dup(modulus.get());
        BIGNUM* e = BN_dup(exponent.get());
        if (RSA_set0_key(rsa, n, e, nullptr) != 1) {
            BN_free(n);
            BN_free(e);
            RSA_free(rsa);
            EVP_PKEY_free(pkey);
            throw ExasolException("Failed to set RSA key components", "08004");
        }

        if (EVP_PKEY_assign_RSA(pkey, rsa) != 1) {
            // rsa is freed with pkey on failure path
            EVP_PKEY_free(pkey);
            throw ExasolException("Failed to assign RSA to EVP_PKEY", "08004");
        }

        EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, nullptr);
        if (!ctx) {
            EVP_PKEY_free(pkey);
            throw ExasolException("Failed to create EVP_PKEY_CTX", "08004");
        }

        if (EVP_PKEY_encrypt_init(ctx) <= 0) {
            EVP_PKEY_CTX_free(ctx);
            EVP_PKEY_free(pkey);
            throw ExasolException("Failed to init EVP encryption", "08004");
        }

        if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) <= 0) {
            EVP_PKEY_CTX_free(ctx);
            EVP_PKEY_free(pkey);
            throw ExasolException("Failed to set RSA padding", "08004");
        }

        // Determine output size
        size_t outLen = 0;
        const auto* inputData = reinterpret_cast<const unsigned char*>(password.data());
        size_t inputLen = password.size();

        if (EVP_PKEY_encrypt(ctx, nullptr, &outLen, inputData, inputLen) <= 0) {
            EVP_PKEY_CTX_free(ctx);
            EVP_PKEY_free(pkey);
            throw ExasolException("Failed to determine encryption output size", "08004");
        }

        std::vector<unsigned char> encrypted(outLen);
        if (EVP_PKEY_encrypt(ctx, encrypted.data(), &outLen, inputData, inputLen) <= 0) {
            EVP_PKEY_CTX_free(ctx);
            EVP_PKEY_free(pkey);
            throw ExasolException("RSA encryption failed", "08004");
        }

        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);

        std::vector<unsigned char> base64Out(4 * ((outLen + 2) / 3) + 1);
        int base64Len = EVP_EncodeBlock(base64Out.data(), encrypted.data(),
                                        static_cast<int>(outLen));
        return std::string(reinterpret_cast<const char*>(base64Out.data()), base64Len);

#else
        // OpenSSL 1.x: use RSA_public_encrypt directly
        RSA* rsa = RSA_new();
        if (!rsa) {
            throw ExasolException("Failed to create RSA structure", "08004");
        }

        BIGNUM* n = BN_dup(modulus.get());
        BIGNUM* e = BN_dup(exponent.get());
        if (RSA_set0_key(rsa, n, e, nullptr) != 1) {
            BN_free(n);
            BN_free(e);
            RSA_free(rsa);
            throw ExasolException("Failed to set RSA key components", "08004");
        }

        int rsaSize = RSA_size(rsa);
        std::vector<unsigned char> encrypted(rsaSize);

        int encryptedLen = RSA_public_encrypt(
            static_cast<int>(password.size()),
            reinterpret_cast<const unsigned char*>(password.data()),
            encrypted.data(),
            rsa,
            RSA_PKCS1_PADDING
        );

        RSA_free(rsa);

        if (encryptedLen < 0) {
            throw ExasolException("RSA encryption failed", "08004");
        }

        std::vector<unsigned char> base64Out(4 * ((encryptedLen + 2) / 3) + 1);
        int base64Len = EVP_EncodeBlock(base64Out.data(), encrypted.data(),
                                        encryptedLen);
        return std::string(reinterpret_cast<const char*>(base64Out.data()), base64Len);
#endif
    }

    LoginResponse ExasolAuth::login(
        WebSocketClient& ws,
        const std::string& username,
        const std::string& password,
        int protocolVersion)
    {
        // Step 1: Send login command
        boost::json::object loginCmd;
        loginCmd["command"] = "login";
        loginCmd["protocolVersion"] = protocolVersion;

        std::string rawResponse = ws.sendAndReceive(boost::json::serialize(loginCmd));
        boost::json::value response = parseResponse(rawResponse);

        // Step 2: Extract public key from response
        const auto& responseData = response.as_object().at("responseData").as_object();
        std::string pubKeyModulus = std::string(responseData.at("publicKeyModulus").as_string());
        std::string pubKeyExponent = std::string(responseData.at("publicKeyExponent").as_string());

        // Step 3: Encrypt password
        std::string encryptedPassword = encryptPassword(password, pubKeyModulus, pubKeyExponent);

        // Step 4: Send credentials
        boost::json::object authCmd;
        authCmd["username"] = username;
        authCmd["password"] = encryptedPassword;
        authCmd["useCompression"] = false;
        authCmd["clientName"] = "r-exasol";
        authCmd["driverName"] = "r-exasol";
        authCmd["clientOs"] = "R";
        authCmd["clientVersion"] = "1.0.0";
        authCmd["attributes"] = boost::json::object{};

        rawResponse = ws.sendAndReceive(boost::json::serialize(authCmd));
        response = parseResponse(rawResponse);

        // Step 5: Parse session info
        const auto& sessionData = response.as_object().at("responseData").as_object();
        LoginResponse result;
        result.sessionId = jsonValueOr(sessionData, "sessionId", int64_t(0));
        result.protocolVersion = jsonValueOr(sessionData, "protocolVersion", 0);
        result.releaseVersion = jsonValueOr(sessionData, "releaseVersion", std::string(""));
        result.databaseName = jsonValueOr(sessionData, "databaseName", std::string(""));
        result.productName = jsonValueOr(sessionData, "productName", std::string(""));
        result.maxDataMessageSize = jsonValueOr(sessionData, "maxDataMessageSize", 0);
        result.maxIdentifierLength = jsonValueOr(sessionData, "maxIdentifierLength", 0);
        result.maxVarcharLength = jsonValueOr(sessionData, "maxVarcharLength", 0);
        result.identifierQuoteString = jsonValueOr(sessionData, "identifierQuoteString", std::string("\""));
        result.timeZone = jsonValueOr(sessionData, "timeZone", std::string(""));
        result.timeZoneBehavior = jsonValueOr(sessionData, "timeZoneBehavior", std::string(""));

        return result;
    }

} // namespace exa

#pragma GCC diagnostic pop
