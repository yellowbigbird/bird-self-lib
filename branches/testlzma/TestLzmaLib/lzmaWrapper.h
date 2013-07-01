#pragma once

#include <string>

class CLzmaWrapper{
public:
    CLzmaWrapper();
    virtual ~CLzmaWrapper();

    bool Encode(const std::string& strSrc, std::string& strOut);
    bool Decode(const std::string& strSrc, std::string& strOut);

    int    m_result;
};