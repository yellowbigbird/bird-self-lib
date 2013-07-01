// TestLzmaLib.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
//#include <Lzma2Enc.h>
#include "LzmaEnc.h"

#include "Alloc.h"
#include "7zFile.h"
#include "7zVersion.h"
#include "LzmaDec.h"
#include "LzmaEnc.h"

#include "lzmaWrapper.h"
#include "utilsFile.h"

using namespace std;
///////////////////
void TestLzma();
void TestUnzip();
void TestWrapper();

SRes Encode(ISeqOutStream *outStream, ISeqInStream *inStream, UInt64 fileSize, char *rs);
SRes Decode(ISeqOutStream *outStream, ISeqInStream *inStream);
SRes Decode2(CLzmaDec *state, ISeqOutStream *outStream, ISeqInStream *inStream,
             UInt64 unpackSize);

static void *SzAlloc(void *p, size_t size) { p = p; return MyAlloc(size); }
static void SzFree(void *p, void *address) { p = p; MyFree(address); }
static ISzAlloc g_Alloc = { SzAlloc, SzFree };

#define IN_BUF_SIZE (1 << 16)
#define OUT_BUF_SIZE (1 << 16)

//////////////

int _tmain(int argc, _TCHAR* argv[])
{
    TestLzma();
    TestUnzip();
    TestWrapper();

    return 0;
}


const TCHAR* c_strFileTxt = _T("d:\\wukong.txt");
const TCHAR* c_strFileBin = _T("d:\\wukong-lzma.bin");
const TCHAR* c_strFileDecode = _T("d:\\wukong-decode.txt");

void TestWrapper()
{
    CLzmaWrapper wrapper;
    //CLzma2Enc
    string strSrc;
    string strDest;
    string strDestDecode;

    bool ifok = false;
    ifok = UtilFile::ReadFileAsString(c_strFileTxt, strSrc);
    ifok = wrapper.Encode(strSrc, strDest);
    ifok = UtilFile::WriteFileAsString(c_strFileBin, strDest);

    ifok = wrapper.Decode(strDest, strDestDecode);
    ifok = UtilFile::WriteFileAsString(c_strFileBin, strDest);

    int i=0;
    i++;
}


void TestLzma()
{  
    char c = 'e';
    int res;
    int encodeMode = 1;
    Bool useOutFile = False;
    char* strFile = "d:\\wukong.txt";
    char* strFileOut = "d:\\wukong-lzma.bin";
    char* strFileOutUnzip = "d:\\wukong-unzip.txt";
    char rs[800] = { 0 };

    CFileSeqInStream inStream;  
    FileSeqInStream_CreateVTable(&inStream);
    File_Construct(&inStream.file);

    CFileOutStream outStream;
    FileOutStream_CreateVTable(&outStream);
    File_Construct(&outStream.file);

    {
        size_t t4 = sizeof(UInt32);
        size_t t8 = sizeof(UInt64);
        //if (t4 != 4 || t8 != 8) ;
        //return PrintError(rs, "Incorrect UInt32 or UInt64");
    }

    if (InFile_Open(&inStream.file, strFile) != 0)
        ;
    //return PrintError(rs, "Can not open input file");

    useOutFile = True;
    if (OutFile_Open(&outStream.file, strFileOut) != 0)
        ;
    //return PrintError(rs, "Can not open output file");

    if (encodeMode)
    {
        UInt64 fileSize;
        File_GetLength(&inStream.file, &fileSize);
        res = Encode(&outStream.s, &inStream.s, fileSize, rs);
    }
    else
    {
        //res = Decode(&outStream.s, useOutFile ? &inStream.s : NULL);
    }
    if (useOutFile)
        File_Close(&outStream.file);
    File_Close(&inStream.file);

}

void TestUnzip()
{
    char c = 'e';
    int res;
    int encodeMode = 1;
    Bool useOutFile = False;
    char* strFile = "d:\\wukong.txt";
    char* strFileOut = "d:\\wukong-lzma.bin";
    char* strFileOutUnzip = "d:\\wukong-unzip.txt";
    char rs[800] = { 0 };

    CFileSeqInStream inStream;  
    FileSeqInStream_CreateVTable(&inStream);
    File_Construct(&inStream.file);

    CFileOutStream outStream;
    FileOutStream_CreateVTable(&outStream);
    File_Construct(&outStream.file);

    {
        size_t t4 = sizeof(UInt32);
        size_t t8 = sizeof(UInt64);
        if (t4 != 4 || t8 != 8) ;
        //return PrintError(rs, "Incorrect UInt32 or UInt64");
    }

    if (InFile_Open(&inStream.file, strFileOut) != 0)
        ;
    //return PrintError(rs, "Can not open input file");

    useOutFile = True;
    if (OutFile_Open(&outStream.file, strFileOutUnzip) != 0)
        ;
    //return PrintError(rs, "Can not open output file");

    if (0)
    {
        //UInt64 fileSize;
        //File_GetLength(&inStream.file, &fileSize);
        //res = Encode(&outStream.s, &inStream.s, fileSize, rs);
    }
    else
    {
        res = Decode(&outStream.s, useOutFile ? &inStream.s : NULL);
    }

    if (useOutFile)
        File_Close(&outStream.file);
    File_Close(&inStream.file);
}

SRes Encode(ISeqOutStream *outStream, ISeqInStream *inStream, UInt64 fileSize, char *rs)
{
    CLzmaEncHandle enc;
    SRes res;
    CLzmaEncProps props;

    //rs = rs;

    enc = LzmaEnc_Create(&g_Alloc);
    if (enc == 0)
        return SZ_ERROR_MEM;

    LzmaEncProps_Init(&props);
    res = LzmaEnc_SetProps(enc, &props);

    if (res == SZ_OK)
    {
        Byte header[LZMA_PROPS_SIZE + 8];
        size_t headerSize = LZMA_PROPS_SIZE;
        int i;

        res = LzmaEnc_WriteProperties(enc, header, &headerSize);
        for (i = 0; i < 8; i++)
            header[headerSize++] = (Byte)(fileSize >> (8 * i));
        if (outStream->Write(outStream, header, headerSize) != headerSize)
            res = SZ_ERROR_WRITE;
        else
        {
            if (res == SZ_OK)
                res = LzmaEnc_Encode(enc, outStream, inStream, NULL, &g_Alloc, &g_Alloc);
        }
    }
    LzmaEnc_Destroy(enc, &g_Alloc, &g_Alloc);
    return res;
}

SRes Decode(ISeqOutStream *outStream, ISeqInStream *inStream)
{
    UInt64 unpackSize;
    int i;
    SRes res = 0;

    CLzmaDec state;

    /* header: 5 bytes of LZMA properties and 8 bytes of uncompressed size */
    unsigned char header[LZMA_PROPS_SIZE + 8];

    /* Read and parse header */

    RINOK(SeqInStream_Read(inStream, header, sizeof(header)));

    unpackSize = 0;
    for (i = 0; i < 8; i++)
        unpackSize += (UInt64)header[LZMA_PROPS_SIZE + i] << (i * 8);

    LzmaDec_Construct(&state);
    RINOK(LzmaDec_Allocate(&state, header, LZMA_PROPS_SIZE, &g_Alloc));
    res = Decode2(&state, outStream, inStream, unpackSize);
    LzmaDec_Free(&state, &g_Alloc);
    return res;
}

SRes Decode2(CLzmaDec *state, ISeqOutStream *outStream, ISeqInStream *inStream,
             UInt64 unpackSize)
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