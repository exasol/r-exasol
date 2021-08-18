import socket
import string
import subprocess

import socket_wrapper


def con_controller_read_test(protocol: string):
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.bind(("localhost", 5000))
    # become a server socket
    serversocket.listen(5)

    p_unit_test = subprocess.Popen(["./r_exasol", "ConnectionControllerImport" + protocol])

    (clientsocket, address) = serversocket.accept()

    recvMetaInfoRequest = clientsocket.recv(12)
    b = bytearray(b'\x00\x00\x00\x00\4\0\0\0Test\0\0\0\0\0\0\0\0\0\0\0\0')
    clientsocket.send(b)

    clientsocket = socket_wrapper.wrap(clientsocket, protocol)

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
    socket_wrapper.unwrap(clientsocket, protocol)
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


def con_controller_echo_test(protocol: string):
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    serversocket.bind(("localhost", 5000))
    # become a server socket
    serversocket.listen(5)

    p_unit_test = subprocess.Popen(["./r_exasol", "ConnectionControllerEcho" + protocol])

    (clientsocket, address) = serversocket.accept()

    #Receive Magic numbers
    recvMetaInfoRequest = clientsocket.recv(12)
    #Send back hostname (Test) and PortNumber (4)
    b = bytearray(b'\x00\x00\x00\x00\4\0\0\0Test\0\0\0\0\0\0\0\0\0\0\0\0')
    clientsocket.send(b)

    clientsocket = socket_wrapper.wrap(clientsocket, protocol)

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

    socket_wrapper.unwrap(clientsocket, protocol)

    (newclientsocket, address) = serversocket.accept()
    recvMetaInfoRequest = newclientsocket.recv(100)
    b = bytearray(b'\x00\x00\x00\x00\4\0\0\0Test\0\0\0\0\0\0\0\0\0\0\0\0')
    newclientsocket.send(b)

    newclientsocket = socket_wrapper.wrap(newclientsocket, protocol)
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

    socket_wrapper.unwrap(newclientsocket, protocol)
    p_unit_test.wait()
    try:
        clientsocket.shutdown(socket.SHUT_RDWR)
        newclientsocket.close()
    except socket.error:
        pass # Under MacOS shutdown can throw an exception if the client has closed the socket already
    assert p_unit_test.returncode == 0
