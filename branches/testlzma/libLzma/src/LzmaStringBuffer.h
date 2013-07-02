
#pragma once

#include <vector>
#include <string>

#include "types.h"

class CStringStream
{
public:
    CStringStream(const std::string& dataIn, std::string& dataOut);

    bool    ReadData(void *data, const size_t sizeToRead, size_t& sizeHasRead);
    bool    WriteData(const void *data, const size_t sizeToRead, size_t& writtenSize);

    ISeqInStream        m_streamIn;
    ISeqOutStream       m_streamOut;

    const std::string&      m_dataIn;
    std::string&            m_dataOut;

    UINT                    m_pointerIdxIn;

};


//class CStringOutStream
//{
//public:
//    CStringOutStream(std::string& data);
//    
//    std::string&         m_data;
//    //UINT              m_pointerIdx;
//
//    //void CreateVTable();
//    bool  WriteData(const void *data, const size_t sizeToRead, size_t& writtenSize);
//};
