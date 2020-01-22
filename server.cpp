/**
 * File              : server.cpp
 * Author            : Philipp Zettl <philipp.zettl@godesteem.de>
 * Date              : 21.01.2020
 * Last Modified Date: 22.01.2020
 * Last Modified By  : Philipp Zettl <philipp.zettl@godesteem.de>
 */

#include "server.h"

void print_event(ENetEvent event){
  std::cout << "SERVER::Incoming data: \n" \
    << "ChannelID: " << event.channelID << ", \n" \
    << "Data: " << event.data << ", \n" \
    << "Packet: \n" \
    << "\tdata: " << event.packet->data << ", \n" \
    << "\tdataLength: " << event.packet->dataLength << ", \n" \
    << "\tflags: " << event.packet->flags << ", \n" \
    << "\tfreeCallback: " << event.packet->freeCallback << ", \n" \
    << "\treferenceCount: " << event.packet->referenceCount << ", \n" \
    << "\tuserData: " << event.packet->userData << "; \n" \
    << "Peer: \n" \
    << "\tAddress: " << event.peer->address.host << ", \n" \
    << "\tData: " << event.peer->data << ", \n" \
    << "\teventData: " << event.peer->eventData << ", \n" \
    << "Type: " << event.type << ", " \
    << std::endl;
}



// Constructors
int Server::create(){
  // TODO: Check conditions before creating a server, i.e. currently a server instance running on this port?
  if(active){
    fprintf(stderr, "SERVER::create ERROR: Server already running.");
    return -1; 
  }
  ENetAddress address;
  memset(&address, 0, sizeof(address));

  address.host = ENET_HOST_ANY;
  address.port = PORT;
  HOST = enet_host_create(&address, MAX_CLIENTS, 0, 0, 0);
  if(!HOST){
    fprintf(stderr, "SERVER::create ERROR: Cannot create host.");
  }
  active = true;
  return 1;
}

// Deconstructor
Server::~Server(){
  if(HOST){
    enet_host_destroy(HOST);
  }
}

// Setter

// Getter
bool Server::is_active() const{
  return this->active;
}

// Methods

int Server::_process_config(ENetEvent event){
  int msg = decode_uint32(&event.packet->data[0]);
  int id = decode_uint32(&event.packet->data[4]);

  int processed = 0;
  switch(msg){
    case SYSMSG_ADD_PEER:
      peer_map[id] = NULL;
      // peer connected
      std::cout << "Peer " << id << " connected" << std::endl;
      processed = 1;
      break;
    case SYSMSG_REMOVE_PEER:
      peer_map.erase(id);
      // peer disconnected
      std::cout << "Peer " << id << " disconnected" << std::endl;
      processed = 1;
      break;
  }
  std::cout << "MSG: " << msg << " ID: " << id << std::endl;
  return processed;
}
int Server::_process_message(ENetEvent event){
  Packet packet;
  packet.packet = event.packet;

  uint32_t *id = (uint32_t *)event.peer->data;
  if(event.packet->dataLength < 8){
    return -1;
  }

  uint32_t source = decode_uint32(&event.packet->data[0]);
  int target = decode_uint32(&event.packet->data[4]);
  packet.from = source;
  packet.channel = event.channelID;
          
  if(source != *id){
    // TODO: Cheater!
    return -2;
  }
  packet.from = *id;

  if(target == 1){
    // this is for us
  } else if (!is_spreading) {
    return 0;
  } else if (target == 0){
    // distribute to everyone except sender *shrug*
    //incoming_packets.push_back(packet);
    for(std::map<int, ENetPeer*>::iterator E= peer_map.begin(); E != peer_map.end(); ++E){
      std::cout << "E: " << E->first << std::endl;
      if(E->first == source){
        continue;
      }
      send_message(*E->second, event.channelID, packet);
    }
  } else if (target < 0){
    // distribute to everyone except 1
    for(std::map<int, ENetPeer*>::iterator E= peer_map.begin(); E != peer_map.end(); ++E){
      std::cout << "E: " << E->first << std::endl;
      if(uint32_t(E->first) == source || E->first == -target){
        continue;
      }
      send_message(*E->second, event.channelID, packet);
    }
    if(-target != -1){
      // server is not excluded
      // incoming_packets.push_back(packet);
    } else {
      // Server is excluded, clean up!
      enet_packet_destroy(packet.packet);
    }
  } else {
    // specific target
    if(peer_map.find(target) == peer_map.end()){
      // Who dis?
    }
    enet_peer_send(peer_map[target], event.channelID, packet.packet);
  }
  std::cout << packet.packet << std::endl;
  return 1;
}

int Server::send_message(ENetPeer &peer, int channelID, Server::Packet packet){
  ENetPacket *packet2 = enet_packet_create(packet.packet->data, packet.packet->dataLength, packet.packet->flags);
  return enet_peer_send(&peer, channelID, packet2);
}

int Server::run(){
  ENetEvent event;
  active = true;
  refuse_connections = false;
  while(enet_host_service(HOST, &event, POLL_TIMEOUT) > 0){
    switch(event.type){
      case ENET_EVENT_TYPE_CONNECT:
        std::cout << "Player () from " \
          << event.peer->address.host \
          <<":"<< event.peer->address.port \
          << " connected to the server." \
          << std::endl;
        if(refuse_connections){
          // TODO: error handling
          break;
        }
        refuse_connections = peer_map.size() >= MAX_CLIENTS;
        
        break;

      case ENET_EVENT_TYPE_DISCONNECT:
        std::cout << "Player () from " \
          << event.peer -> address.host \
          <<":"<< event.peer->address.port \
          << " disconnected from the server." \
          << std::endl;
        event.peer->data = NULL;
        break;

      case ENET_EVENT_TYPE_RECEIVE:
        print_event(event);
        if(event.channelID == SYSCH_CONFIG){
          int processed = _process_config(event);
          // TODO: log result
          enet_packet_destroy(event.packet);
        } else if (event.channelID < channel_count) {
          int processed = _process_message(event);
          // TODO: log result
          enet_packet_destroy(event.packet);
        }
        break;
      case ENET_EVENT_TYPE_NONE:
        // do nothing
        break;
    }
  }
  active = false;
  std::cout << "Polled for " << float(POLL_TIMEOUT/1000.0) << " Seconds, no package received. "<< _internal_id << " Going to sleep." << std::endl;
  return 0;
}

// Operators

// Friend operators
std::ostream& operator<<(std::ostream& os, const Server& s){
  os << "IP: "<< s.IP<< "\tPORT: " << s.PORT << "\tActive: " << (s.is_active() ? "True" : "False");
  return os;
}


/*
Error NetworkedMultiplayerENet::create_server(int p_port, int p_max_clients, int p_in_bandwidth, int p_out_bandwidth) {

	ERR_FAIL_COND_V(active, ERR_ALREADY_IN_USE);
	ERR_FAIL_COND_V(p_port < 0 || p_port > 65535, ERR_INVALID_PARAMETER);
	ERR_FAIL_COND_V(p_max_clients < 0, ERR_INVALID_PARAMETER);
	ERR_FAIL_COND_V(p_in_bandwidth < 0, ERR_INVALID_PARAMETER);
	ERR_FAIL_COND_V(p_out_bandwidth < 0, ERR_INVALID_PARAMETER);

	_setup_compressor();
	active = true;
	server = true;
	refuse_connections = false;
	unique_id = 1;
	connection_status = CONNECTION_CONNECTED;
	return OK;
}
*/
