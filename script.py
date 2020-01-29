#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# File              : script.py
# Author            : Philipp Zettl <philipp.zettl@godesteem.de>
# Date              : 25.01.2020
# Last Modified Date: 25.01.2020
# Last Modified By  : Philipp Zettl <philipp.zettl@godesteem.de>
import logging
import socket
import struct
import enet

log = logging.getLogger('udp_server')


class NoEventAvailableError(Exception):
    def __init__(self, *args):
        super().__init__(*args)


class Client:
    def __init__(self, address: enet.Address, _id: int):
        self.address = address
        self.id = _id
        self.name = ""
        self.SEED = 5
        self.position = [0.0, 0.0]

    def __str__(self):
        return f'{self.id} from {self.address}'


class Clients:
    def __init__(self):
        self._clients = []

    def add_client(self, client: Client):
        for set_client in self._clients:
            if client == set_client:
                return

        self._clients.append(client)

    def remove_client(self, client: Client):
        client_index = -1
        for index, set_client in enumerate(self._clients):
            if client == set_client:
                client_index = index
                break

        if client_index > -1:
            self._clients.pop(client_index)

    def to_dict(self):
        return {
            c.id: {'name': c.name, 'position': c.position, 'SEED': c.SEED} for c in self._clients
        }


class EventProcessor:
    def __init__(self):
        pass

    def _send_player_info(self, event):
        pass

    def pre_configure_game(self, event):
        pass


class UDPServer(EventProcessor):
    def __init__(self, host, port):
        super().__init__()
        self.host = host
        self.port = port
        self.peer = enet.Host(enet.Address(bytes(host, 'utf-8'), port), 1, 0, 0)
        self._current_event = None
        self.clients = Clients()

    def run(self):
        while event := self.peer.service(0):
            if event.type != enet.EVENT_TYPE_NONE:
                self.process_event(event)

    def _process_connect(self):
        if not self._current_event:
            raise NoEventAvailableError()
        client = Client(self._current_event.peer.address, self._current_event.peer.incomingPeerID)
        self.clients.add_client(client)
        log.debug(f'Client {client} connected.')
        # rpc_id(id, "pre_configure_game", players)
        # TODO: send packet \w info of connection
        #  to all connected clients
        packet = enet.Packet(b'Packetle', enet.PACKET_FLAG_RELIABLE)
        self.peer.broadcast(0, packet)

    def _process_disconnect(self):
        if not self._current_event:
            raise NoEventAvailableError()
        client = Client(self._current_event.peer.address, self._current_event.peer.incomingPeerID)
        self.clients.remove_client(client)
        log.debug(f'Client {client} disconnected.')

    def _process_none(self):
        log.debug('Process None')

    def _process_receive(self):
        log.debug('Receive data')
        event = self._current_event
        if event.packet.is_valid():
            packet_size = len(event.packet.data)
            out = str(event.packet.data)
            log.debug(event.peer)
            source = int(event.packet.data[0])
            target = int(event.packet.data[4])
            log.debug(f'{source} -> {target}')
            log.debug(out)
            self.send_packet(event.packet, event.channelID)

    def send_packet(self, packet: enet.Packet, channel: int):
        self.peer.broadcast(channel, packet)

    def process_event(self, event):
        event_process_map = {
            enet.EVENT_TYPE_CONNECT: self._process_connect,
            enet.EVENT_TYPE_DISCONNECT: self._process_disconnect,
            enet.EVENT_TYPE_RECEIVE: self._process_receive,
            enet.EVENT_TYPE_NONE: self._process_none,
        }
        log.debug(event.type)
        self._current_event = event
        event_process_map[event.type]()
        self.peer.flush()
        self._current_event = None


FORMAT_CONS = '%(asctime)s %(name)-12s %(levelname)8s\t%(message)s'
logging.basicConfig(level=logging.DEBUG, format=FORMAT_CONS)

if __name__ == '__main__':
    server = UDPServer('localhost', 6666)
    server.run()
