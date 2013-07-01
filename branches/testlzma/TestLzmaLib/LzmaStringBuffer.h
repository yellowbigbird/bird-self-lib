
#pragma once

#include <vector>
#include <string>

#include "types.h"

class CStringInStream
{
public:
    CStringInStream(const std::string& data);
    ISeqInStream      m_stream;
    const std::string&      m_data;
    UINT              m_pointerIdx;

    void CreateVTable();
    bool ReadData(void *data, const size_t sizeToRead, size_t& sizeHasRead);
};

/* writes *size bytes */
//WRes Buffer_Write(void *p, const void *data, size_t *size);

//void CStringInStream_CreateVTable(CStringInStream& rstream);

class CStringOutStream
{
public:
    CStringOutStream(std::string& data);
    ISeqOutStream       m_stream;
    std::string&         m_data;
    //UINT              m_pointerIdx;

    void CreateVTable();
    bool  WriteData(const void *data, const size_t sizeToRead, size_t& writtenSize);
};
