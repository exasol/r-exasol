#ifndef R_EXASOL_RREADERCONNECTION_H
#define R_EXASOL_RREADERCONNECTION_H

#include <r-exasol/rconnection/RconnFwd.h>
#include <r-exasol/rconnection/RConnection.h>
#include <Rdefines.h>
#include <r-exasol/connection/Reader.h>
#include <memory>

namespace exa {

    namespace rconnection {

        /**
         * Implements r-reader connection. Serves as a bridge between the network reader(@class exa::reader::Reader) and the R client.
         */
        class RReaderConnection : public exa::rconnection::RConnection {
        public:
            explicit RReaderConnection(std::weak_ptr<exa::reader::Reader> reader);

            /**
             * Creates the R-connection object(Using R_new_custom_connection). The R- client can use this object
             * to stream data into a R dataframe.
             * @return R-connection object.
             */
            SEXP create();
            /**
             * Reset's the connection hook.
             */
            void release() override;

        private:
            std::weak_ptr<exa::reader::Reader> mReader;
            ::Rconnection mConn;
        };
    }
}

#endif //R_EXASOL_RREADERCONNECTION_H
