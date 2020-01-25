/**
 * File              : utils.cpp
 * Author            : Philipp Zettl <philipp.zettl@godesteem.de>
 * Date              : 25.01.2020
 * Last Modified Date: 25.01.2020
 * Last Modified By  : Philipp Zettl <philipp.zettl@godesteem.de>
 */

#include "utils.h"

static std::string upperFirst(std::string value){
  std::string stringCopy = value;
  stringCopy[0] = toupper(stringCopy[0]);
  return stringCopy;
}
void print_line(std::string line){
  std::cout << line;
}
template <class T>
T *_get_data(T input){
  if(!input)
    return NULL;
  return reinterpret_cast<T *>(input);
}
template <class T>
T *ptrw(T input){
  //_copy_on_write();
  return (T *)_get_data(input);
}

bool parse_utf8(const char *p_utf8, int p_len, std::string input) {

#define _UNICERROR(m_err) print_line("Unicode error: " + std::string(m_err));
	if (!p_utf8)
		return true;

        std::string aux = std::string(p_utf8);

	int cstr_size = 0;
	int str_size = 0;

        std::cout << "utils::utf8 init done "<< aux << "\n";

	/* HANDLE BOM (Byte Order Mark) */
	if (p_len < 0 || p_len >= 3) {

		bool has_bom = uint8_t(p_utf8[0]) == 0xEF && uint8_t(p_utf8[1]) == 0xBB && uint8_t(p_utf8[2]) == 0xBF;
		if (has_bom) {

			//just skip it
			if (p_len >= 0)
				p_len -= 3;
			p_utf8 += 3;
		}
	}
        std::cout << "utils::utf8 bom handled\n";
	{
		const char *ptrtmp = p_utf8;
		const char *ptrtmp_limit = &p_utf8[p_len];
		int skip = 0;
                std::cout << "enter while\n";
		while (ptrtmp != ptrtmp_limit && *ptrtmp) {
                    std::cout << "while begin\n";
			if (skip == 0) {
                          std::cout << "skip = false\n";

				uint8_t c = *ptrtmp >= 0 ? *ptrtmp : uint8_t(256 + *ptrtmp);
                          std::cout << "found c: " << c << std::endl;
				/* Determine the number of characters in sequence */
				if ((c & 0x80) == 0)
					skip = 0;
				else if ((c & 0xE0) == 0xC0)
					skip = 1;
				else if ((c & 0xF0) == 0xE0)
					skip = 2;
				else if ((c & 0xF8) == 0xF0)
					skip = 3;
				else if ((c & 0xFC) == 0xF8)
					skip = 4;
				else if ((c & 0xFE) == 0xFC)
					skip = 5;
				else {
					_UNICERROR("invalid skip");
					return true; //invalid utf8
				}

				if (skip == 1 && (c & 0x1E) == 0) {
					//printf("overlong rejected\n");
					_UNICERROR("overlong rejected");
					return true; //reject overlong
				}

				str_size++;

			} else {

				--skip;
			}

			cstr_size++;
			ptrtmp++;
		}

		if (skip) {
			_UNICERROR("no space left");
			return true; //not enough spac
		}
	}
        std::cout << "while loop done \n";

	if (str_size == 0) {
		//clear();
		return false;
	}
        std::cout << "prepare for copy\n"; 
	aux.resize(str_size + 1);
        std::cout << "resize done\n";
        std::string dst = aux;
        std::cout << "ptrw done " << std::string(dst) << "\n" << sizeof(dst) << std::endl;
	dst[str_size] = 0;
        
        std::cout << "start copy\n";
        size_t i = 0;
	while (cstr_size) {

		int len = 0;

		/* Determine the number of characters in sequence */
		if ((*p_utf8 & 0x80) == 0)
			len = 1;
		else if ((*p_utf8 & 0xE0) == 0xC0)
			len = 2;
		else if ((*p_utf8 & 0xF0) == 0xE0)
			len = 3;
		else if ((*p_utf8 & 0xF8) == 0xF0)
			len = 4;
		else if ((*p_utf8 & 0xFC) == 0xF8)
			len = 5;
		else if ((*p_utf8 & 0xFE) == 0xFC)
			len = 6;
		else {
			_UNICERROR("invalid len");

			return true; //invalid UTF8
		}

		if (len > cstr_size) {
			_UNICERROR("no space left");
			return true; //not enough space
		}

		if (len == 2 && (*p_utf8 & 0x1E) == 0) {
			//printf("overlong rejected\n");
			_UNICERROR("no space left");
			return true; //reject overlong
		}

		/* Convert the first character */

		uint32_t unichar = 0;

		if (len == 1)
			unichar = *p_utf8;
		else {

			unichar = (0xFF >> (len + 1)) & *p_utf8;

			for (int i = 1; i < len; i++) {

				if ((p_utf8[i] & 0xC0) != 0x80) {
					_UNICERROR("invalid utf8");
					return true; //invalid utf8
				}
				if (unichar == 0 && i == 2 && ((p_utf8[i] & 0x7F) >> (7 - len)) == 0) {
					_UNICERROR("invalid utf8 overlong");
					return true; //no overlong
				}
				unichar = (unichar << 6) | (p_utf8[i] & 0x3F);
			}
		}

		printf("char %i, len %i\n",unichar,len);
		if(sizeof(wchar_t) == 2 && unichar > 0xFFFF) {
			unichar = ' '; //too long for windows
		}

		dst[i] = unichar;
		cstr_size -= len;
		p_utf8 += len;
                i++;
	}
        std::cout << "copy done " << dst << "\n";
        input = dst;
	return false;
}


std::string utf8(const char *p_utf8, int p_len) {
  std::string ret;
  parse_utf8(p_utf8, p_len, ret);
  return ret;
};
/*
void* addLengthFront(Buffer buffer){
  if(buffer.length > 0){
    
  }
  Buffer newBuffer = Buffer(0);
  return newBuffer.getContent();

}
*/

