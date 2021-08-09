import os
import socket
import subprocess

import http.server

import time


def reading_test():
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.bind(("localhost", 5000))
    # become a server socket
    serversocket.listen(5)

    p_unit_test = subprocess.Popen(["./r_exasol", "Import"])

    (clientsocket, address) = serversocket.accept()

    data = 'CHUNK DATA;' * 20
    b = bytearray(f'{hex(len(data))}\n', 'UTF-8')
    clientsocket.send(b)
    d = bytearray(data, 'UTF-8')
    d.append(0)
    d.append(0)
    clientsocket.send(d)

    # Send zer termination
    b = bytearray(f'{0}\n', 'UTF-8')
    clientsocket.send(b)

    recvmsg = clientsocket.recv(100)

    assert recvmsg == b'HTTP/1.1 200 OK\r\nServer: EXASolution R Package\r\nConnection: close\r\n\r\n'
    p_unit_test.wait()
    try:
        clientsocket.shutdown(socket.SHUT_RDWR)
    except socket.error:
        pass # Under MacOS shutdown can throw an exception if the client has closed the socket already
    assert p_unit_test.returncode == 0


def reading_test_big():
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.bind(("localhost", 5000))
    # become a server socket
    serversocket.listen(5)

    p_unit_test = subprocess.Popen(["./r_exasol", "ImportBig"])

    (clientsocket, address) = serversocket.accept()

    for idxChunk in range(1000):
        data = 'CHUNK DATA;' * 20000
        b = bytearray(f'{hex(len(data))}\n', 'UTF-8')
        clientsocket.send(b)
        d = bytearray(data, 'UTF-8')
        d.append(0)
        d.append(0)
        clientsocket.send(d)

    # Send zer termination
    b = bytearray(f'{0}\n', 'UTF-8')
    clientsocket.send(b)

    recvmsg = clientsocket.recv(100)

    assert recvmsg == b'HTTP/1.1 200 OK\r\nServer: EXASolution R Package\r\nConnection: close\r\n\r\n'
    p_unit_test.wait()
    try:
        clientsocket.shutdown(socket.SHUT_RDWR)
    except socket.error:
        pass # Under MacOS shutdown can throw an exception if the client has closed the socket already
    assert p_unit_test.returncode == 0


def writing_test():
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.bind(("localhost", 5000))
    # become a server socket
    serversocket.listen(5)

    p_unit_test = subprocess.Popen(["./r_exasol", "Export"])

    (clientsocket, address) = serversocket.accept()

    header = b'HTTP/1.1 200 OK\r\nServer: EXASolution R Package\r\nContent-type: application/octet-stream\r\nContent-disposition: attachment; filename=data.csv\r\nConnection: close\r\n\r\n'
    recvmsg = clientsocket.recv(len(header))

    assert recvmsg == header

    data = 'CHUNK DATA;' * 20
    recvmsg = clientsocket.recv(len(data))
    assert data.encode('UTF-8') == recvmsg

    p_unit_test.wait()
    try:
        clientsocket.shutdown(socket.SHUT_RDWR)
    except socket.error:
        pass # Under MacOS shutdown can throw an exception if the client has closed the socket already
    assert p_unit_test.returncode == 0


def reading_http_test():
    class handler(http.server.BaseHTTPRequestHandler):
        def handle_one_request(self):
            global done
            print("got request")
            self.requestline = "test"
            self.request_version = "1.1"
            self.send_response(200)
            self.send_header('Content-type', 'text/plain')
            self.end_headers()

            data = 'CHUNK DATA;' * 20
            b = bytearray(f'{hex(len(data))}\n', 'UTF-8')
            self.wfile.write(b)
            d = bytearray(data, 'UTF-8')
            d.append(0)
            d.append(0)
            self.wfile.write(d)

            # Send zer termination
            b = bytearray(f'{0}\n', 'UTF-8')
            self.wfile.write(b)

            done = True

    address = ('localhost', 5000)
    httpd = http.server.HTTPServer(address, handler)
    p_unit_test = subprocess.Popen(["./r_exasol", "ImportHttp"])
    httpd.handle_request()
    p_unit_test.wait()
    assert p_unit_test.returncode == 0


def con_controller_read_test():
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.bind(("localhost", 5000))
    # become a server socket
    serversocket.listen(5)

    p_unit_test = subprocess.Popen(["./r_exasol", "ConnectionControllerImport"])

    (clientsocket, address) = serversocket.accept()

    recvMetaInfoRequest = clientsocket.recv(12)
    b = bytearray(b'\x00\x00\x00\x00\4\0\0\0Test\0\0\0\0\0\0\0\0\0\0\0\0')
    clientsocket.send(b)
    b = bytearray(b"\r\n")  # empty header
    clientsocket.send(b)
    data = 'CHUNK DATA;' * 20
    b = bytearray(f'{hex(len(data))}\n', 'UTF-8')
    clientsocket.send(b)
    d = bytearray(data, 'UTF-8')
    d.append(0)
    d.append(0)
    clientsocket.send(d)

    # Send zer termination
    b = bytearray(f'{0}\n', 'UTF-8')
    clientsocket.send(b)

    recvmsg = clientsocket.recv(100)

    assert recvmsg == b'HTTP/1.1 200 OK\r\nServer: EXASolution R Package\r\nConnection: close\r\n\r\n'
    p_unit_test.wait()
    try:
        clientsocket.shutdown(socket.SHUT_RDWR)
    except socket.error:
        pass # Under MacOS shutdown can throw an exception if the client has closed the socket already
    assert p_unit_test.returncode == 0


def con_controller_read_test_with_error():
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.bind(("localhost", 5000))
    # become a server socket
    serversocket.listen(5)

    p_unit_test = subprocess.Popen(["./r_exasol", "ConnectionControllerImportWithError"])

    (clientsocket, address) = serversocket.accept()

    recvMetaInfoRequest = clientsocket.recv(12)

    # Simulate connection abort by close socket
    clientsocket.shutdown(socket.SHUT_RDWR)

    # Next attempt
    (newClientsocket, address) = serversocket.accept()

    recvMetaInfoRequest = newClientsocket.recv(12)
    b = bytearray(b'\x00\x00\x00\x00\4\0\0\0Test\0\0\0\0\0\0\0\0\0\0\0\0')
    newClientsocket.send(b)
    b = bytearray(b"\r\n")  # empty header
    newClientsocket.send(b)
    data = 'CHUNK DATA;' * 20
    b = bytearray(f'{hex(len(data))}\n', 'UTF-8')
    newClientsocket.send(b)
    d = bytearray(data, 'UTF-8')
    d.append(0)
    d.append(0)
    newClientsocket.send(d)

    # Send zer termination
    b = bytearray(f'{0}\n', 'UTF-8')
    newClientsocket.send(b)

    recvmsg = newClientsocket.recv(100)

    assert recvmsg == b'HTTP/1.1 200 OK\r\nServer: EXASolution R Package\r\nConnection: close\r\n\r\n'
    p_unit_test.wait()
    try:
        clientsocket.shutdown(socket.SHUT_RDWR)
    except socket.error:
        pass # Under MacOS shutdown can throw an exception if the client has closed the socket already
    assert p_unit_test.returncode == 0


def con_controller_echo_test():
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.bind(("localhost", 5000))
    # become a server socket
    serversocket.listen(5)

    p_unit_test = subprocess.Popen(["./r_exasol", "ConnectionControllerEcho"])

    (clientsocket, address) = serversocket.accept()

    recvMetaInfoRequest = clientsocket.recv(12)
    b = bytearray(b'\x00\x00\x00\x00\4\0\0\0Test\0\0\0\0\0\0\0\0\0\0\0\0')
    clientsocket.send(b)
    b = bytearray(b"\r\n")  # empty header
    clientsocket.send(b)
    ok_answer = "HTTP/1.1 200 OK\r\n" \
                "Server: EXASolution R Package\r\n" \
                "Content-type: application/octet-stream\r\n" \
                "Content-disposition: attachment; filename=data.csv\r\n" \
                "Connection: close\r\n\r\n"
    header_from_client = clientsocket.recv(len(ok_answer))
    assert header_from_client == ok_answer.encode("UTF-8")
    data = clientsocket.recv(3)
    assert data == "\"a\"".encode("UTF-8")
    data = clientsocket.recv(1)
    assert data == "\n".encode("UTF-8")
    data = clientsocket.recv(3)
    assert data == "\"b\"".encode("UTF-8")
    data = clientsocket.recv(1)
    assert data == "\n".encode("UTF-8")

    (newclientsocket, address) = serversocket.accept()
    recvMetaInfoRequest = newclientsocket.recv(100)
    b = bytearray(b'\x00\x00\x00\x00\4\0\0\0Test\0\0\0\0\0\0\0\0\0\0\0\0')
    newclientsocket.send(b)
    b = bytearray(b"\r\n")  # empty header
    newclientsocket.send(b)
    data = "Name\na\nb"
    b = bytearray(f'{hex(len(data))}\n', 'UTF-8')
    newclientsocket.send(b)
    d = bytearray(data, 'UTF-8')
    d.append(0)
    d.append(0)
    newclientsocket.send(d)

    # Send zer termination
    b = bytearray(f'{0}\n', 'UTF-8')
    newclientsocket.send(b)

    recvmsg = newclientsocket.recv(100)
    assert recvmsg == b'HTTP/1.1 200 OK\r\nServer: EXASolution R Package\r\nConnection: close\r\n\r\n'
    p_unit_test.wait()
    try:
        clientsocket.shutdown(socket.SHUT_RDWR)
        newclientsocket.close()
    except socket.error:
        pass # Under MacOS shutdown can throw an exception if the client has closed the socket already
    assert p_unit_test.returncode == 0


def reading_async_test():
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.bind(("localhost", 5000))
    # become a server socket
    serversocket.listen(5)

    p_unit_test = subprocess.Popen(["./r_exasol", "ReaderCloseConnection"],
                                   stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.STDOUT)

    (clientsocket, address) = serversocket.accept()

    recvMetaInfoRequest = clientsocket.recv(12)
    b = bytearray(b'\x00\x00\x00\x00\4\0\0\0Test\0\0\0\0\0\0\0\0\0\0\0\0')
    clientsocket.send(b)
    b = bytearray(b"\r\n")  # empty header
    clientsocket.send(b)
    data = 'CHUNK DATA;' * 20
    b = bytearray(f'{hex(len(data))}\n', 'UTF-8')
    clientsocket.send(b)
    d = bytearray(data, 'UTF-8')
    d.append(0)
    d.append(0)
    clientsocket.send(d)

    # Send zer termination
    b = bytearray(f'{0}\n', 'UTF-8')
    clientsocket.send(b)

    recvmsg = clientsocket.recv(100)

    assert recvmsg == b'HTTP/1.1 200 OK\r\nServer: EXASolution R Package\r\nConnection: close\r\n\r\n'
    result = p_unit_test.communicate(input=b'FINISHED')[0]
    p_unit_test.wait()
    try:
        clientsocket.shutdown(socket.SHUT_RDWR)
    except socket.error:
        pass # Under MacOS shutdown can throw an exception if the client has closed the socket already
    assert p_unit_test.returncode == 0

def reading_async_test_abort():
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.bind(("localhost", 5000))
    # become a server socket
    serversocket.listen(5)

    p_unit_test = subprocess.Popen(["./r_exasol", "ReaderCloseConnectionAbort"],
                                   stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.STDOUT)

    (clientsocket, address) = serversocket.accept()

    recvMetaInfoRequest = clientsocket.recv(12)
    b = bytearray(b'\x00\x00\x00\x00\4\0\0\0Test\0\0\0\0\0\0\0\0\0\0\0\0')
    clientsocket.send(b)
    b = bytearray(b"\r\n")  # empty header
    clientsocket.send(b)
    data = 'CHUNK DATA;' * 20
    b = bytearray(f'{hex(len(data))}\n', 'UTF-8')
    clientsocket.send(b)
    d = bytearray(data, 'UTF-8')
    d.append(0)
    d.append(0)
    clientsocket.send(d)

    # Send zer termination
    b = bytearray(f'{0}\n', 'UTF-8')
    clientsocket.send(b)
    time.sleep(0.1)
    result = p_unit_test.communicate(input=b'ABORT')[0]
    print(f'stdout={p_unit_test.stdout}')
    print(f'result={result}')
    p_unit_test.wait()
    try:
        clientsocket.shutdown(socket.SHUT_RDWR)
    except socket.error:
        pass # Under MacOS shutdown can throw an exception if the client has closed the socket already
    assert p_unit_test.returncode == 0


def writing_async_test():
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.bind(("localhost", 5000))
    # become a server socket
    serversocket.listen(5)

    p_unit_test = subprocess.Popen(["./r_exasol", "WriterCloseConnection"],
                                   stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.STDOUT)

    (clientsocket, address) = serversocket.accept()

    recvMetaInfoRequest = clientsocket.recv(12)
    b = bytearray(b'\x00\x00\x00\x00\4\0\0\0Test\0\0\0\0\0\0\0\0\0\0\0\0')
    clientsocket.send(b)
    b = bytearray(b"\r\n")  # empty header
    clientsocket.send(b)
    ok_answer = "HTTP/1.1 200 OK\r\n" \
                "Server: EXASolution R Package\r\n" \
                "Content-type: application/octet-stream\r\n" \
                "Content-disposition: attachment; filename=data.csv\r\n" \
                "Connection: close\r\n\r\n"
    header_from_client = clientsocket.recv(len(ok_answer))
    assert header_from_client == ok_answer.encode("UTF-8")
    data = clientsocket.recv(3)
    assert data == "\"a\"".encode("UTF-8")
    data = clientsocket.recv(1)
    assert data == "\n".encode("UTF-8")
    data = clientsocket.recv(3)
    assert data == "\"b\"".encode("UTF-8")
    data = clientsocket.recv(1)
    assert data == "\n".encode("UTF-8")

    result = p_unit_test.communicate(input=b'FINISHED')[0]
    p_unit_test.wait()
    try:
        clientsocket.shutdown(socket.SHUT_RDWR)
    except socket.error:
        pass # Under MacOS shutdown can throw an exception if the client has closed the socket already
    assert p_unit_test.returncode == 0


def writing_async_test_abort():
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.bind(("localhost", 5000))
    # become a server socket
    serversocket.listen(5)

    p_unit_test = subprocess.Popen(["./r_exasol", "WriterCloseConnectionAbort"],
                                   stdout=subprocess.PIPE, stdin=subprocess.PIPE, stderr=subprocess.STDOUT)

    (clientsocket, address) = serversocket.accept()

    recvMetaInfoRequest = clientsocket.recv(12)
    b = bytearray(b'\x00\x00\x00\x00\4\0\0\0Test\0\0\0\0\0\0\0\0\0\0\0\0')
    clientsocket.send(b)
    b = bytearray(b"\r\n")  # empty header
    clientsocket.send(b)
    ok_answer = "HTTP/1.1 200 OK\r\n" \
                "Server: EXASolution R Package\r\n" \
                "Content-type: application/octet-stream\r\n" \
                "Content-disposition: attachment; filename=data.csv\r\n" \
                "Connection: close\r\n\r\n"
    header_from_client = clientsocket.recv(len(ok_answer))
    assert header_from_client == ok_answer.encode("UTF-8")
    data = clientsocket.recv(3)
    assert data == "\"a\"".encode("UTF-8")
    data = clientsocket.recv(1)
    assert data == "\n".encode("UTF-8")
    data = clientsocket.recv(3)
    assert data == "\"b\"".encode("UTF-8")
    result = p_unit_test.communicate(input=b'ABORT')[0]

    p_unit_test.wait()
    try:
        clientsocket.shutdown(socket.SHUT_RDWR)
    except socket.error:
        pass # Under MacOS shutdown can throw an exception if the client has closed the socket already
    assert p_unit_test.returncode == 0


def algo_tests():
    p_unit_test = subprocess.Popen(["./r_exasol", "[algo]"])
    p_unit_test.wait()
    assert p_unit_test.returncode == 0


if __name__ == "__main__":
    reading_test()
    writing_test()
    reading_test_big()
    reading_http_test()
    con_controller_read_test()
    con_controller_echo_test()
    reading_async_test()
    reading_async_test_abort()
    writing_async_test()
    writing_async_test_abort()
    con_controller_read_test_with_error()
    algo_tests()
