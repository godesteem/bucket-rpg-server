/**
 * File              : serializer.h
 * Author            : Philipp Zettl <philipp.zettl@godesteem.de>
 * Date              : 22.01.2020
 * Last Modified Date: 23.01.2020
 * Last Modified By  : Philipp Zettl <philipp.zettl@godesteem.de>
 */
#ifndef SERIALIZER_H
#define SERIALIZER_H


class Serializer
{
private:
  void *_buffer;

public:

  Serializer()
  {}


private:
  void* _get(void *);
  void _put(void *, void *);

public:
  
  void put_var(void*, void*);
  void* get_var(void*);
  
  

};
#endif // SERIALIZER_H
