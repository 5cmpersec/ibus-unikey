#include "unix/ibus/utils.h"


namespace utils {

// code from x-unikey, for convert charset that not is XUtf-8
int LatinToUtf(unsigned char* dst, unsigned char* src, int inSize, int* pOutSize)
{
    int i;
    int outLeft;
    unsigned char ch;

    outLeft = *pOutSize;

    for (i=0; i<inSize; i++)
    {
        ch = *src++;
        if (ch < 0x80)
        {
            outLeft -= 1;
            if (outLeft >= 0)
                *dst++ = ch;
        }
        else
        {
            outLeft -= 2;
            if (outLeft >= 0)
            {
                *dst++ = (0xC0 | ch >> 6);
                *dst++ = (0x80 | (ch & 0x3F));
            }
        }
    }

    *pOutSize = outLeft;
    return (outLeft >= 0);
}

void EraseCharsUtf8(std::string& utf8, unsigned int num_chars)
{
    if(utf8.empty())
    {
        return;
    }

    int i = 0;
    guchar c;

    for (i = utf8.length()-1; i >= 0 && num_chars > 0; i--)
    {
        c = utf8.at(i);

        // count down if byte is begin byte of utf-8 char
        if (c < (guchar)'\x80' || c >= (guchar)'\xC0')
        {
            num_chars--;
        }
    }

    utf8.erase(i+1);
}

} // namespace utils