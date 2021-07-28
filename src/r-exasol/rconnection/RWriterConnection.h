//
// Created by thomas on 08/07/2021.
//

#ifndef R_EXASOL_RWRITERCONNECTION_H
#define R_EXASOL_RWRITERCONNECTION_H

#include <r-exasol/rconnection/RconnFwd.h>
#include <r-exasol/rconnection/RConnection.h>
#include <Rdefines.h>
#include <r-exasol/connection/Writer.h>
#include <memory>

namespace exa {
    namespace rconnection {
        class RWriterConnection : public RConnection {
        public:
            explicit RWriterConnection(std::weak_ptr<writer::Writer> writer);

            SEXP create();
            void release() override;

        private:
            std::weak_ptr<writer::Writer> mWriter;
            Rconnection mConn;
        };
    }
}


#endif //R_EXASOL_RWRITERCONNECTION_H
