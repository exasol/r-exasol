#include <r_exasol/ssl/certificate_exception.h>
#include "certificate.h"
#include <openssl/conf.h>
#include <openssl/x509v3.h>
#include <openssl/ssl.h>
#include <string>

//Code based on OpenSSL demo:
//https://opensource.apple.com/source/OpenSSL/OpenSSL-22/openssl/demos/x509/mkcert.c


namespace exa::ssl {

        struct CertExtension {
            CertExtension(int nid, const char* value) :
            mNid(nid),
            mValue(value) {}

            int mNid;
            const char *mValue;
        };

        class CertExtensionBuilder {
        public:
            explicit CertExtensionBuilder(X509 *cert) : mCert(cert) {}

            CertExtensionBuilder& operator << (const CertExtension && certExtension);
        private:
            X509 *mCert;
        };

        /* Add extension using V3 code: we can set the config file as NULL
         * because we wont reference any other sections.
         */

        CertExtensionBuilder &CertExtensionBuilder::operator<<(const CertExtension &&certExtension) {
            X509_EXTENSION *extension;
            X509V3_CTX ctx;
            /* This sets the 'context' of the extensions. */
            /* No configuration database */
            X509V3_set_ctx_nodb(&ctx);
            /* Issuer and subject certs: both the target since it is self signed,
             * no request and no CRL
             */
            X509V3_set_ctx(&ctx, mCert, mCert, nullptr, nullptr, 0);
            extension = X509V3_EXT_conf_nid(nullptr, &ctx, certExtension.mNid, certExtension.mValue);
            if (extension == nullptr) {
                throw CertificateException("Cannot add extension to certificate.");
            }
            X509_add_ext(mCert,extension,-1);
            X509_EXTENSION_free(extension);
            return *this;
        }

        struct CertName {
            CertName(const char * field, int type, const char * bytes)
            : mField(field)
            , mType(type)
            , mBytes(bytes) {}
            const char * mField;
            int mType;
            const char * mBytes;
        };

        class CertNameBuilder  {
        public:
            CertNameBuilder(X509_NAME *name) : mName(name) {}

            CertNameBuilder & operator <<(const CertName && certName);

        private:
            X509_NAME *mName;
        };

        CertNameBuilder &CertNameBuilder::operator<<(const CertName &&certName) {
            const auto* bytes = reinterpret_cast<const unsigned char *>(certName.mBytes);
            int ret = X509_NAME_add_entry_by_txt(mName, certName.mField,
                                                 certName.mType, bytes, -1, -1, 0);
            if (1 != ret) {
                throw CertificateException(std::string("cannot set name of certificate for field:") + certName.mField);
            }
            return *this;
        }
}

void exa::ssl::Certificate::mkcert(int bits, int serial, int days) {
    constexpr long kSecondsPerMinute = 60;
    constexpr long kMinutesPerHour = 60;
    constexpr long kHoursPerDay = 24;

    X509 *cert = nullptr;
    EVP_PKEY *pkey = nullptr;
    RSA *rsa = nullptr;
    X509_NAME *name=nullptr;

    pkey=EVP_PKEY_new();
    cert=X509_new();

    rsa = RSA_new();

    BIGNUM* bignum = nullptr;
    bignum = BN_new();
    BN_set_word(bignum, RSA_F4);
    int ret = RSA_generate_key_ex(rsa, bits, bignum, nullptr);
    BN_free(bignum);
    if (ret == 0) {
        throw CertificateException("Error generating RSA key.");
    }
    if (!EVP_PKEY_assign_RSA(pkey,rsa))
    {
        throw CertificateException("Error assigning RSA key.");
    }
    //According to https://www.openssl.org/docs/man1.1.1/man3/EVP_PKEY_assign_RSA.html
    //pkey is associated with rsa, and OpenSSL will release the memory of rsa together with pkey.
    rsa=nullptr;

    X509_set_version(cert,2);
    ASN1_INTEGER_set(X509_get_serialNumber(cert),serial);
    X509_gmtime_adj(X509_get_notBefore(cert),0);
    X509_gmtime_adj(X509_get_notAfter(cert),static_cast<long>(kSecondsPerMinute*kMinutesPerHour*kHoursPerDay*days));
    X509_set_pubkey(cert,pkey);

    name=X509_get_subject_name(cert);

    /* This function creates and adds the entry, working out the
	 * correct string type and performing checks on its length.
	 * Normally we'd check the return value for errors...
	 */
    CertNameBuilder(name) <<
        CertName("C", MBSTRING_ASC, "DE") <<
        CertName("CN", MBSTRING_ASC, "Exasol Ag");

    /* Its self signed so set the issuer name to be the same as the
 	 * subject.
	 */
    X509_set_issuer_name(cert,name);

    /* Add various extensions: standard extensions */
    CertExtensionBuilder(cert) <<
        CertExtension(NID_basic_constraints, "critical,CA:TRUE") <<
        CertExtension(NID_key_usage, "critical,keyCertSign,cRLSign") <<
        CertExtension(NID_subject_key_identifier, "hash") <<
    /* Some Netscape specific extensions */
        CertExtension(NID_netscape_cert_type, "sslCA") <<
        CertExtension(NID_netscape_comment, "Temporary certificate for r-exasol.");


    if (X509_sign(cert, pkey, EVP_md5()) == 0) {
        throw CertificateException("Cannot create X509 signature.");
    }
    mX509p = cert;
    mPkeyp = pkey;
}

exa::ssl::Certificate::~Certificate() {
    ::X509_free(mX509p);
    ::EVP_PKEY_free(mPkeyp);
}

void exa::ssl::Certificate::apply(SSL_CTX *ctx) const {
    int retVal = ::SSL_CTX_use_certificate(ctx, mX509p);
    if (1 != retVal) {
        throw CertificateException("Error assigning certificate.");
    }
    retVal = ::SSL_CTX_use_PrivateKey(ctx, mPkeyp);
    if (1 != retVal) {
        throw CertificateException("Error assigning private key.");
    }
}

bool exa::ssl::Certificate::isValid() const {
    return (mX509p != nullptr && mPkeyp != nullptr);
}
