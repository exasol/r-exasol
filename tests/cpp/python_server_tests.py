import socket
import subprocess

import http.server


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
    clientsocket.close()
    serversocket.close()


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
    clientsocket.close()
    serversocket.close()

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


if __name__ == "__main__":
    reading_test()
    writing_test()
    reading_http_test()
