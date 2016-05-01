#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <pthread.h>

#ifndef _WIN32
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dlfcn.h>
#include <arpa/inet.h>
#include <errno.h>
#else
#include <winsock2.h>
#include <windows.h>
#endif

#include <sql.h>

#ifdef ERROR
#undef ERROR
#endif
#include <R.h>
#include <Rdefines.h>
#include <R_ext/Connections.h>

#define MAX_RODBC_THREADS 1
#define MAX_HTTP_HEADER_LEN 16384
#define MAX_HTTP_CHUNK_SIZE 524288

#ifdef _WIN32
#ifndef SHUT_RDWR
#define SHUT_RDWR SD_BOTH
#endif
#endif

#ifdef _WIN32
# define f_seek fseeko64
# define f_tell ftello64
# define OFF_T off64_t
#elif defined(HAVE_OFF_T) && defined(HAVE_FSEEKO)
# define f_seek fseeko
# define f_tell ftello
# define OFF_T off_t
#else
# define f_seek fseek
# define f_tell ftell
# define OFF_T long
#endif

extern void Rf_set_iconv(Rconnection con);
extern int dummy_fgetc(Rconnection con);

typedef struct rodbcHandle {
    SQLHDBC	    hDbc;      /* connection handle */
    SQLHSTMT	  hStmt;     /* statement handle */
    SQLLEN	    nRows;     /* number of rows and columns in result set */
    SQLSMALLINT	nColumns;
    int channel;      /* as stored on the R-level object */
    int id;           /* ditto */
    int useNRows;     /* value of believeNRows */
    /* entries used to bind data for result sets and updates */
    void	*ColData;	  /* this will be allocated as an array */
    int		nAllocated; /* how many cols were allocated */
    SQLUINTEGER	rowsFetched;	/* use to indicate the number of rows fetched */
    SQLUINTEGER	rowArraySize;	/* use to indicate the number of rows we expect back */
    SQLUINTEGER	rowsUsed;	/* for when we fetch more than we need */
    void	*msglist;	/* root of linked list of messages */
    SEXP  extPtr;		/* address of external pointer for this
                       channel, so we can clear it */
} RODBCHandle, *pRODBCHandle;

typedef struct {
    pRODBCHandle rodbc;
    SQLHSTMT stmt;
    SQLRETURN res;
    SQLCHAR *query;
    volatile char done;
    char *error;
#ifndef _WIN32
    int fd;
#else
    SOCKET fd;
#endif
    char proxyHost[16];
    int proxyPort;
    char asyncThread_started;
    Rconnection conn;
    pthread_t asyncThread;
    char chunk_buf[MAX_HTTP_CHUNK_SIZE];
    size_t chunk_len, chunk_pos, chunk_num;
} asyncODBC_t;

static asyncODBC_t asyncODBC[MAX_RODBC_THREADS];

static inline ssize_t read_next_chunk(asyncODBC_t *t) {
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
        t->chunk_buf[pos] = t->chunk_buf[pos + 1] = '\0';
        if ((rc = recv(t->fd, &(t->chunk_buf[pos]), 1, MSG_WAITALL)) < 1) {
            // fprintf(stderr, "### error (%d)\n", rc);
            goto error;
        }
        if (t->chunk_buf[pos] == '\n') {
          break;
        }
    }

    if (pos > 19) {
      goto error;
    }

    t->chunk_buf[pos] = '\0';
    buflen = -1;

    if (sscanf(t->chunk_buf, "%x", &buflen) < 1) {
      goto error;
    }

    if (buflen == 0) {
        send(t->fd, ok_answer, strlen(ok_answer), 0);
        shutdown(t->fd, 1);
        t->fd = -1;
        return 0;
    }

    if ((buflen + 2) > MAX_HTTP_CHUNK_SIZE) {
      goto error;
    }

    buflen = recv(t->fd, t->chunk_buf, buflen + 2, MSG_WAITALL);
    if (buflen < 3) {
      goto error;
    }

    t->chunk_len = buflen - 2;
    t->chunk_pos = 0;
    t->chunk_buf[buflen-2] = '\0';
    t->chunk_num ++;
    return t->chunk_len;

error:
    send(t->fd, error_answer, strlen(error_answer), 0);
    shutdown(t->fd, 1);
    t->fd = -1;
    return -1;
}

static inline ssize_t write_next_chunk(asyncODBC_t *t) {
    const char *ok_answer =
        "HTTP/1.1 200 OK\r\n"
        "Server: EXASolution R Package\r\n"
        "Content-type: application/octet-stream\r\n"
        "Content-disposition: attachment; filename=data.csv\r\n"
        "Connection: close\r\n\r\n";

    const size_t ok_len = strlen(ok_answer);
    const size_t chunk_len = t->chunk_len;
    const char *error_answer =
        "HTTP/1.1 404 ERROR\r\n"
        "Server: EXASolution R Package\r\n"
        "Connection: close\r\n\r\n";

    //fprintf(stderr, "### write_next_chunk %d (%d)\n", t->chunk_len, t->chunk_num);
    if (chunk_len == 0) {
      goto error;
    }

    if (t->chunk_num == 0) {
      if (send(t->fd, ok_answer, ok_len, 0) != ok_len) {
        goto error;
      }
    }

    if (send(t->fd, t->chunk_buf, chunk_len, 0) != chunk_len) {
      goto error;
    }

    t->chunk_len = 0;
    t->chunk_num ++;
    return chunk_len;

error:
    send(t->fd, error_answer, strlen(error_answer), 0);
    shutdown(t->fd, SHUT_RDWR);
    t->fd = -1;
    return -1;
}

static inline ssize_t read_next(asyncODBC_t *t, char *buffer, size_t buflen) {

    size_t rest_chunk = t->chunk_len - t->chunk_pos;
    ssize_t readlen = 0, retlen = 0;
    char *buf = buffer;

    for (;;) {
        if (buflen <= rest_chunk) {
            memcpy(buf, &(t->chunk_buf[t->chunk_pos]), buflen);
            t->chunk_pos += buflen;
            retlen += buflen;
            return retlen;
        }

        memcpy(buf, &(t->chunk_buf[t->chunk_pos]), rest_chunk);
        retlen += rest_chunk;

        readlen = read_next_chunk(t);
        if (readlen == 0) {
          return retlen;
        }
        if (readlen < 0) {
          return -1;
        }

        buf = &(buf[rest_chunk]);
        buflen -= rest_chunk;
        rest_chunk = t->chunk_len;
    }

    return -1;
}

static size_t pipe_read(void *ptr, const size_t size, const size_t nitems,
                        const Rconnection con) {

    asyncODBC_t *t = *((asyncODBC_t**) con->private);
    const ssize_t len = size * nitems;
    const ssize_t rlen = read_next(t, ptr, len);
    if (rlen > 0) {
      return rlen / size;
    }
    return rlen;
}

static int file_fgetc_internal(const Rconnection con) {
    asyncODBC_t *t = *((asyncODBC_t**) con->private);
    if ((t->chunk_len - t->chunk_pos) < 1) {
        if (read_next_chunk(t) < 1) {
          return -1;
        }
    }
    return (int) t->chunk_buf[t->chunk_pos++];
}

extern int dummy_vfprintf(Rconnection con, const char *format, va_list ap);

static int pipe_vfprintf(const Rconnection con, const char *format, va_list ap) {
    return dummy_vfprintf(con, format, ap);
}

static size_t pipe_write(const void *ptr, size_t size, size_t nitems,
                         const Rconnection con) {

    asyncODBC_t *t = *((asyncODBC_t**) con->private);
    char *src = (char*) ptr;
    size_t cur_rest = MAX_HTTP_CHUNK_SIZE - t->chunk_len;
    size_t len = size * nitems;

    //fprintf(stderr, "### pipe_write %d: %d/%d\n", len, t->chunk_len, t->chunk_num);
    for (;;) {
        if (cur_rest >= len) {
            memcpy(&(t->chunk_buf[t->chunk_len]), src, len);
            t->chunk_len += len;
            return len;
        }

        if (cur_rest > 0) {
            memcpy(&(t->chunk_buf[t->chunk_len]), src, cur_rest);
            t->chunk_len += cur_rest;
            src = &(src[cur_rest]);
            len = len - cur_rest;
        }

        if (write_next_chunk(t) < 0)
            return -1;

        cur_rest = MAX_HTTP_CHUNK_SIZE - t->chunk_len;
    }
    return -1;
}

static int pipe_fflush(Rconnection con) {
    asyncODBC_t *t = *((asyncODBC_t**) con->private);
    if (t->chunk_len > 0) {
        if (write_next_chunk(t) < 0)
            return -1;
        else return 0;
    }
    return 0;
}

static void *asyncRODBCQueryExecuter(void *arg) {
    int slot = *(int*) arg;
    asyncODBC_t *t = &(asyncODBC[slot]);

    t->res = SQLExecDirect(t->stmt, t->query, SQL_NTS);
    t->done = 1;
    if (t->res != SQL_SUCCESS && t->res != SQL_SUCCESS_WITH_INFO) {
      shutdown(t->fd, SHUT_RDWR);
    }
    return NULL;
}

SEXP asyncRODBCIOStart(SEXP slotA, SEXP hostA, SEXP portA) {
    const int slot = asInteger(slotA);
    const int port = asInteger(portA);
    const char *host = CHAR(STRING_ELT(hostA, 0));
    asyncODBC_t *t = NULL;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    struct { int m:32; int x:32; int y:32; } proxy_header;
    struct { int v:32; int port:32; char s[16]; } proxy_answer;

    memset((char *) &proxy_header, 0, sizeof(proxy_header));
    proxy_header.m = 0x02212102;
#if __BYTE_ORDER == __BIG_ENDIAN
#ifndef _WIN32
    proxy_header.x = 0x01000000;
    proxy_header.y = 0x01000000;
#else
    proxy_header.x = 1;
    proxy_header.y = 1;
#endif
#else
#ifdef __APPLE__
    proxy_header.x = 0;
    proxy_header.y = 0;
#else
    proxy_header.x = 1;
    proxy_header.y = 1;
#endif
#endif


    if (slot < 0 || slot >= MAX_RODBC_THREADS) {
        error("Slot need to be from 0 to %d.", MAX_RODBC_THREADS);
        goto error;
    }
    t = &(asyncODBC[slot]);
    if (t->done) {
        error("Slot already done, please finish the connection.");
        goto error;
    }
    if (t->fd >= 0) {
        shutdown(t->fd, SHUT_RDWR);
        t->fd = -1;
    }

#ifdef _WIN32
    {   WSADATA wsa;
	if (WSAStartup(MAKEWORD(2,0), &wsa) != 0) {
	    error("Could not initialize WSA");
	    goto error;
	}
    }
#endif

    t->fd = socket(AF_INET, SOCK_STREAM, 0);
    if (t->fd < 0) {
        error("Could not create socket.");
        goto error;
    }

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    server = gethostbyname(host);
    if (server == NULL) {
        error("No such host: %s", host);
        goto error;
    }
    memcpy((char *)&serv_addr.sin_addr.s_addr, (char *) server->h_addr, server->h_length);

    if (connect(t->fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
#ifndef _WIN32
        error("Could not connect to %s:%d", host, port);
#else
        error("Could not connect to %s:%d (%d)", host, port, WSAGetLastError());
#endif
        goto error;
    }

    {   int r;
	if ((r = send(t->fd, (void*)&proxy_header, sizeof(proxy_header), 0)) != sizeof(proxy_header)) {
   	    error("Failed to send proxy header (%d != %d)", r, sizeof(proxy_header));
	    goto error;
        }

#ifndef _WIN32
	errno = 0;
#endif

	if ((r = recv(t->fd, (void*)&(proxy_answer), sizeof(proxy_answer), MSG_WAITALL)) != sizeof(proxy_answer)) {
#ifdef __APPLE__
	  error("Proxy header... - M = %d; x = %d; y = %d", proxy_header.m, proxy_header.x, proxy_header.y);
#endif
#ifndef _WIN32
	  error("Failed to receive proxy header from %s:%d (%d != %d); errno: %d", inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port), r, sizeof(proxy_answer), errno);
#else
	  error("Failed to receive proxy header from %s:%d (%d != %d; WS error code: %d)",  inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port), r, sizeof(proxy_answer), WSAGetLastError());
#endif
            goto error;
        }
	else {
	  REprintf("Successfully received proxy header from %s:%d (%d != %d)\n", inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port), r, sizeof(proxy_answer));
	}
    }
    proxy_answer.s[15] = '\0';
    memcpy(t->proxyHost, proxy_answer.s, 16);
    t->proxyPort = proxy_answer.port;

    return ScalarInteger(0);

error:
    if (t != NULL && t->fd >= 0) {
        shutdown(t->fd, SHUT_RDWR);
        t->fd = -1;
    }
    return ScalarInteger(-1);
}

SEXP asyncRODBCProxyHost(SEXP slotA) {
    SEXP host;
    const int slot = asInteger(slotA);
    if (slot < 0 || slot >= MAX_RODBC_THREADS) {
        error("Slot need to be from 0 to %d.", MAX_RODBC_THREADS);
        return ScalarInteger(-1);
    }

    PROTECT(host = allocVector(STRSXP, 1));
    SET_STRING_ELT(host, 0, mkChar(asyncODBC[slot].proxyHost));
    UNPROTECT(1);

    return host;
}

SEXP asyncRODBCProxyPort(SEXP slotA) {
    const int slot = asInteger(slotA);
    if (slot < 0 || slot >= MAX_RODBC_THREADS) {
        error("Slot need to be from 0 to %d.", MAX_RODBC_THREADS);
        return ScalarInteger(-1);
    }

    return ScalarInteger(asyncODBC[slot].proxyPort);
}

SEXP asyncRODBCIsDone(SEXP slotA) {
    const int slot = asInteger(slotA);
    if (slot < 0 || slot >= MAX_RODBC_THREADS) {
        error("Slot need to be from 0 to %d.", MAX_RODBC_THREADS);
        return ScalarInteger(-1);
    }
    return ScalarInteger(asyncODBC[slot].done);
}

SEXP asyncRODBCMax() {
    return ScalarInteger(MAX_RODBC_THREADS);
}

SEXP asyncRODBCQueryStart(SEXP slotA, SEXP chan, SEXP query, SEXP writerA) {
    const int slot = asInteger(slotA);
    const int writer = asInteger(writerA);
    asyncODBC_t *t = NULL;
    char line[4096], data = '\0';;
    ssize_t len = -1;
    int pos = 0;
    pRODBCHandle rodbc = R_ExternalPtrAddr(chan);
    SEXP conn;

    if (rodbc == NULL) {
        error("Could not get RODBC structure from channel");
        goto error;
    }

    if (slot < 0 || slot >= MAX_RODBC_THREADS) {
        error("Slot need to be from 0 to %d.", MAX_RODBC_THREADS);
        goto error;
    }
    t = &(asyncODBC[slot]);
    if (t->done) {
        error("Slot already done");
        return ScalarInteger(-1);
    }

    t->stmt = NULL;
    t->res = SQLAllocHandle(SQL_HANDLE_STMT, rodbc->hDbc, &t->stmt);
    if (t->res != SQL_SUCCESS && t->res != SQL_SUCCESS_WITH_INFO) {
        error("Could not allocate SQLAllocHandle");
        goto error;
    }

    t->query = (SQLCHAR *) translateChar(STRING_ELT(query, 0));
    t->done = 0;

    if (pthread_create(&t->asyncThread, NULL, asyncRODBCQueryExecuter, (void*)&slot))
    {
        error("Query thread could not be created.");
        goto error;
    }
    t->asyncThread_started = 1;

    line[0] = '\0';
    do {
        if (pos > 4094) {
            error("Could not read header, line too long.");
            goto error;
        }
        len = recv(t->fd, &data, 1, MSG_WAITALL);
        if (len != 1) {
            error("Could not read header.");
            goto error;
        }
        line[pos++] = data;
        line[pos] = '\0';
        if (data == '\n' && pos > 1 && line[pos-2] == '\r') {
	    // fprintf(stderr, "### got line: %s", line);
            if (pos == 2) {
              break; /* header finished */
            }
            pos = 0;
            line[0] = '\0';
        }
    } while(1);

    PROTECT(conn = R_new_custom_connection("exasol", writer ? "w" : "r", "textConnection", &t->conn));

    t->chunk_len = 0;
    t->chunk_pos = 0;
    t->chunk_num = 0;
    t->conn->isopen = TRUE;
    t->conn->blocking = TRUE;
    t->conn->canread = writer == 0 ? TRUE : FALSE;
    t->conn->canwrite = writer == 0 ? FALSE : TRUE;
    if (!writer) {
        t->conn->canread = TRUE;
        t->conn->canwrite = FALSE;
        t->conn->read = &pipe_read;
        t->conn->fgetc = &dummy_fgetc;
        t->conn->fgetc_internal = &file_fgetc_internal;
    } else {
        t->conn->canread = FALSE;
        t->conn->canwrite = TRUE;
        t->conn->vfprintf = &pipe_vfprintf;
        t->conn->write = &pipe_write;
        t->conn->fflush = &pipe_fflush;
    }
    t->conn->save = -1000;
    t->conn->private = (void*)malloc(sizeof(asyncODBC_t*));
    *((asyncODBC_t**)t->conn->private) = t;
    Rf_set_iconv(t->conn);

    UNPROTECT(1);
    return conn;

error:
    return ScalarInteger(-1);
}

SEXP asyncRODBCQueryCheck(SEXP slotA) {
    const int slot = asInteger(slotA);
    if (slot < 0 || slot >= MAX_RODBC_THREADS) {
        error("Slot need to be from 0 to %d.", MAX_RODBC_THREADS);
        return ScalarInteger(-1);
    }
    return ScalarInteger(asyncODBC[slot].done);
}

SEXP asyncRODBCQueryFinish(SEXP slotA, SEXP closeA) {
    int err = 0, i = 0, ret = -1, reterr = 0;
    const int slot = asInteger(slotA);
    const int closefd = asInteger(closeA);
    SQLCHAR sqlstate[6], msg[SQL_MAX_MESSAGE_LENGTH];
    SQLINTEGER NativeError;
    SQLSMALLINT MsgLen;
    asyncODBC_t *t = NULL;

    if (slot < 0 || slot >= MAX_RODBC_THREADS) {
        error("Slot need to be from 0 to %d.", MAX_RODBC_THREADS);
        return ScalarInteger(-1);
    }
    t = &(asyncODBC[slot]);

    if (t->fd >= 0) {
        shutdown(t->fd, SHUT_RDWR);
        t->fd = -1;
    }

    if (t->asyncThread_started && pthread_join(t->asyncThread, NULL)) {
      ++err;
    }

    if (t->done) {
      ret = 0;
    }
    t->done = 0;

    if (t->res != SQL_SUCCESS && t->res != SQL_SUCCESS_WITH_INFO) {
        t->res =  SQLGetDiagRec(SQL_HANDLE_STMT,
                                t->stmt, 1,
                                sqlstate, &NativeError, msg, sizeof(msg),
                                &MsgLen);
        if (t->res != SQL_SUCCESS && t->res != SQL_SUCCESS_WITH_INFO) {
            error("Unknown ODBC error");
        } else {
          reterr = 1;
        }
    }

    if (t->stmt != NULL) {
      (void)SQLFreeHandle(SQL_HANDLE_STMT, t->stmt);
    }

    t->conn = NULL;
    t->stmt = NULL;
    t->asyncThread_started = 0;

    if (reterr) {
      error("%s %d %s", sqlstate, (int)NativeError, msg);
    } else if (!closefd) {
        if (err > 1) error("Could not finish threads.");
        if (err > 0) error("Could not finish thread.");
        if (ret) warning("Slot %d was not done jet.", slot);
    }
    return ScalarInteger(ret);
}

#include <R_ext/Rdynload.h>

R_CallMethodDef CallEntries[] = {
    {"asyncRODBCIOStart", (DL_FUNC) &asyncRODBCIOStart, 3},
    {"asyncRODBCProxyHost", (DL_FUNC) &asyncRODBCProxyHost, 1},
    {"asyncRODBCProxyPort", (DL_FUNC) &asyncRODBCProxyPort, 1},
    {"asyncRODBCIsDone", (DL_FUNC) &asyncRODBCIsDone, 1},
    {"asyncRODBCMax", (DL_FUNC) &asyncRODBCMax, 0},
    {"asyncRODBCQueryStart", (DL_FUNC) &asyncRODBCQueryStart, 4},
    {"asyncRODBCQueryCheck", (DL_FUNC) &asyncRODBCQueryCheck, 1},
    {"asyncRODBCQueryFinish", (DL_FUNC) &asyncRODBCQueryFinish, 2},
    {NULL, NULL, 0}
};

void R_init_exasol(DllInfo *dll)
{
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
    R_forceSymbols(dll, TRUE);

    for (int i = 0; i < MAX_RODBC_THREADS; ++i) {
        asyncODBC[i].rodbc = NULL;
        asyncODBC[i].stmt = NULL;
        asyncODBC[i].done = 0;
        asyncODBC[i].asyncThread_started = 0;
        asyncODBC[i].fd = -1;
        asyncODBC[i].conn = NULL;
        asyncODBC[i].chunk_len = 0;
        asyncODBC[i].chunk_pos = 0;
        asyncODBC[i].chunk_num = 0;
    }
}
