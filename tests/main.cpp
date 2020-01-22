/**
 * File              : main.cpp
 * Author            : Philipp Zettl <philipp.zettl@godesteem.de>
 * Date              : 22.01.2020
 * Last Modified Date: 22.01.2020
 * Last Modified By  : Philipp Zettl <philipp.zettl@godesteem.de>
 */

#include <iostream>
#include "test_utils.h"



int main(){
  std::cout << "Starting test suite.\n";
  utilsTests();

  std::cout << "Test suite successfully executed.\n";
  
  return 0;
}
