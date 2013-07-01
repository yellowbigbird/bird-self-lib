#include "stdafx.h"
#include "lzmastringbuffer.h"

using namespace std;

//////////////////

CStringInStream::CStringInStream(const std::string& data)
:m_data(data)
{
    CreateVTable();
}

static SRes BufferSeqInStream_Read(void *pp, void *buf, size_t *size)
{
    CStringInStream *pstream = (CStringInStream *)pp;
    WRes wret = pstream->ReadData(buf, *size);
    SRes ret =  (wret == 0) ? SZ_OK : SZ_ERROR_READ;
    return ret;
}

void CStringInStream::CreateVTable()
{
    m_stream.Read = BufferSeqInStream_Read;
    m_pointerIdx = 0;
}

WRes CStringInStream::ReadData(void *data, size_t sizeToRead)
{
    const UINT alllen = m_data.size();
    if(m_pointerIdx+ sizeToRead > alllen)
        return 1;
    const char* pstr =  m_data.data();
    memcpy(data,pstr + m_pointerIdx, sizeToRead);
    return 0;
}
/////////////

static size_t BufferOutStream_Write(void *pp, const void *data, size_t size)
{
  CStringOutStream *pEntry = (CStringOutStream *)pp;
  if(!pEntry)
      return 0;
  size_t writtenSize = 0;
  bool ifok = pEntry->WriteData( data, size , writtenSize);
  return size;
}

CStringOutStream::CStringOutStream(std::string& data)
:m_data(data)
{
    CreateVTable();
}

void CStringOutStream::CreateVTable()
{
    m_stream.Write = BufferOutStream_Write;
    //m_pointerIdx = 0;
}

bool CStringOutStream::WriteData(const void *dataSrc, const size_t sizeToWrite, size_t& writtenSize)
{
    bool ifok= false;
    size_t pointerStart = m_data.size()-1;
    const char* chardata = (const char*)dataSrc;
    for(size_t idx= 0; idx< sizeToWrite; idx++)
    {
        m_data.push_back(chardata[idx]);
    }
    writtenSize = m_data.size()-1 - pointerStart;
    return ifok;
}