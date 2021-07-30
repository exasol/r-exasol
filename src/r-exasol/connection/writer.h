#ifndef R_EXASOL_WRITER_H
#define R_EXASOL_WRITER_H

#include <cstdio>

namespace exa {
    namespace writer {
        /**
         * Abstract writer class which declares a interface compatible with
         * external streaming writer.
         */
        class Writer {
        public:
            virtual ~Writer() = default;
            virtual void start() = 0;
            virtual size_t pipe_write(const void *ptr, size_t size, size_t nitems) = 0;
            virtual int pipe_fflush()  = 0;
        };
    }
}

#endif //R_EXASOL_WRITER_H
