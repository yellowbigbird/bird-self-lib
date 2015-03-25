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

    bool    XmlToFastInfoSet(const std::string& strRawXml);    
    bool    FastInfoSetToXml(const std::string& strFast);

    const std::string&      GetStrFastInfoSet() const;
    const std::string&      GetStrRawXml()      const;

    void    Clear();

protected:
    ossxEnv*            m_pEnv;
    std::string         m_strFastInfoSet;
    std::string         m_strRawXml;
//    ossxMemoryHandle*	m_hOssMem;
//    OssxBufExtended*    m_pBufExtend;
};


