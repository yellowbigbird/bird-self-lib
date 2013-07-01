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
    size_t sizeToRead = *size;
    bool ifok = pstream->ReadData(buf, sizeToRead, *size );
    SRes ret =  (ifok) ? SZ_OK : SZ_ERROR_READ;
    return ret;
}

void CStringInStream::CreateVTable()
{
    m_stream.Read = BufferSeqInStream_Read;
    m_pointerIdx = 0;
}

bool CStringInStream::ReadData(void *data, const size_t sizeToRead0, size_t& sizeHasRead)
{
    const UINT alllen = m_data.size();
    size_t sizeToRead1 = sizeToRead0;
    if(m_pointerIdx+ sizeToRead0 > alllen){
        sizeToRead1 = alllen - m_pointerIdx;
    }
    const char* pstr =  m_data.data();
    memcpy(data,pstr + m_pointerIdx, sizeToRead1);
    m_pointerIdx += sizeToRead1;
    sizeHasRead = sizeToRead1;
    return true;
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
    ifok = true;
    if(writtenSize != sizeToWrite)
        ifok = false;
    return ifok;
}