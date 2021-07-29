#ifndef R_EXASOL_HTTPCHUNKWRITER_H
#define R_EXASOL_HTTPCHUNKWRITER_H

#include <r-exasol/connection/socket/Socket.h>
#include <r-exasol/connection/protocol/http/Chunk.h>
#include <r-exasol/connection/Writer.h>
#include <memory>

namespace exa {

    namespace writer {

        /**
         * Writer implementation which provides writing access to an Http connection.
         * This implementation expects a reference to a given memory buffer (@private mChunk) which can be used to
         *  cache data between the network connection and client access.
         */
        class HttpChunkWriter : public Writer {
        public:
            ~HttpChunkWriter() override = default;
            explicit HttpChunkWriter(std::weak_ptr<Socket> socket, Chunk & chunk);
            void start() override;
            size_t pipe_write(const void *ptr, size_t size, size_t nitems) override;
            int pipe_fflush() override;

        private:
            ssize_t write_next_chunk();

        private:
            std::weak_ptr<Socket> mSocket;
            Chunk &mChunk;
        };
    }
}


#endif //R_EXASOL_HTTPCHUNKWRITER_H
