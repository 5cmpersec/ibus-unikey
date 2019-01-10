#pragma once

#include <string>
#include <glib.h>

namespace utils {

int LatinToUtf(unsigned char* dst, unsigned char* src, int inSize, int* pOutSize);

void EraseCharsUtf8(std::string& utf8, unsigned int num_chars);

} // namespace utils
