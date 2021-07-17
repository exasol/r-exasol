//
// Created by thomas on 08/07/2021.
//

#ifndef R_EXASOL_RWRITERCONNECTION_H
#define R_EXASOL_RWRITERCONNECTION_H

#include <r-exasol/impl/rconnection/RconnFwd.h>
#include <r-exasol/impl/rconnection/RConnection.h>
#include <Rdefines.h>
#include <r-exasol/if/Writer.h>

namespace exa {
    namespace rconnection {
        class RWriterConnection : public RConnection {
        public:
            explicit RWriterConnection(writer::Writer & writer);

            SEXP create();
            void release() override;

        private:
            writer::Writer & mWriter;
            Rconnection mConn;
        };
    }
}


#endif //R_EXASOL_RWRITERCONNECTION_H
