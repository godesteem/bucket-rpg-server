#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# File              : script.py
# Author            : Philipp Zettl <philipp.zettl@godesteem.de>
# Date              : 25.01.2020
# Last Modified Date: 25.01.2020
# Last Modified By  : Philipp Zettl <philipp.zettl@godesteem.de>
import logging
import socket

log = logging.getLogger('udp_server')


def udp_server(host='127.0.0.1', port=6666):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    log.info("Listening on udp %s:%s" % (host, port))
    s.bind((host, port))
    while True:
        (data, addr) = s.recvfrom(128*1024)
        yield data


FORMAT_CONS = '%(asctime)s %(name)-12s %(levelname)8s\t%(message)s'
logging.basicConfig(level=logging.DEBUG, format=FORMAT_CONS)

for data in udp_server():
    log.debug("%r" % (data,))
