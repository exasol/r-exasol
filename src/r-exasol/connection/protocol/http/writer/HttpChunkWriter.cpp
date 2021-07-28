//
// Created by thomas on 08/07/2021.
//

#include <r-exasol/connection/protocol/http/writer/HttpChunkWriter.h>
#include <r-exasol/connection/protocol/http/common.h>
#include <r-exasol/connection/ConnectionException.h>
#include <cstring>
#include <utility>

namespace wri = exa::writer;

wri::HttpChunkWriter::HttpChunkWriter(std::weak_ptr<Socket> socket, Chunk & chunk)
: mSocket(std::move(socket))
, mChunk(chunk) {
    mChunk.reset();
}

ssize_t wri::HttpChunkWriter::write_next_chunk() {
    static const char *ok_answer =
            "HTTP/1.1 200 OK\r\n"
            "Server: EXASolution R Package\r\n"
            "Content-type: application/octet-stream\r\n"
            "Content-disposition: attachment; filename=data.csv\r\n"
            "Connection: close\r\n\r\n";

    const size_t ok_len = strlen(ok_answer);
    const size_t chunk_len = mChunk.chunk_len;
    static const char *error_answer =
            "HTTP/1.1 404 ERROR\r\n"
            "Server: EXASolution R Package\r\n"
            "Connection: close\r\n\r\n";
    ssize_t retVal = -1;

    auto socket = mSocket.lock();
    try {
        if (chunk_len == 0) {
            throw exa::ConnectionException("invalid chunklen");
        }

        if (socket) {
            if (mChunk.chunk_num == 0) {
                if (socket->send(ok_answer, ok_len) != ok_len) {
                    throw exa::ConnectionException("socket invalid");
                }
            }

            if (socket->send(mChunk.chunk_buf, chunk_len) != chunk_len) {
                throw exa::ConnectionException("socket invalid");
            }

            mChunk.chunk_len = 0;
            mChunk.chunk_num++;
            retVal = chunk_len;
        }
    } catch (const ConnectionException& ex) {
        if (socket) {
            socket->send(error_answer, strlen(error_answer));
            socket->shutdownRdWr();
        }
    }
    return retVal;
}

size_t wri::HttpChunkWriter::pipe_write(const void *ptr, size_t size, size_t nitems) {

    char *src = (char*) ptr;
    size_t cur_rest = MAX_HTTP_CHUNK_SIZE - mChunk.chunk_len;
    size_t len = size * nitems;

    for (;;) {
        if (cur_rest >= len) {
            ::memcpy(&(mChunk.chunk_buf[mChunk.chunk_len]), src, len);
            mChunk.chunk_len += len;
            return len;
        }

        if (cur_rest > 0) {
            ::memcpy(&(mChunk.chunk_buf[mChunk.chunk_len]), src, cur_rest);
            mChunk.chunk_len += cur_rest;
            src = &(src[cur_rest]);
            len = len - cur_rest;
        }

        if (write_next_chunk() < 0)
            return -1;

        cur_rest = MAX_HTTP_CHUNK_SIZE - mChunk.chunk_len;
    }
    return -1;
}

int wri::HttpChunkWriter::pipe_fflush() {
    if (mChunk.chunk_len > 0) {
        if (write_next_chunk() < 0)
            return -1;
        else return 0;
    }
    return 0;
}

void exa::writer::HttpChunkWriter::start() {
    auto socket = mSocket.lock();
    if (socket) {
        exa::readHttpHeader(*socket);
    } else {
        throw ConnectionException("socket invalid");
    }
}
