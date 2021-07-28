//
// Created by thomas on 08/07/2021.
//

#ifndef R_EXASOL_RREADERCONNECTION_H
#define R_EXASOL_RREADERCONNECTION_H

#include <r-exasol/rconnection/RconnFwd.h>
#include <r-exasol/rconnection/RConnection.h>
#include <Rdefines.h>
#include <r-exasol/connection/Reader.h>

namespace exa {

    namespace rconnection {

        class RReaderConnection : public RConnection {
        public:
            explicit RReaderConnection(reader::Reader & reader);

            SEXP create();
            void release() override;


        private:
            reader::Reader & mReader;
            Rconnection mConn;
        };
    }
}

#endif //R_EXASOL_RREADERCONNECTION_H
