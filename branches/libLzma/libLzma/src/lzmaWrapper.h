#pragma once

#include <string>

#include "lzma_dll.h"

class LZMA_DLL CLzmaWrapper{
public:
    CLzmaWrapper();
    virtual ~CLzmaWrapper();

    bool Encode(const std::string& strSrc, std::string& strOut);
    bool Decode(const std::string& strSrc, std::string& strOut);

protected:
    int    m_result;
};