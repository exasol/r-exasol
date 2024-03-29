#include <r_exasol/connection/protocol/common.h>
#include <r_exasol/connection/connection_exception.h>

namespace exa {

    void readHttpHeader(Socket& socket) {
        char line[4096], data = '\0';;
        ssize_t len = -1;
        int pos = 0;

        line[0] = '\0';
        do {
            if (pos > 4094) {
                throw exa::ConnectionException ("Could not read header, line too long.");
            }
            len = socket.recv(&data, 1);

            if (len != 1) {
                throw exa::ConnectionException ("Could not receive header");
            }
            line[pos++] = data;
            line[pos] = '\0';
            if (data == '\n' && pos > 1 && line[pos-2] == '\r') {
                if (pos == 2) {
                    break; /* header finished */
                }
                pos = 0;
                line[0] = '\0';
            }
        } while(true);
    }
}
