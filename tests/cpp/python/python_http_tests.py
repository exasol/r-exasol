import socket
import string
import subprocess

import socket_wrapper


def reading_test(protocol: string):
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.bind(("localhost", 5000))
    # become a server socket
    serversocket.listen(5)

    p_unit_test = subprocess.Popen(["./r_exasol_tests", "Import" + protocol])

    (clientsocket, address) = serversocket.accept()
    clientsocket = socket_wrapper.wrap(clientsocket, protocol)

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
    socket_wrapper.unwrap(clientsocket, protocol)
    p_unit_test.wait()
    try:
        clientsocket.shutdown(socket.SHUT_RDWR)
    except socket.error:
        pass  # Under MacOS shutdown can throw an exception if the client has closed the socket already
    assert p_unit_test.returncode == 0
    serversocket.close()


def reading_test_big(protocol: string):
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.bind(("localhost", 5000))
    # become a server socket
    serversocket.listen(5)

    p_unit_test = subprocess.Popen(["./r_exasol_tests", "ImportBig" + protocol])

    (clientsocket, address) = serversocket.accept()
    clientsocket = socket_wrapper.wrap(clientsocket, protocol)

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
    socket_wrapper.unwrap(clientsocket, protocol)
    p_unit_test.wait()
    try:
        clientsocket.shutdown(socket.SHUT_RDWR)
    except socket.error:
        pass  # Under MacOS shutdown can throw an exception if the client has closed the socket already
    assert p_unit_test.returncode == 0
    serversocket.close()


def writing_test(protocol: string):
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.bind(("localhost", 5000))
    # become a server socket
    serversocket.listen(5)

    p_unit_test = subprocess.Popen(["./r_exasol_tests", "Export" + protocol])

    (clientsocket, address) = serversocket.accept()
    clientsocket = socket_wrapper.wrap(clientsocket, protocol)

    header = b'HTTP/1.1 200 OK\r\nServer: EXASolution R Package\r\nContent-type: application/octet-stream\r\nContent-disposition: attachment; filename=data.csv\r\nConnection: close\r\n\r\n'
    recvmsg = clientsocket.recv(len(header))

    assert recvmsg == header

    data = 'CHUNK DATA;' * 20
    recvmsg = clientsocket.recv(len(data))
    assert data.encode('UTF-8') == recvmsg
    socket_wrapper.unwrap(clientsocket, protocol)
    p_unit_test.wait()
    try:
        clientsocket.shutdown(socket.SHUT_RDWR)
    except socket.error:
        pass  # Under MacOS shutdown can throw an exception if the client has closed the socket already
    assert p_unit_test.returncode == 0
    serversocket.close()
