/**
 * File              : server.cpp
 * Author            : Philipp Zettl <philipp.zettl@godesteem.de>
 * Date              : 21.01.2020
 * Last Modified Date: 25.01.2020
 * Last Modified By  : Philipp Zettl <philipp.zettl@godesteem.de>
 */

#include "server.h"

void print_event(ENetEvent event){
  std::cout << "SERVER::Incoming data: \n" \
    << "ChannelID: " << unsigned(event.channelID) << ", \n" \
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
  channel_count = SYSCH_MAX;
  unique_id = 1;
  return 1;
}

// Deconstructor
Server::~Server(){
  if(HOST){
    close_connection(0);
  }
}

// Setter

// Getter
bool Server::is_active() const{
  return this->active;
}

int Server::get_packet_peer() const {
  if(!active){
    std::cout << "Server inactive!\n";
    return -1;
  }
  if(incoming_packets.size() == 0){
    std::cout << "No packet to work with.\n";
    return -2;
  }
  std::cout << "requesting packet: " << std::endl;
  return incoming_packets.front().from;
}
int Server::get_packet_channel() const {
  if(!active){
    std::cout << "Server inactive!\n";
    return -1;
  }
  if(incoming_packets.size() == 0){
    std::cout << "No packet to work with.\n";
    return -2;
  }
  return incoming_packets.front().channel;
}

// Methods

void Server::disconnect_peer(std::map<int, ENetPeer*>::iterator E){
  enet_peer_disconnect_now(E->second, unique_id);
  int *id = (int *)(E->second->data);
  free(id);
}

void Server::close_connection(uint32_t wait_usec){
  bool peers_disconnected = false;
  for(std::map<int, ENetPeer*>::iterator E= peer_map.begin(); E != peer_map.end(); ++E){
    if(E->first){
      disconnect_peer(E);
      peers_disconnected = true;
    }
    std::cout << "E: " << E->first << std::endl;
  }

  if(peers_disconnected){
    enet_host_flush(HOST);
    if(wait_usec > 0){
      usleep(wait_usec);
    }

  }
  incoming_packets.clear();
  peer_map.clear();
  active = false;
  enet_host_destroy(HOST);
}

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
  std::cout << "Unpack packet.\n";
  uint32_t *id = (uint32_t *)event.peer->data;
  if(event.packet->dataLength < 8){
    std::cout << "Packet too small.\n";
    return -1;
  }
  std::cout << "Decode packet.\n";

  uint32_t source = decode_uint32(&event.packet->data[0]);
  int target = decode_uint32(&event.packet->data[4]);
  packet.from = source;
  packet.channel = event.channelID;
  
  std::cout << "Decoded. " << &id << " from "<< &source << "\n";

  if(&source != id){
    std::cout << "Cheater detected.\n";
    // TODO: Cheater!
    //return -2;
  }
  //packet.from = *id;

  std::cout << "Start processing...\n";
  if(target == 1){
    // this is for us
    std::cout << "Message for Server.\n";
    incoming_packets.push_back(packet);
    process_messages();

  } else if (!is_spreading) {
    std::cout << "Server does not spread messages.\n";

    return 0;
  } else if (target == 0){
    std::cout << "Distribute packet to everyone except sender.\n";

    // distribute to everyone except sender *shrug*
    incoming_packets.push_back(packet);
    for(std::map<int, ENetPeer*>::iterator E= peer_map.begin(); E != peer_map.end(); ++E){
      std::cout << "E: " << E->first << std::endl;
      if(E->first == source){
        continue;
      }
      send_message(*E->second, event.channelID, packet);
    }
  } else if (target < 0){
      std::cout << "Distribute packet to everyone except Server.\n";

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
      incoming_packets.push_back(packet);
      std::cout << "Server allowed to process packet.\n";

    } else {
      std::cout << "Server not allowed to process packet, clean up!\n";

      // Server is excluded, clean up!
      enet_packet_destroy(packet.packet);
    }
  } else {
    std::cout << "Send packet to specific target.\n";

    // specific target
    if(peer_map.find(target) == peer_map.end()){
      // Who dis?
      std::cout << "Do not know target.\n";
    }
    enet_peer_send(peer_map[target], event.channelID, packet.packet);
  }
  std::cout << "Packet: " << packet.packet << "\tSender: " << packet.from << "\tTarget: " << target << std::endl;
  return 1;
}

int Server::send_message(ENetPeer &peer, int channelID, Server::Packet packet){
  ENetPacket *packet2 = enet_packet_create(packet.packet->data, packet.packet->dataLength, packet.packet->flags);
  return enet_peer_send(&peer, channelID, packet2);
}



void Server::_pop_current_packet(){
  if(current_packet.packet){
    enet_packet_destroy(current_packet.packet);
    current_packet.packet = NULL;
    current_packet.from = 0;
    current_packet.channel = -1;
  }
}

Error Server::get_packet(const uint8_t **buffer, int &buffer_size){
  if(incoming_packets.size() == 0){
    std::cout << "No packets to work with.\n";
    return ERR_UNAVAILABLE;
  }

  _pop_current_packet();

  current_packet = incoming_packets.front();
  incoming_packets.pop_front();

  *buffer = (const uint8_t *)(&current_packet.packet->data[8]);
  buffer_size = current_packet.packet->dataLength - 8;
  return OK; 
}


int Server::get_available_packet_count(){
  return incoming_packets.size();
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

int Server::run(){
  process_messages();
  return OK;
}
int Server::process_messages(){
  if(!HOST){
    return -1;
  }
  std::cout << "Start to poll.\n";
  poll_messages();
  std::cout << "Done polling.\n";
  
  int prev_count = get_available_packet_count();
  int current_count = 0;
  std::cout << "Process "<<prev_count<< " packets.\n";
  while(get_available_packet_count() > 0){
    std::cout << "Start processing of event #" << prev_count - current_count << std::endl;
    current_count++;
    int sender = get_packet_peer();
    const uint8_t *packet;
    int len;

    Error err = get_packet(&packet, len);
    if(err != OK){
      break;
    }
    std::cout << "Init done." << std::endl;
    rpc_sender_id = sender;
    _process_packet(sender, packet, len);
    rpc_sender_id = 0;
    std::cout << "Action performed.\n";
    if(!HOST){
      break;
    }
  }
  return 1; 
}
int Server::poll_messages(){
  ENetEvent event;
  peer_active = true;
  refuse_connections = false;
  while(enet_host_service(HOST, &event, POLL_TIMEOUT) > 0){
    switch(event.type){
      case ENET_EVENT_TYPE_CONNECT:
        std::cout << "Player () from " \
          << event.peer->address.host \
          << " connected to the server." \
          << std::endl;
        if(refuse_connections){
          // TODO: error handling
          break;
        }
        refuse_connections = peer_map.size() >= MAX_CLIENTS;
        // TODO: add player to list of connected players 
        break;

      case ENET_EVENT_TYPE_DISCONNECT:
        std::cout << "Player () from " \
          << event.peer -> address.host \
          << " disconnected from the server." \
          << std::endl;
        event.peer->data = NULL;
        // TODO: Remove player from list of connected players
        break;

      case ENET_EVENT_TYPE_RECEIVE:{
          print_event(event);
          int channelID = unsigned(event.channelID);
          if(channelID == SYSCH_CONFIG){
            int processed = _process_config(event);
            // TODO: log result
            std::cout << "Processing config.\n";
            enet_packet_destroy(event.packet);
          } else if (channelID < channel_count) {
            std::cout << "Processing message.\n";
            int processed = _process_message(event);
            std::cout << "Processed message.\n";
            // TODO: log result
            enet_packet_destroy(event.packet);
          } else {
            std::cout << "No clue what to do. Emit message.\n";
          }
        }
        break;
      case ENET_EVENT_TYPE_NONE:
        // do nothing
        break;
    }
  }
  peer_active = false;
  std::cout << "Polled for " << float(POLL_TIMEOUT/1000.0) << " Seconds, no package received. "<< _internal_id << " Going to sleep." << std::endl;
  return 0;
}
Node* Server::_get_node(int from, const uint8_t *packet, int packet_len){
  uint32_t target = decode_uint32(&packet[1]);
  Node *node = NULL;

  if(target & 0x8000000){
    int ofs = target & 0x7FFFFFF;
    if(ofs >= packet_len){

    }
    std::string _paths;
    parse_utf8((const char *)&packet[ofs], packet_len - ofs, _paths);

    for(std::list<Node *>::iterator p = nodes.begin(); p!=nodes.end(); ++p){
      if((*p)->get_path() == _paths){
        node = (*p);
      }
    }
    if(!node){
      std::cout << "Failed to get node!\n";
    }
  } else {
    int id = target;
    std::cout << "NOT IMPLEMENTED YET\n";
  }
  return node;
}
void Server::_process_packet(int from, const uint8_t *packet, int packet_len){
  std::cout << "Processing packet.\n";
  if(packet_len < 1){
    //TODO: packet too small
    std::cout << "Packet too small!\n";
  }
  uint8_t packet_type = packet[0];

  Node *node = _get_node(from, packet, packet_len);

  std::cout << "Packet type: "<< unsigned(packet_type) << std::endl;

  switch(packet_type){
    case NETWORK_COMMAND_SIMPLIFY_PATH:{
      
    } break;
    case NETWORK_COMMAND_CONFIG_PATH:{

    } break;
    case NETWORK_COMMAND_REMOTE_CALL:
    case NETWORK_COMMAND_REMOTE_SET:{
      std::cout << "Action type: SET/CALL\n";
      int len_end = 5;
      for(; len_end < packet_len; ++len_end){
        if(packet[len_end] == 0){
          break;
        }
      }
      if(len_end > packet_len){
        std::cout << "Packet too small!\n";
      }
      std::cout << "Converting " << (const char*)&packet[5] << " to utf-8\n";

      const char* name = utf8((const char*)&packet[5], packet_len).c_str();
      
      std::cout << "Converted. " << std::string(name) << "\n";
      if(packet_type == NETWORK_COMMAND_REMOTE_CALL){
        std::cout << "CALL: "<< name << "\n";
        _process_rpc(node, name, from, packet, packet_len, len_end + 1);
      }
      else {
        std::cout << "SET: " << name << "\n";
        _process_rset(node, name, from, packet, packet_len, len_end + 1);
      }
    } break;
    case NETWORK_COMMAND_RAW: {
            
    } 
  }
}

void Server::_process_rpc(Node *node,const char* name, int from, const uint8_t *packet, int packet_len, int packet_offset){
}
void Server::_process_rset(Node *node, const char* name, int from ,const uint8_t *packet, int packet_len, int packet_offset){
  if(packet_offset >= packet_len){
    std::cout << "Invalid packet, too small!\n";
    return;
  }
  Variant value;

  bool valid;

  node->set(name, value, &valid);

  if(!valid){
    std::cout << "Error setting remote value '" << name << "' not found in object of type " << node->get_type() << ".\n";
  }
}
