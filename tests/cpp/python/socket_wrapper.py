import socket
import ssl
import string


def wrap(s: socket, protocol: string):
    ret_val = s

    if protocol == "Https":
        context = ssl.SSLContext()
        context.verify_mode = ssl.CERT_NONE
        ret_val = context.wrap_socket(s, do_handshake_on_connect=False)
    return ret_val


def unwrap(s: socket, protocol: string):
    if protocol == "Https":
        s.unwrap()
