#ifndef R_EXASOL_RWRITERCONNECTION_H
#define R_EXASOL_RWRITERCONNECTION_H

#include <r-exasol/rconnection/RconnFwd.h>
#include <r-exasol/rconnection/RConnection.h>
#include <Rdefines.h>
#include <r-exasol/connection/Writer.h>
#include <memory>

namespace exa {
    namespace rconnection {
        /**
         * Implements r-writer connection. Serves as a bridge between the network writer(@class exa::writer::Writer) and the R client.
         */
        class RWriterConnection : public RConnection {
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
            std::weak_ptr<writer::Writer> mWriter;
            Rconnection mConn;
        };
    }
}


#endif //R_EXASOL_RWRITERCONNECTION_H
