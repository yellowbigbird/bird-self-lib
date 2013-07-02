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


static SRes Decode2(CLzmaDec *state, ISeqOutStream *outStream, ISeqInStream *inStream, UInt64 unpackSize);

static ISzAlloc g_Alloc = { SzAlloc, SzFree };
#define IN_BUF_SIZE (1 << 16)
#define OUT_BUF_SIZE (1 << 16)

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
bool CLzmaWrapper::Decode(const std::string& strSrc, std::string& strOut)
{
  UInt64 unpackSize;
  int i;
  SRes res = 0;

  CLzmaDec state;
  CStringOutStream outStream(strOut);
  CStringInStream inStream(strSrc);

  /* header: 5 bytes of LZMA properties and 8 bytes of uncompressed size */
  unsigned char header[LZMA_PROPS_SIZE + 8];

  /* Read and parse header */

  RINOK(SeqInStream_Read(&inStream.m_stream, header, sizeof(header)));

  unpackSize = 0;
  for (i = 0; i < 8; i++)
    unpackSize += (UInt64)header[LZMA_PROPS_SIZE + i] << (i * 8);

  LzmaDec_Construct(&state);
  RINOK(LzmaDec_Allocate(&state, header, LZMA_PROPS_SIZE, &g_Alloc));

  res = Decode2(&state, &outStream.m_stream, &inStream.m_stream, unpackSize);

  LzmaDec_Free(&state, &g_Alloc);
  return res;
}

SRes Decode2(CLzmaDec *state, ISeqOutStream *outStream, ISeqInStream *inStream, UInt64 unpackSize)
{
    int thereIsSize = (unpackSize != (UInt64)(Int64)-1);
    Byte inBuf[IN_BUF_SIZE];
    Byte outBuf[OUT_BUF_SIZE];
    size_t inPos = 0, inSize = 0, outPos = 0;
    LzmaDec_Init(state);
    for (;;)
    {
        if (inPos == inSize)
        {
            inSize = IN_BUF_SIZE;
            RINOK(inStream->Read(inStream, inBuf, &inSize));
            inPos = 0;
        }
        {
            SRes res;
            SizeT inProcessed = inSize - inPos;
            SizeT outProcessed = OUT_BUF_SIZE - outPos;
            ELzmaFinishMode finishMode = LZMA_FINISH_ANY;
            ELzmaStatus status;
            if (thereIsSize && outProcessed > unpackSize)
            {
                outProcessed = (SizeT)unpackSize;
                finishMode = LZMA_FINISH_END;
            }

            res = LzmaDec_DecodeToBuf(state, outBuf + outPos, &outProcessed,
                inBuf + inPos, &inProcessed, finishMode, &status);
            inPos += inProcessed;
            outPos += outProcessed;
            unpackSize -= outProcessed;

            if (outStream)
                if (outStream->Write(outStream, outBuf, outPos) != outPos)
                    return SZ_ERROR_WRITE;

            outPos = 0;

            if (res != SZ_OK || thereIsSize && unpackSize == 0)
                return res;

            if (inProcessed == 0 && outProcessed == 0)
            {
                if (thereIsSize || status != LZMA_STATUS_FINISHED_WITH_MARK)
                    return SZ_ERROR_DATA;
                return res;
            }
        }
    }
}

//////////////////