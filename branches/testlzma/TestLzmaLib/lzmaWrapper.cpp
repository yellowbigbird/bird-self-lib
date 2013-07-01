#include "stdafx.h"
#include "lzmaWrapper.h"

#include <vector>
#include <string>

#include "Types.h"
#include "LzmaDec.h"
#include "LzmaEnc.h"
#include "Alloc.h"

#include "lzmaStringBuffer.h"

/////////////////////////
using namespace std;

static void *SzAlloc(void *p, size_t size) { p = p; return MyAlloc(size); }
static void SzFree(void *p, void *address) { p = p; MyFree(address); }
static ISzAlloc g_Alloc = { SzAlloc, SzFree };

///////////////
CLzmaWrapper::CLzmaWrapper()
{
}

CLzmaWrapper::~CLzmaWrapper()
{
}

/////////////////
bool CLzmaWrapper::Encode(const std::string& strSrc, std::string& strOut)
{
    bool ifok = false;
    strOut = "";
    const UINT srcSize = strSrc.size();

    CLzmaEncHandle enc;
    SRes res;
    CLzmaEncProps props;

    //rs = rs;

    enc = LzmaEnc_Create(&g_Alloc);
    if (enc == 0)
        return false;

    LzmaEncProps_Init(&props);
    res = LzmaEnc_SetProps(enc, &props);

    CStringOutStream outStream(strOut);
    CStringInStream inStream(strSrc);

    if (res == SZ_OK)
    {
        Byte header[LZMA_PROPS_SIZE + 8];
        size_t headerSize = LZMA_PROPS_SIZE;
        int i;

        res = LzmaEnc_WriteProperties(enc, header, &headerSize);
        for (i = 0; i < 8; i++)
            header[headerSize++] = (Byte)(srcSize >> (8 * i));

        size_t wrritenSize = 0;
        ifok = outStream.WriteData( header, headerSize,wrritenSize);
        if(!ifok){
            res = SZ_ERROR_WRITE;
            return false;
        }
        res = LzmaEnc_Encode(enc, &outStream.m_stream, &inStream.m_stream, NULL, &g_Alloc, &g_Alloc);
        if(res)
            ifok = false;
    }
    LzmaEnc_Destroy(enc, &g_Alloc, &g_Alloc);

    //strOut = outStream.m_data;
    return ifok;
}

////////////////

//SRes CLzmaWrapper::Decode(ISeqOutStream *outStream, ISeqInStream *inStream)
//bool CLzmaWrapper::Decode(const std::string& strSrc, std::string& strOut)
//{
//  UInt64 unpackSize;
//  int i;
//  SRes res = 0;
//
//  CLzmaDec state;
//
//  /* header: 5 bytes of LZMA properties and 8 bytes of uncompressed size */
//  unsigned char header[LZMA_PROPS_SIZE + 8];
//
//  /* Read and parse header */
//
//  RINOK(SeqInStream_Read(inStream, header, sizeof(header)));
//
//  unpackSize = 0;
//  for (i = 0; i < 8; i++)
//    unpackSize += (UInt64)header[LZMA_PROPS_SIZE + i] << (i * 8);
//
//  LzmaDec_Construct(&state);
//  RINOK(LzmaDec_Allocate(&state, header, LZMA_PROPS_SIZE, &g_Alloc));
//  res = Decode2(&state, outStream, inStream, unpackSize);
//  LzmaDec_Free(&state, &g_Alloc);
//  return res;
//}

//////////////////