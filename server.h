/**
 * File              : server.h
 * Author            : Philipp Zettl <philipp.zettl@godesteem.de>
 * Date              : 21.01.2020
 * Last Modified Date: 25.01.2020
 * Last Modified By  : Philipp Zettl <philipp.zettl@godesteem.de>
 */
#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <cstring>
#include <string>
#include <map>
#include <list>
#include <unistd.h>
#include <enet/enet.h>
#include "godot/marshalls.h"
#include "godot/error_list.h"
#include "utils.h"
#include "node.h"

template<class T>
class Variant<T>{

};

class Server{
private:
  ENetHost* HOST;
  bool active = false;
  bool peer_active = false;
  size_t ID = 1;
  size_t POLL_TIMEOUT = 1000; // for now we poll 10 minutes 
  size_t _internal_id = -1;
  int channel_count = 0;
  bool is_spreading = true;
  bool refuse_connections = true;
  int rpc_sender_id;

  std::list<Node *> nodes;

  std::map<int, ENetPeer*> peer_map;

  struct Packet {
    ENetPacket *packet;
    int from;
    int channel;
  };
  
  std::list<Packet> incoming_packets;
  Packet current_packet;

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
  int unique_id;

  enum {
    NETWORK_COMMAND_REMOTE_CALL,
    NETWORK_COMMAND_REMOTE_SET,
    NETWORK_COMMAND_SIMPLIFY_PATH,
    NETWORK_COMMAND_CONFIG_PATH,
    NETWORK_COMMAND_RAW,

  };

  Server(size_t id, std::string ip, size_t port, size_t max_clients)
    :_internal_id(id), IP(ip), PORT(port), MAX_CLIENTS(max_clients){
      create();
  };

private:
  int create();

  int _process_config(ENetEvent event);
  int _process_message(ENetEvent event);

  void disconnect_peer(std::map<int, ENetPeer*>::iterator);

  int get_packet_peer() const;
  int get_packet_channel() const;
  int get_available_packet_count();
  Error get_packet(const uint8_t **, int &);
  void _pop_current_packet();
  void _process_packet(int, const uint8_t*, int);
  void _process_rpc(Node*, const char* name, int from, const uint8_t *packet, int packet_len, int packet_offset);
  void _process_rset(Node*, const char* name, int from, const uint8_t *packet, int packet_len, int packet_offset);
  Node* _get_node(int, const uint8_t*, int);
  
  int poll_messages();
  int process_messages();
public:
  bool is_active() const;
  int run();

  int send_message(ENetPeer&, int, Packet);

  void close_connection(uint32_t);

  friend std::ostream& operator <<(std::ostream& os, const Server& s);

  ~Server();
};


#endif
