/**
 * File              : serializer.cpp
 * Author            : Philipp Zettl <philipp.zettl@godesteem.de>
 * Date              : 23.01.2020
 * Last Modified Date: 24.01.2020
 * Last Modified By  : Philipp Zettl <philipp.zettl@godesteem.de>
 */

#include "serializer.h"

void Serializer::put_var(void *data, void *data2){
  _put(data, data2); 
};

void* Serializer::get_var(void *data){
  return _get(data);
}

