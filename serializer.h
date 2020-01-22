/**
 * File              : serializer.h
 * Author            : Philipp Zettl <philipp.zettl@godesteem.de>
 * Date              : 22.01.2020
 * Last Modified Date: 22.01.2020
 * Last Modified By  : Philipp Zettl <philipp.zettl@godesteem.de>
 */

class Serializer
{
private:


public:

  Serializer()
    :{}

private:
  void* _get(void *);
  void _put(void *, void *);

public:
  
  void put_var(void *data){
    this._buffer = this._internalPut(data); 
  };
  void* get_var(void *);
  
  

};
