
#pragma once

#include <vector>
#include <string>

#include "types.h"

class CStringStreamIn
{
public:
    CStringStreamIn(const std::string& dataIn);

    bool    ReadData(void *data, const size_t sizeToRead, size_t& sizeHasRead);
    //bool    WriteData(const void *data, const size_t sizeToRead, size_t& writtenSize);

    ISeqInStream        m_streamIn;
    //ISeqOutStream       m_streamOut;

    const std::string&      m_dataIn;
    //std::string&            m_dataOut;

    UINT                    m_pointerIdxIn;

};

class CStringStreamOut
{
public:
    CStringStreamOut(std::string& data);
    bool  WriteData(const void *data, const size_t sizeToRead, size_t& writtenSize);
    
    //ISeqInStream        m_streamIn;
    ISeqOutStream       m_streamOut;

    //const std::string&      m_dataIn;
    std::string&            m_dataOut;
};
