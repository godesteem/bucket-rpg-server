/**
 * File              : node.cpp
 * Author            : Philipp Zettl <philipp.zettl@godesteem.de>
 * Date              : 25.01.2020
 * Last Modified Date: 25.01.2020
 * Last Modified By  : Philipp Zettl <philipp.zettl@godesteem.de>
 */
#include "node.h"

void Node::set_path(std::string p){
  _path = p;
}

std::string Node::get_path(){
  return _path;
}

void Node::set_name(std::string n){
  name = n;
}

std::string Node::get_name(){
  return name;
}

void Node::set_type(Type t){
  type = t;
}

Node::Type Node::get_type(){
  return type;
}

