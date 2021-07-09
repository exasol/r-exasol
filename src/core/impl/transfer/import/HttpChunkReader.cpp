//
// Created by thomas on 08/07/2021.
//

#include <impl/transfer/import/HttpChunkReader.h>
#include <cstring>

namespace im = exa::import;

im::HttpChunkReader::HttpChunkReader(Socket &socket, Chunk & chunk)
: mSocket(socket)
, mChunk(chunk) {
    mChunk.reset();
}

size_t im::HttpChunkReader::read_next_chunk() {
    size_t pos = 0;
    int buflen, rc;
    const char *ok_answer =
            "HTTP/1.1 200 OK\r\n"
            "Server: EXASolution R Package\r\n"
            "Connection: close\r\n\r\n";
    const char *error_answer =
            "HTTP/1.1 404 ERROR\r\n"
            "Server: EXASolution R Package\r\n"
            "Connection: close\r\n\r\n";

    for (pos = 0; pos < 20; pos++) {
        mChunk.chunk_buf[pos] = mChunk.chunk_buf[pos + 1] = '\0';
        if ((rc = mSocket.recv(&(mChunk.chunk_buf[pos]), 1)) < 1) {
            // fprintf(stderr, "### error (%d)\n", rc);
            goto error;
        }
        if (mChunk.chunk_buf[pos] == '\n') {
            break;
        }
    }

    if (pos > 19) {
        goto error;
    }

    mChunk.chunk_buf[pos] = '\0';
    buflen = -1;

    if (::sscanf(mChunk.chunk_buf, "%x", &buflen) < 1) {
        goto error;
    }

    if (buflen == 0) {
        mSocket.send(ok_answer, strlen(ok_answer));
        mSocket.shutdownWr();
        return 0;
    }

    if ((buflen + 2) > MAX_HTTP_CHUNK_SIZE) {
        goto error;
    }

    buflen = mSocket.recv(mChunk.chunk_buf, buflen + 2);
    if (buflen < 3) {
        goto error;
    }

    mChunk.chunk_len = buflen - 2;
    mChunk.chunk_pos = 0;
    mChunk.chunk_buf[buflen-2] = '\0';
    mChunk.chunk_num ++;
    return mChunk.chunk_len;

    error:
    mSocket.send(error_answer, strlen(error_answer));
    mSocket.shutdownWr();
    return -1;
}

size_t im::HttpChunkReader::read_next(char *buffer, size_t buflen) {

    size_t rest_chunk = mChunk.chunk_len - mChunk.chunk_pos;
    ssize_t readlen = 0, retlen = 0;
    char *buf = buffer;

    for (;;) {
        if (buflen <= rest_chunk) {
            ::memcpy(buf, &(mChunk.chunk_buf[mChunk.chunk_pos]), buflen);
            mChunk.chunk_pos += buflen;
            retlen += buflen;
            return retlen;
        }

        ::memcpy(buf, &(mChunk.chunk_buf[mChunk.chunk_pos]), rest_chunk);
        retlen += rest_chunk;

        readlen = read_next_chunk();
        if (readlen == 0) {
            return retlen;
        }
        if (readlen < 0) {
            return -1;
        }

        buf = &(buf[rest_chunk]);
        buflen -= rest_chunk;
        rest_chunk = mChunk.chunk_len;
    }

    return -1;
}

size_t im::HttpChunkReader::pipe_read(void *ptr, const size_t size, const size_t nitems) {

    const ssize_t len = size * nitems;
    const ssize_t rlen = read_next(static_cast<char *>(ptr), len);
    if (rlen > 0) {
        return rlen / size;
    }
    return rlen;
}

int im::HttpChunkReader::fgetc() {
    if ((mChunk.chunk_len - mChunk.chunk_pos) < 1) {
        if (read_next_chunk() < 1) {
            return -1;
        }
    }
    return (int) mChunk.chunk_buf[mChunk.chunk_pos++];
}

