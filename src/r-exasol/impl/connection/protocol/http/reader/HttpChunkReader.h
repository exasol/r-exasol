//
// Created by thomas on 08/07/2021.
//

#ifndef R_EXASOL_HTTPCHUNKREADER_H
#define R_EXASOL_HTTPCHUNKREADER_H

#include <r-exasol/impl/connection/socket/Socket.h>
#include <r-exasol/impl/connection/protocol/http/Chunk.h>
#include <r-exasol/if/Reader.h>

namespace exa {
    namespace reader {

        class HttpChunkReader : public Reader {
        public:
            ~HttpChunkReader() override = default;
            explicit HttpChunkReader(Socket &socket, Chunk & chunk);
            void start() override;
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
