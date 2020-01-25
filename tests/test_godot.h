/**
 * File              : test_godot.h
 * Author            : Philipp Zettl <philipp.zettl@godesteem.de>
 * Date              : 25.01.2020
 * Last Modified Date: 25.01.2020
 * Last Modified By  : Philipp Zettl <philipp.zettl@godesteem.de>
 */
#ifndef TEST_GODOT_H
#define TEST_GODOT_H

#include <cassert>
#include "../godot/marshalls.h"


void test_decode_uint32(){
  std::cout << "Testing GODOT::MARSHALLS::decode_uint32." << std::endl;
  int a = 1;
  const uint8_t *arr = (uint8_t*)&a;
  uint32_t result = decode_uint32(arr);

  assert(result == a);

  std::cout << "GODOT::MARSHALLS::decode_uint32 is working." << std::endl;
}

void test_decode_uint16(){
  std::cout << "Testing GODOT::MARSHALLS::decode_uint16." << std::endl;
  int a = 2;
  const uint8_t *arr = (uint8_t*)&a;
  uint16_t result = decode_uint16(arr);

  assert(result == a);

  std::cout << "GODOT::MARSHALLS::decode_uint16 is working." << std::endl;
}

void test_decode_float(){
  std::cout << "Testing GODOT::MARSHALLS::decode_float." << std::endl;
  
  float a = 2.2;
  const uint8_t *arr = (uint8_t *)&a;
  float result = decode_float(arr);

  assert(result == a);
  
  std::cout << "GODOT::MARSHALLS::decode_float is working." << std::endl;

}

void marshallsTest(){
  std::cout << "Testing GODOT::MARSHALLS." << std::endl;

  test_decode_uint32();
  test_decode_uint16();
  test_decode_float();

  std::cout << "GODOT::MARSHALLS is working." << std::endl;
}

#endif // TEST_GODOT_H
