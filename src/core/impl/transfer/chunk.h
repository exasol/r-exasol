//
// Created by thomas on 08/07/2021.
//

#ifndef R_EXASOL_CHUNK_H
#define R_EXASOL_CHUNK_H

#define  MAX_HTTP_CHUNK_SIZE 524288

namespace exa {
    struct Chunk {
        size_t chunk_len;
        size_t chunk_pos;
        char chunk_buf[MAX_HTTP_CHUNK_SIZE];
        size_t chunk_num;

        Chunk() = default;

        void reset() {
            chunk_len = 0;
            chunk_pos = 0;
            chunk_num = 0;
        }
    };
}

#endif //R_EXASOL_CHUNK_H
