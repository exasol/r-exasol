#ifndef R_EXASOL_CHUNK_H
#define R_EXASOL_CHUNK_H

#include <cstring>

#define  MAX_HTTP_CHUNK_SIZE 524288

namespace exa {
    /**
     * Memory buffer which will be used to exchange data between network connection and client.
     */
    struct Chunk {
        size_t chunk_len;
        size_t chunk_pos;
        char chunk_buf[MAX_HTTP_CHUNK_SIZE];
        size_t chunk_num;

        Chunk() {
            reset();
        }

        void reset() {
            chunk_len = 0;
            chunk_pos = 0;
            chunk_num = 0;
            ::memset(chunk_buf, 0, MAX_HTTP_CHUNK_SIZE);
        }
    };
}

#endif //R_EXASOL_CHUNK_H
