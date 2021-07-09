//
// Created by thomas on 08/07/2021.
//

#ifndef R_EXASOL_HTTPCHUNKREADER_H
#define R_EXASOL_HTTPCHUNKREADER_H

#include <impl/socket/Socket.h>
#include <impl/transfer/chunk.h>
#include <impl/transfer/import/Reader.h>

namespace exa {
    namespace import {

        class HttpChunkReader : public Reader {
        public:
            ~HttpChunkReader() override = default;
            explicit HttpChunkReader(Socket &socket, Chunk & chunk);

            int fgetc() override;
            size_t pipe_read(void *ptr, const size_t size, const size_t nitems) override;

        private:
            size_t read_next_chunk();
            size_t read_next(char *buffer, size_t buflen);
        private:
            Socket & mSocket;
            Chunk & mChunk;
        };
    }
}

#endif //R_EXASOL_HTTPCHUNKREADER_H
