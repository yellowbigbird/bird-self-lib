#pragma once
#include "dlldefine.h"

struct ossxEnv;
struct ossxMemoryHandle;

class OSS_DLL COssxWrapper
{
public:
    COssxWrapper();
    virtual ~COssxWrapper();

    //bool    Init();

    bool    XmlToFastInfoSet(const std::string& strRawXml,  std::string & strRet);
    bool    FastInfoSetToXml(const std::string& strFast,  std::string & strRawXml);

protected:
    ossxEnv*            m_pEnv;
//    ossxMemoryHandle*	m_hOssMem;
//    OssxBufExtended*    m_pBufExtend;
};


