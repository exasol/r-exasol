import socket
import subprocess

print("Hello")
serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
serversocket.bind(("localhost", 5000))
# become a server socket
serversocket.listen(5)

print("Start r-exasol")
subprocess.Popen(["./r_exasol"])

print("accept socket")
(clientsocket, address) = serversocket.accept()

data = 'CHUNK DATA;' * 20
b = bytearray(f'{hex(len(data))}\n', 'UTF-8')
clientsocket.send(b)
d = bytearray(data, 'UTF-8')
d.append(0)
d.append(0)
clientsocket.send(d)

#Send zer termination
b = bytearray(f'{0}\n', 'UTF-8')
clientsocket.send(b)

recvmsg = clientsocket.recv(100)
print(f"Received:{recvmsg}")

assert recvmsg == b'HTTP/1.1 200 OK\r\nServer: EXASolution R Package\r\nConnection: close\r\n\r\n'