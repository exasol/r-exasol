//
// Created by thomas on 08/07/2021.
//

#ifndef R_EXASOL_WRITER_H
#define R_EXASOL_WRITER_H

#include <stdio.h>

namespace exa {
    namespace writer {
        class Writer {
        public:
            virtual ~Writer() {};
            virtual size_t pipe_write(const void *ptr, size_t size, size_t nitems) = 0;
            virtual int pipe_fflush()  = 0;
        };
    }
}

#endif //R_EXASOL_WRITER_H
