/**
 * File              : utils.h
 * Author            : Philipp Zettl <philipp.zettl@godesteem.de>
 * Date              : 21.01.2020
 * Last Modified Date: 25.01.2020
 * Last Modified By  : Philipp Zettl <philipp.zettl@godesteem.de>
 */
#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>

static std::string upperFirst(std::string);

void _print_line(std::string);

template<class T>
T *_get_data(T);
template<class T>
T *ptrw(T);
bool parse_utf8(const char*, int, std::string);

std::string utf8(const char*, int);

#endif // UTILS_H
