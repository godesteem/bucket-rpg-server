/**
 * File              : node.h
 * Author            : Philipp Zettl <philipp.zettl@godesteem.de>
 * Date              : 25.01.2020
 * Last Modified Date: 25.01.2020
 * Last Modified By  : Philipp Zettl <philipp.zettl@godesteem.de>
 */
#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <string>

class Node
{
private:
  std::string _path;

public:
  std::string name;
  enum Type {
    NPC,
    PLAYER,
    OBJECT
  };
  Type type;

  Node();

  void set_name(std::string);
  std::string get_name();

  void set_type(Type);
  Type get_type();
  
  void set_path(std::string);
  std::string get_path();

  void set();


};

#endif //NODE_H
