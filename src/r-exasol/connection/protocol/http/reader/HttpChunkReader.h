#ifndef R_EXASOL_HTTPCHUNKREADER_H
#define R_EXASOL_HTTPCHUNKREADER_H

#include <r-exasol/connection/socket/Socket.h>
#include <r-exasol/connection/protocol/http/Chunk.h>
#include <r-exasol/connection/Reader.h>
#include <memory>

namespace exa {
    namespace reader {

        /**
         * Reader implementation which provides reading access from an Http connection.
         * This implementation expects a reference to a given memory buffer (@private mChunk) which can be used to
         *  cache data between the network connection and client access.
         */
        class HttpChunkReader : public Reader {
        public:
            ~HttpChunkReader() override = default;
            explicit HttpChunkReader(std::weak_ptr<Socket> socket, Chunk & chunk);
            void start() override;
            int fgetc() override;
            size_t pipe_read(void *ptr, const size_t size, const size_t nitems) override;

        private:
            ssize_t read_next_chunk();
            ssize_t read_next(char *buffer, size_t buflen);
            void closeSocketWithError();
        private:
            std::weak_ptr<Socket> mSocket;
            Chunk & mChunk;
        };
    }
}

#endif //R_EXASOL_HTTPCHUNKREADER_H
