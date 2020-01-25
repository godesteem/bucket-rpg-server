/**
 * File              : test_utils.h
 * Author            : Philipp Zettl <philipp.zettl@godesteem.de>
 * Date              : 22.01.2020
 * Last Modified Date: 25.01.2020
 * Last Modified By  : Philipp Zettl <philipp.zettl@godesteem.de>
 */
#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <cassert>
#include "../utils.h"

void test_upperFirst(){
  std::string val = "value";
  val = upperFirst(val);
  std::string expectedResult = "Value";

  assert(val == expectedResult);
  std::cout << "Testing utils::upperFirst() successfull." << std::endl;
}

void utilsTests(){
  std::cout << "Running tests for \"utils.h\"\n";

  test_upperFirst();

  std::cout << "Tested \"utils.h\" successfully.\n";
}

#endif // TEST_UTILS_H

