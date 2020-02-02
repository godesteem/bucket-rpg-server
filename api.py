import asyncio
import logging
import os

import websockets

from server import Server

logging.basicConfig(level=logging.INFO)
server = Server()
url = os.environ.get("URL", "0.0.0.0")
port = os.environ.get('PORT', 6666)


def app():
    start_server = websockets.serve(server.run, url, port)

    asyncio.get_event_loop().run_until_complete(start_server)
    logging.info(f'API::Info Server running on {url}:{port}')
    asyncio.get_event_loop().run_forever()


if __name__ == "__main__":
    app()
