import socket
import sys
import os
import json

SERVER_ADDRESS = "server.sock"
CLIENT_ADDRESS = "client.sock"

if __name__ == "__main__":
    try:
        with open(CLIENT_ADDRESS, 'w+') as fp:
            pass
    except:
        pass

    sock = socket.socket(socket.AF_UNIX, socket.SOCK_DGRAM)
    os.unlink(CLIENT_ADDRESS)
    sock.bind(CLIENT_ADDRESS)

    message = json.dumps({
        "special_key_id": "MSG1",
        "boolField": True,
        "intField": 1,
        "floatField": 1.23,
        "stringField": "string data",
        "msg2List": [
            {
                "boolField": True,
                "intField": 1
            },
            {
                "boolField": True,
                "intField": 1
            },
            {
                "boolField": "True",
                "intField": 1
            }
        ]
    }).encode()

    try:

        # Send data
        print('sending {!r}'.format(message))
        sent = sock.sendto(message, SERVER_ADDRESS)

        # Receive response
        print('waiting to receive')
        sock.settimeout(1)
        data, server = sock.recvfrom(512)
        print(json.dumps(json.loads(data.decode("utf-8")), indent=4))

    finally:
        print('closing socket')
        sock.close()