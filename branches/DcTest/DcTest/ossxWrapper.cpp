#include "stdafx.h"
#include "ossxWrapper.h"

#include <ossfws.h>
#include <ossxsd.h>
#include <ossxtype.h>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
/////////////////
COssxWrapper::COssxWrapper()
:m_pEnv(NULL)
,m_hOssMem(NULL)
//,m_pBufExtend(NULL)
{
}

COssxWrapper::~COssxWrapper()
{ 
    //SAFE_DELETE(m_pBufExtend);

    ossxDeleteMemoryHandle(m_pEnv, m_hOssMem);
    m_hOssMem = NULL;
    ossxterm(m_pEnv);
    SAFE_DELETE(m_pEnv);
}

bool COssxWrapper::Init()
{
    if(!m_pEnv)
        m_pEnv = new ossxEnv; //warning C4345
    if(!m_pEnv)
        return false;

    void* pCtlTbl = NULL;

    int retcode = ossxInitSoap(m_pEnv, pCtlTbl, NULL, NULL );
    if(retcode){    ASSERT(false);        return false; }
    //OSSEnforce(retcode, MSG_HOTEL_OSS_MAPPING_OSS_INIT_FAILURE);

    DWORD const dwMemChunkLength = 4 * 1024 * 1024;
    m_hOssMem = ossxCreateMemoryHandle(m_pEnv, dwMemChunkLength);

    //set buffer
    OssxBuf encodedData; 
    if(!m_pBufExtend)
        m_pBufExtend = new(OssxBufExtended); 
    if(!m_pBufExtend)
        return false;
    OssxBufExtended& initialBuf = *m_pBufExtend;

    int myGlobElemNum = 1;//MyElement_GlobElem; 
    //MyElement *decodedDataPtr = 0; 
    const int INITIAL_DATA_SIZE = 4000;
    unsigned char preEncodedData[INITIAL_DATA_SIZE]= {0}; 

    initialBuf.value = (byte*)(m_pEnv->mallocp)(INITIAL_DATA_SIZE); 
    memcpy(initialBuf.value, preEncodedData, INITIAL_DATA_SIZE); 
    initialBuf.length = INITIAL_DATA_SIZE;  /* size of initial buffer */ 
    initialBuf.byteOffset = 0; /* offset in buffer from where data starts */ 

    encodedData.length = 0; 
    //encodedData.value = (byte*)ossxMarkObj(&m_OSSEnvSoap, OSSX_FILE, "decoder.in");  //OSSX_MEMORY
    encodedData.value = (byte*)ossxMarkObj(m_pEnv, OSSX_MEMORY, "decoder.in");
    ossxSetBuffer(m_pEnv, &initialBuf);

    return true;
}

