#include "stdafx.h"
#include "lzmaWrapper.h"

#include <vector>
#include <string>

#include "Types.h"
#include "LzmaDec.h"
#include "LzmaEnc.h"
#include "Alloc.h"

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

    if (res == SZ_OK)
    {
        Byte header[LZMA_PROPS_SIZE + 8];
        size_t headerSize = LZMA_PROPS_SIZE;
        int i;

        res = LzmaEnc_WriteProperties(enc, header, &headerSize);
        for (i = 0; i < 8; i++)
            header[headerSize++] = (Byte)(srcSize >> (8 * i));

        //if (outStream->Write(outStream, header, headerSize) != headerSize)
            //res = SZ_ERROR_WRITE;
        strOut.append((char*)header);
        res = LzmaEnc_Encode(enc, outStream, inStream, NULL, &g_Alloc, &g_Alloc);
    }
    LzmaEnc_Destroy(enc, &g_Alloc, &g_Alloc);

    return ifok;
}

////////////////