#ifndef R_EXASOL_READER_H
#define R_EXASOL_READER_H

#include <cstdio>

namespace exa {
    namespace reader {
        /**
         * Abstract reader class which declares a interface compatible with
         * external streaming reader.
         */
        class Reader {
        public:
            virtual ~Reader() = default;
            virtual void start() = 0;
            virtual int fgetc() = 0;
            virtual size_t pipe_read(void *ptr, const size_t size, const size_t nitems) = 0;
        };
    }
}

#endif //R_EXASOL_READER_H
