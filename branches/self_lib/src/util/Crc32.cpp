
#include "stdafx.h"
#include "crc32.h"

#include <stdio.h>
//#include <windows.h>

#ifdef UINT
#else
typedef unsigned char byte;
typedef unsigned int        UINT;
typedef unsigned int        UINT32;
typedef unsigned __int64    ULONGLONG;
#endif

//////////////////////////////////////////////////////////////////////////
#define FIVE_MB_BUF_SIZE    0xfffff
#define CRC32_POLYNOMIAL    0xEDB88320L

//extern bool bUseMemoryMappedFiles;

//int iTotalFiles;
//int iFileNum;

CCrc32::CCrc32()
:mIfTableBuilt(false)
{
    BuildCRCTable();
}

CCrc32::~CCrc32()
{
}
/*
*******************************************************************
* BUILDCRCTABLE *
*******************************************************************
*/

// for CRC-32 via table lookup (faster than calc)
void CCrc32::BuildCRCTable(void)
{
    int i;
    int j;
    unsigned long crc;

    for (i=0; i<=255; i++)
    {
        crc=i;
        for ( j=8; j>0; j-- )
        {
            if ( crc & 1 )
                crc= ( crc>> 1 ) ^ CRC32_POLYNOMIAL;
            else
                crc >>= 1;
        }
        mCRC32Table[ i ] = crc;
    }

    mIfTableBuilt = true;

    return;
}

/*
**************************************************************************
* Routine calculates the CRC for a block of data using table lookup method
*
* Returns: true if success, false if file / memory mapped file access error
* (can happen if CD is actually corrupt with "data error (crc)"
**************************************************************************
*/
// for CRC-32 via table lookup (faster than calc)
//const byte* pBlock, const ULONGLONG iInitialSize, UINT32& crc32, const bool bInitCRC
bool CCrc32::CalculateCRC32(const char* pBlock, const unsigned long long iInitialSize, UINT& crc32) const
{
    if(!pBlock)
        return false;

    unsigned char *ppp = 0;
    UINT32 temp1 = 0;
    UINT32 temp2 = 0;
    bool retval = false;

    ULONGLONG iBlockSize = iInitialSize;
    ULONGLONG iBytesRemaining = iInitialSize;
    ULONGLONG iBlockCount = 0;

    if(iBlockSize > FIVE_MB_BUF_SIZE)
        iBlockSize = FIVE_MB_BUF_SIZE; // for large memory mapped files, this makes the status

    // message print from here. If memory map failed (e.g. on a
    // 2 GB file), the calling routine will print the status
    // message as it reads the file in 5 MB chunks.

    //if(bInitCRC)
    //    crc32 = 0xFFFFFFFFL;

    if(!mIfTableBuilt)
        return false;

    ppp = (unsigned char * ) pBlock;
    try
    {

        while(iBytesRemaining > 0)
        {
            iBlockCount = iBlockSize;
            while ( iBlockCount-- != 0 ) // optimize this loop for speed
            {
                // This does the magic crc32 calculation
                temp1 = (crc32 >> 8) & 0x00FFFFFFL;
                temp2 = mCRC32Table[ ( (int) crc32 ^ *ppp++ ) & 0xff ];
                crc32 = temp1 ^ temp2;
            }

            // BEWARE: unsigned iBlockCount will be 0xFFFFFFFFFFFFFFFF after this loop!
            iBytesRemaining -= iBlockSize;

            if(iBytesRemaining > 0)
            {
                printf("Megabytes checked so far (memmapped): %I64d / %I64d, file %d of %d\r",
                    (iInitialSize - iBytesRemaining) / (1024*1024),
                    iInitialSize / (1024*1024),
                    1, 1);
#ifdef _WINDOWS
                Sleep(1); // give the rest of the GUI a chance
#endif
            }

            // Else let calling routine print status message for large non-memory mapped files.
            // Small files get no status message.

            if(iBlockSize > iBytesRemaining)
                iBlockSize = iBytesRemaining; // for next iteration, if any
        }

        retval = true; // success
    }
    catch(...)
    {
        // The pragma was added above for the catch block of CalculateCRC32. The compiler incorrectly claims
        // this code is unreachable, but the debugger can get there.
        // #pragma warning(disable : 4702) // for "warning C4702: unreachable code"

        retval = false; // failed - probably corrupt file (e.g. Windows would report

        // "data error (cyclic redundancy check)" when copying the file)
    }

    return(retval);
}

bool CCrc32::GetCrc32FromFile(const char* fileName, unsigned int& crc32) const
{
    //assert(fileName);
    if(!fileName)
        return false;

    //FILE* pFile = fopen(fileName, "rb");
    FILE* pFile = 0;
    const errno_t errt = fopen_s(&pFile, fileName, "rb");
    if(!pFile)
        return false;

    //read file size
    fseek(pFile, 0, SEEK_END);
    const UINT fileSize = ftell(pFile);

    char* buffer = new char[fileSize];
    if(!buffer)
        return false;

    //read to buffer
    fseek(pFile, 0, SEEK_SET);
    const UINT readNum = (UINT)fread(buffer, fileSize, 1, pFile);
    if(!readNum)
        return false;
    
    fclose(pFile);

    const bool ifok = CalculateCRC32(buffer, fileSize, crc32);

    delete[] buffer;
    return ifok;
}

//////////////////////////////////////////////////////////////////////////




