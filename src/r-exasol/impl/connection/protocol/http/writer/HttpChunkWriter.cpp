//
// Created by thomas on 08/07/2021.
//

#include <r-exasol/impl/connection/protocol/http/writer/HttpChunkWriter.h>
#include <r-exasol/impl/connection/protocol/http/common.h>
#include <cstring>

namespace wri = exa::writer;

wri::HttpChunkWriter::HttpChunkWriter(Socket &socket, Chunk & chunk)
: mSocket(socket)
, mChunk(chunk) {
    mChunk.reset();
}

size_t wri::HttpChunkWriter::write_next_chunk() {
    const char *ok_answer =
            "HTTP/1.1 200 OK\r\n"
            "Server: EXASolution R Package\r\n"
            "Content-type: application/octet-stream\r\n"
            "Content-disposition: attachment; filename=data.csv\r\n"
            "Connection: close\r\n\r\n";

    const size_t ok_len = strlen(ok_answer);
    const size_t chunk_len = mChunk.chunk_len;
    const char *error_answer =
            "HTTP/1.1 404 ERROR\r\n"
            "Server: EXASolution R Package\r\n"
            "Connection: close\r\n\r\n";

    if (chunk_len == 0) {
        goto error;
    }

    if (mChunk.chunk_num == 0) {
        if (mSocket.send(ok_answer, ok_len) != ok_len) {
            goto error;
        }
    }

    if (mSocket.send(mChunk.chunk_buf, chunk_len) != chunk_len) {
        goto error;
    }

    mChunk.chunk_len = 0;
    mChunk.chunk_num ++;
    return chunk_len;

    error:
    mSocket.send(error_answer, strlen(error_answer));
    mSocket.shutdownRdWr();
    return -1;
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
    exa::readHttpHeader(mSocket);
}
