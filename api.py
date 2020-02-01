import asyncio
import os

import websockets

from server import Server

server = Server()
url = os.environ.get("URL", "localhost")
port = os.environ.get('PORT', 6666)


def app():
    start_server = websockets.serve(server.run, url, port)

    asyncio.get_event_loop().run_until_complete(start_server)
    print(f'Server running on {url}:{port}')
    asyncio.get_event_loop().run_forever()


if __name__ == "__main__":
    app()
