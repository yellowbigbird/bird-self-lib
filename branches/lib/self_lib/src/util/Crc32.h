#pragma once
#include "selflib_dll.h"

class SELFLIB_DLL CCrc32
{
public:
    CCrc32();
    virtual ~CCrc32();
	    
    bool CalculateCRC32(const char* pBlock, const unsigned long long iInitialSize, unsigned int& crc32) const;
    bool GetCrc32FromFile(const char* fileName, unsigned int& crc32) const;

protected:
	void BuildCRCTable(void);

private:
    unsigned int    mCRC32Table[256]; //uint
    bool            mIfTableBuilt;
};
