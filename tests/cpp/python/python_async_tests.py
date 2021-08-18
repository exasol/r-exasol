import socket
import subprocess
import time


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



