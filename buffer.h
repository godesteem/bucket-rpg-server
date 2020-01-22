/**
 * File              : buffer.h
 * Author            : Philipp Zettl <philipp.zettl@godesteem.de>
 * Date              : 22.01.2020
 * Last Modified Date: 22.01.2020
 * Last Modified By  : Philipp Zettl <philipp.zettl@godesteem.de>
 */
#ifndef BUFFER_H
#define BUFFER_H
#include <stdint.h>
#include <cstddef>

class Buffer
{
private:
  void *content;
public:
  size_t length = 0;

  Buffer(size_t size)
    :length(size)
     {}
  
  void writeUInt32LE(uint32_t value, size_t offset);
  void alloc(size_t size);
  void concat(void* lists);


};

#endif // BUFFER_H
