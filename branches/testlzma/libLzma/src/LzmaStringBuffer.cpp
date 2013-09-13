//#include "stdafx.h"
#include "lzmastringbuffer.h"

using namespace std;

static SRes BufferSeqInStream_Read(void *pp, void *buf, size_t *size);
static size_t BufferOutStream_Write(void *pp, const void *data, size_t size);
//////////////////

CStringStreamIn::CStringStreamIn(const std::string& dataIn )
:m_dataIn(dataIn)
//,m_dataOut(dataOut)
,m_pointerIdxIn(0)
{
    m_streamIn.Read = BufferSeqInStream_Read;
    //m_streamOut.Write = BufferOutStream_Write;
    //CreateVTable();
}

CStringStreamOut::CStringStreamOut(std::string& dataOut)
//:m_dataIn(dataIn)
:m_dataOut(dataOut)
//,m_pointerIdxIn(0)
{
    //m_streamIn.Read = BufferSeqInStream_Read;
    m_streamOut.Write = BufferOutStream_Write;
    //CreateVTable();
}

static SRes BufferSeqInStream_Read(void *pp, void *buf, size_t *size)
{
    CStringStreamIn *pstream = (CStringStreamIn *)pp;
    size_t sizeToRead = *size;
    bool ifok = pstream->ReadData(buf, sizeToRead, *size );
    SRes ret =  (ifok) ? SZ_OK : SZ_ERROR_READ;
    return ret;
}

static size_t BufferOutStream_Write(void *pp, const void *data, size_t size)
{
  CStringStreamOut *pEntry = (CStringStreamOut *)pp;
  if(!pEntry)
      return 0;
  size_t writtenSize = 0;
  bool ifok = pEntry->WriteData( data, size , writtenSize);
  return size;
}

bool CStringStreamIn::ReadData(void *data, const size_t sizeToRead0, size_t& sizeHasRead)
{
    const size_t alllen = m_dataIn.size();
    size_t sizeToRead1 = sizeToRead0;
    if(m_pointerIdxIn+ sizeToRead0 > alllen){
        sizeToRead1 = alllen - m_pointerIdxIn;
    }
    const char* pstr =  m_dataIn.data();
    if(sizeToRead1)
        memcpy(data,pstr + m_pointerIdxIn, sizeToRead1);
    m_pointerIdxIn += sizeToRead1;
    sizeHasRead = sizeToRead1;
    return true;
}
/////////////


bool CStringStreamOut::WriteData(const void *dataSrc, const size_t sizeToWrite, size_t& writtenSize)
{
    if(!dataSrc)
        return false;
    bool ifok= false;
    const size_t pointerStart = m_dataOut.size();
    const char* chardata = (const char*)dataSrc;
    for(size_t idx= 0; idx< sizeToWrite; idx++)
    {
        m_dataOut.push_back(chardata[idx]);  //todo, need change to memcpy
    }
    //check
    writtenSize = m_dataOut.size() - pointerStart;
    ifok = true;
    if(writtenSize != sizeToWrite)
        ifok = false;
    return ifok;
}