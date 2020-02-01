#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# File              : script.py
# Author            : Philipp Zettl <philipp.zettl@godesteem.de>
# Date              : 01.02.2020
# Last Modified Date: 01.02.2020
# Last Modified By  : Philipp Zettl <philipp.zettl@godesteem.de>
import asyncio
import json
import logging
import uuid

logging.basicConfig()


class User:
    def __init__(self, socket, user_id, peer_id):
        self.socket = socket
        self.user_id = user_id
        self.is_active = False
        self.peer_id = peer_id

    async def send(self, message):
        await self.socket.send(json.dumps(message.to_dict()))
        print(f'Send message: {message}')


class Message:
    def __init__(self, type, user_id, recipient_id, data, peer_id):
        self.type = type
        self.user_id = user_id
        self.recipient_id = recipient_id
        self.data = data
        self.peer_id = peer_id

    @classmethod
    def from_dict(cls, dictionary):
        return cls(**dictionary)

    def to_dict(self):
        return {
            'type': self.type,
            'recipient_id': self.recipient_id,
            'user_id': self.user_id,
            'peer_id': self.peer_id,
            'data': self.data,
        }

    def __str__(self):
        return str(self.to_dict())


class Server:
    STATE = {"value": 0}

    USERS = list()
    CURRENT_CONNECTED_SOCKET = None
    registration_open = False

    def get_user(self, user_id):
        for user in self.USERS:
            if str(user.user_id) == str(user_id):
                return user

    def get_user_by_socket(self, websocket):
        for user in self.USERS:
            if user.socket == websocket:
                return user

    def remove_user_by_socket(self, websocket):
        drop_user = self.get_user_by_socket(websocket)
        if drop_user:
            self.USERS.remove(drop_user)

    @staticmethod
    def generate_unique_id():
        return uuid.uuid4()

    async def bulk_distribute(self, message):
        for user in self.USERS:
            if user.user_id != message.user_id:
                await user.send(message)
    
    async def sent_to_all(self, msg):
        if len(self.USERS) > 1 and msg.user_id:
            for user in self.USERS:
                await user.send(msg)

    @staticmethod
    def users_event(user_id, data):
        return json.dumps({"type": "M", "user_id": user_id, "data": data})

    @staticmethod
    def user_connected_event(user_id):
        return json.dumps({"type": "C", "user_id": user_id})

    @staticmethod
    def user_disconnected_event(user_id):
        return json.dumps({"type": "D", "user_id": user_id})
    
    async def notify_connect(self, msg):
        if msg.user_id:
            if self.CURRENT_CONNECTED_SOCKET:
                self.get_user_by_socket(self.CURRENT_CONNECTED_SOCKET).user_id = msg.user_id
                self.get_user_by_socket(self.CURRENT_CONNECTED_SOCKET).peer_id = msg.peer_id
                self.CURRENT_CONNECTED_SOCKET = None
            if len(self.USERS) > 1:
                await self.bulk_distribute(msg)
            welcome_message = Message("C", 0, msg.user_id, msg.data, msg.peer_id)
            user = self.get_user(msg.user_id)
            await user.send(welcome_message)
    
    async def notify_joined(self, msg):
        if len(self.USERS) > 1 and msg.user_id:
            msg.type = "J"
            for user in self.USERS:
                await user.send(msg)
    
    async def notify_disconnect(self, msg):
        if len(self.USERS) > 1 and msg.user_id:
            await self.bulk_distribute(msg)

    def add_user(self, socket):
        if not self.get_user_by_socket(socket):
            self.USERS.append(User(socket, None, None))

    async def register_socket(self, websocket):
        self.add_user(websocket)
        self.CURRENT_CONNECTED_SOCKET = websocket
        await self.notify_connect(Message(type='C', user_id=0, recipient_id=-1, data={}, peer_id=None))
    
    async def register(self, msg):
        self.registration_open = True
        user = self.get_user(msg.user_id)
        self.USERS.remove(user)
        unique_id = self.generate_unique_id()
        while self.get_user(unique_id):
            unique_id = self.generate_unique_id()
    
        user.user_id = unique_id
        self.USERS.append(user)
        await user.send(Message("R", 0, msg.user_id, {"new_user_id": str(unique_id)}, msg.peer_id))
    
        self.registration_open = False
    
    async def unregister_socket(self, websocket):
        self.remove_user_by_socket(websocket)
        await self.notify_disconnect(Message(type='D', user_id=0, recipient_id=-1, data={}, peer_id=None))

    async def system(self, msg):
        signal = msg.data.get('signal')
        if signal:
            if signal == "done_preconfiguring":
                user = self.get_user(msg.user_id)
                user.is_active = True
                await user.send(Message(type="S", user_id=0, recipient_id=msg.user_id, data={"is_active": True}, peer_id=msg.peer_id))

    async def run(self, websocket, path):
        # register(websocket) sends user_event() to websocket
        await self.register_socket(websocket)
        try:
            async for message in websocket:
                await self.process_event(message)
        finally:
            await self.unregister_socket(websocket)

    async def process_event(self, message):
        msg = Message.from_dict(json.loads(message))
        type_map = {
            "C": self.notify_connect,
            "J": self.notify_joined,
            "D": self.notify_disconnect,
            "M": self.sent_to_all,
            "P": self.sent_to_all,
            "R": self.register,
            "S": self.system,
        }
        process_method = type_map.get(msg.type)

        if process_method:
            while self.registration_open:
                await asyncio.sleep(0.1)
            await process_method(msg)
            print(f"processed event: {msg}")
        else:
            logging.error(f"unsupported event: {msg}")
