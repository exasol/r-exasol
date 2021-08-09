#include <r_exasol/connection/protocol/http/reader/http_chunk_reader.h>
#include <cstring>
#include <r_exasol/connection/protocol/common.h>
#include <iostream>
#include <r_exasol/connection/connection_exception.h>
#include <utility>

namespace re = exa::reader;

namespace exa {
    namespace reader {
    class ConnectionFinished : public std::exception {};
    }
}

re::HttpChunkReader::HttpChunkReader(std::weak_ptr<Socket> socket, Chunk &chunk)
        : mSocket(std::move(socket)), mChunk(chunk) {
    mChunk.reset();
}

ssize_t re::HttpChunkReader::read_next_chunk() {
    size_t pos = 0;
    int buflen, rc;
    static const char *ok_answer =
            "HTTP/1.1 200 OK\r\n"
            "Server: EXASolution R Package\r\n"
            "Connection: close\r\n\r\n";
    ssize_t retVal = -1;
    try {
        auto socket = mSocket.lock();
        if (!socket) {
            throw ConnectionException("socket invalid");
        }

        for (pos = 0; pos < 20; pos++) {
            mChunk.chunk_buf[pos] = mChunk.chunk_buf[pos + 1] = '\0';
            if ((rc = socket->recv(&(mChunk.chunk_buf[pos]), 1)) < 1) {
                //Chunk reader might try to read from socket after stream has fnished.
                //We should not treat as error, but jump to end and return -1.
                throw ConnectionFinished();
            }
            if (mChunk.chunk_buf[pos] == '\n') {
                break;
            }
        }

        if (pos > 19) {
            throw exa::ConnectionException("buffer length exceed size");
        }

        mChunk.chunk_buf[pos] = '\0';
        buflen = -1;

        if (::sscanf(mChunk.chunk_buf, "%x", &buflen) < 1) {
            throw exa::ConnectionException("invalid buffer size provided");
        }

        if (0 == buflen) {
            socket->send(ok_answer, strlen(ok_answer));
            socket->shutdownWr();
            retVal = 0;
        } else {
            if ((buflen + 2) > MAX_HTTP_CHUNK_SIZE) {
                throw exa::ConnectionException("Buffer received larger than max chunk size.");
            }

            buflen = socket->recv(mChunk.chunk_buf, buflen + 2);
            if (buflen < 3) {
                throw exa::ConnectionException("invalid buffer length");
            }

            mChunk.chunk_len = buflen - 2;
            mChunk.chunk_pos = 0;
            mChunk.chunk_buf[buflen - 2] = '\0';
            mChunk.chunk_num++;
            retVal = mChunk.chunk_len;
        }
    } catch (const ConnectionFinished & ex) {
        closeSocketWithError();
    } catch (const exa::ConnectionException &ex) {
        std::cerr << "Error occurred in HttpChunkReader:" << ex.what() << std::endl;
        closeSocketWithError();
    }
    return retVal;
}

ssize_t re::HttpChunkReader::read_next(char *buffer, size_t buflen) {

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

size_t re::HttpChunkReader::pipe_read(void *ptr, const size_t size, const size_t nitems) {

    const ssize_t len = size * nitems;
    const ssize_t rlen = read_next(static_cast<char *>(ptr), len);
    if (rlen > 0) {
        return rlen / size;
    }
    /*
     * r_len is of type ssize_t and might return -1, while this function returns a size_t (which is unsigned)
     * This is expected behavior as the Rextension function will cast the size_t back to an integer for error checking
     * @see https://github.com/wch/r-source/blob/15c308bc93d4bed2803088ecf93a65e551da4c25/src/main/connections.c#L294
     */
    return rlen;
}

int re::HttpChunkReader::fgetc() {
    if ((mChunk.chunk_len - mChunk.chunk_pos) < 1) {
        if (read_next_chunk() < 1) {
            return -1;
        }
    }
    return (int) mChunk.chunk_buf[mChunk.chunk_pos++];
}

void exa::reader::HttpChunkReader::start() {
    auto socket = mSocket.lock();
    if (socket) {
        exa::readHttpHeader(*socket);
    } else {
        throw exa::ConnectionException("socket invalid");
    }
}

void exa::reader::HttpChunkReader::closeSocketWithError() {
    static const char *error_answer =
            "HTTP/1.1 404 ERROR\r\n"
            "Server: EXASolution R Package\r\n"
            "Connection: close\r\n\r\n";
    auto socket = mSocket.lock();
    if (socket) {
        socket->send(error_answer, strlen(error_answer));
        socket->shutdownWr();
    }
}

