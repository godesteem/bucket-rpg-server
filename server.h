/**
 * File              : server.h
 * Author            : Philipp Zettl <philipp.zettl@godesteem.de>
 * Date              : 21.01.2020
 * Last Modified Date: 22.01.2020
 * Last Modified By  : Philipp Zettl <philipp.zettl@godesteem.de>
 */
#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <cstring>
#include <map>
#include <enet/enet.h>
#include "utils.h"

class Server{
private:
  ENetHost* HOST;
  bool active = false;
  size_t ID = 1;
  size_t POLL_TIMEOUT = 1000 * 60 * 10; // for now we poll 10 minutes 
  size_t _internal_id = -1;
  int channel_count = 0;
  bool is_spreading = true;
  bool refuse_connections = true;

  std::map<int, ENetPeer*> peer_map;

  struct Packet {
    ENetPacket *packet;
    int from;
    int channel;
  };

  enum {
    SYSMSG_ADD_PEER,
    SYSMSG_REMOVE_PEER
  };

  enum {
    SYSCH_CONFIG,
    SYSCH_RELIABLE,
    SYSCH_UNRELIABLE,
    SYSCH_MAX
  };

public:
  std::string IP;
  size_t PORT;
  size_t MAX_CLIENTS;

  Server(size_t id, std::string ip, size_t port, size_t max_clients)
    :_internal_id(id), IP(ip), PORT(port), MAX_CLIENTS(max_clients){
      create();
  };

private:
  int create();

  int _process_config(ENetEvent event);
  int _process_message(ENetEvent event);

public:
  bool is_active() const;
  int run();

  int send_message(ENetPeer&, int, Packet);

  friend std::ostream& operator <<(std::ostream& os, const Server& s);

  ~Server();
};


#endif
