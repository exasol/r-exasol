#ifndef R_EXASOL_R_WRITER_CONNECTION_H
#define R_EXASOL_R_WRITER_CONNECTION_H

#include <r_exasol/rconnection/r_conn_fwd.h>
#include <r_exasol/rconnection/r_connection.h>
#include <Rdefines.h>
#include <r_exasol/connection/writer.h>
#include <memory>

namespace exa {
    namespace rconnection {
        /**
         * Implements r-writer connection. Serves as a bridge between the network writer(@class exa::writer::Writer) and the R client.
         */
        class RWriterConnection : public exa::rconnection::RConnection {
        public:
            explicit RWriterConnection(std::weak_ptr<writer::Writer> writer);

            /**
             * Creates the R-connection object(Using R_new_custom_connection). The R- client can use this object
             * to stream data from a R dataframe.
             * @return R-connection object.
             */
            SEXP create();
            /**
             * Reset's the connection hook.
             */
            void release() override;

        private:
            std::weak_ptr<exa::writer::Writer> mWriter;
            ::Rconnection mConn;
        };
    }
}


#endif //R_EXASOL_R_WRITER_CONNECTION_H
