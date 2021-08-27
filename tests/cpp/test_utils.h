#ifndef R_EXASOL_TEST_UTILS_H
#define R_EXASOL_TEST_UTILS_H

#include <string>
#include <sstream>
#include <iterator>
#include <r_exasol/connection/socket/socket_impl.h>
#include <r_exasol/connection/socket/ssl_socket_impl.h>
#include <r_exasol/ssl/certificate.h>

namespace test_utils {

    static const char host[] = "localhost";
    enum {
        PORT = 5000
    };

    inline const exa::ssl::Certificate& getCertificate() {
        static exa::ssl::Certificate certificate;
        if( !certificate.isValid()) {
            certificate.mkcert();
        }
        return certificate;
    }

    inline std::string createTestString() {
        std::ostringstream os;
        std::fill_n(std::ostream_iterator<std::string>(os), 20, "CHUNK DATA;");
        return os.str();
    }

    inline std::shared_ptr<exa::SocketImpl> createSocket() {
        std::shared_ptr<exa::SocketImpl> socket = std::make_shared<exa::SocketImpl>();
        //Connect to remote (Python program)
        socket->connect(test_utils::host, test_utils::PORT);
        return socket;
    }

    inline std::shared_ptr<exa::SSLSocketImpl> createSecureSocket() {
        exa::SocketImpl socket;
        //Connect to remote (Python program)
        socket.connect(test_utils::host, test_utils::PORT);
        return std::make_shared<exa::SSLSocketImpl>(socket, getCertificate());
    }
}


#endif //R_EXASOL_TEST_UTILS_H
