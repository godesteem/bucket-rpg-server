/**
 * File              : main.cpp
 * Author            : Philipp Zettl <philipp.zettl@godesteem.de>
 * Date              : 21.01.2020
 * Last Modified Date: 25.01.2020
 * Last Modified By  : Philipp Zettl <philipp.zettl@godesteem.de>
 */
#include <cstdio>
#include <enet/enet.h>
#include "server.h"


int
main(int argc, char **argv){
  if (enet_initialize() != 0){
    fprintf(stderr, "");
    return EXIT_FAILURE;
  }
  atexit(enet_deinitialize);
  
  while(1){
    Server A = Server(1, "localhost", 6666, 2);

    A.run();
  }

  return 0;

}
