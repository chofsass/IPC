import socket
import sys
import os

SERVER_ADDRESS = "server.sock"
CLIENT_ADDRESS = "client.sock"

if __name__ == "__main__":
    sock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
    os.unlink(CLIENT_ADDRESS)
    sock.bind(CLIENT_ADDRESS)

    message = b'This is the message.  It will be repeated.'

    try:

        # Send data
        print('sending {!r}'.format(message))
        sent = sock.sendto(message, SERVER_ADDRESS)

        # Receive response
        print('waiting to receive')
        sock.settimeout(1)
        data, server = sock.recvfrom(512)
        print('received {!r}'.format(data))

    finally:
        print('closing socket')
        sock.close()