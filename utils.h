/**
 * File              : utils.h
 * Author            : Philipp Zettl <philipp.zettl@godesteem.de>
 * Date              : 21.01.2020
 * Last Modified Date: 22.01.2020
 * Last Modified By  : Philipp Zettl <philipp.zettl@godesteem.de>
 */
#ifndef UTILS_H
#define UTILS_H


#include "buffer.h"

static inline uint32_t decode_uint32(const uint8_t *p_arr){
  uint32_t u = 0;
  for(int i=0; i<4; i++){
    uint32_t b = *p_arr;
    b <<= (i * 8);
    u |= b;
    p_arr++;
  }
  return u;
}

static inline std::string upperFirst(std::string value){
  std::string stringCopy = value;
  stringCopy[0] = toupper(stringCopy[0]);
  return stringCopy;
}

const void* addLengthFront(Buffer buffer){
  if(buffer.length > 0){
    
  }
  Buffer newBuffer = Buffer(0);
  return newBuffer;

}
#endif // UTILS_H
