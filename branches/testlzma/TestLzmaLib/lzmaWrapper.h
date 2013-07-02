#pragma once

#include <string>
//#include "types.h"


class CLzmaWrapper{
public:
    CLzmaWrapper();
    virtual ~CLzmaWrapper();

    bool Encode(const std::string& strSrc, std::string& strOut);
    bool Decode(const std::string& strSrc, std::string& strOut);

protected:
    int    m_result;
};