//
// Created by thomas on 08/07/2021.
//

#ifndef R_EXASOL_HTTPCHUNKWRITER_H
#define R_EXASOL_HTTPCHUNKWRITER_H

#include <r-exasol/impl/connection/socket/Socket.h>
#include <r-exasol/impl/connection/protocol/http/Chunk.h>
#include <r-exasol/if/Writer.h>

namespace exa {

    namespace writer {

        class HttpChunkWriter : public Writer {
        public:
            ~HttpChunkWriter() override = default;
            explicit HttpChunkWriter(Socket &socket, Chunk & chunk);
            void start() override;
            size_t pipe_write(const void *ptr, size_t size, size_t nitems) override;
            int pipe_fflush() override;

        private:
            size_t write_next_chunk();

        private:
            Socket &mSocket;
            Chunk &mChunk;
        };
    }
}


#endif //R_EXASOL_HTTPCHUNKWRITER_H
