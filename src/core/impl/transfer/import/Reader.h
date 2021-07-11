//
// Created by thomas on 08/07/2021.
//

#ifndef R_EXASOL_READER_H
#define R_EXASOL_READER_H

#include <cstdio>

namespace exa {
    namespace import {
        class Reader {
        public:
            virtual ~Reader() {};
            virtual int fgetc() = 0;
            virtual size_t pipe_read(void *ptr, const size_t size, const size_t nitems) = 0;
        };
    }
}

#endif //R_EXASOL_READER_H
