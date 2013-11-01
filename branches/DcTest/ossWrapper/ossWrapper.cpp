// ossWrapper.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ossWrapper.h"

#include <assert.h>

//#include <ossfws.h>
#include <ossxsd.h>
#include <ossxtype.h>

using namespace std;

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

typedef unsigned char byte;
//////////////////////////////////////////////////////////////////////////

COssxWrapper::COssxWrapper()
:m_pEnv(NULL)
{
    m_pEnv = new OssxEnv;
    int ret = ::ossxInitFastInfoset(m_pEnv, &OSSX_FI_CTL_TBL);
    assert(0 == ret);
}

COssxWrapper::~COssxWrapper()
{
    //    ossxterm(m_pEnv);
    ::ossxTermFastInfoset(m_pEnv);
    SAFE_DELETE(m_pEnv);
}

void  COssxWrapper::Clear()
{
    m_strFastInfoSet.clear();
    m_strRawXml.clear();
}

bool COssxWrapper::XmlToFastInfoSet(const std::string& strRawXml)
{
    if(strRawXml.length()< 1
        || !m_pEnv)
        return false;

    OssxBuf inBuffer;
    OssxBuf outBuffer;

    inBuffer.length = (long)strRawXml.size();
    inBuffer.value = (byte*)&strRawXml[0];
    
    outBuffer.length = 0;
    outBuffer.value = NULL;
    bool fok = true;
    int ret = ::ossxXML2FastInfoset(m_pEnv, &inBuffer, &outBuffer);
    if(0 != ret){
        assert(false);
        return false;
    }

    m_strFastInfoSet.resize(outBuffer.length);
    if(m_strFastInfoSet.size() != outBuffer.length){
        assert(false);
        return false;
    }
    memcpy_s(&m_strFastInfoSet[0], outBuffer.length, outBuffer.value, outBuffer.length);
    //strFastInfoSet = (char*)outBuffer.value;
    ::ossxFreeBuf(m_pEnv, outBuffer.value);
    
    return fok ;
}

bool    COssxWrapper::FastInfoSetToXml(const std::string& strFast)
{
    if(strFast.length()< 1
        || !m_pEnv)
        return false;

    int ret= 0;

    OssxBuf inBuffer;
    OssxBuf outBuffer;

    inBuffer.length = (long)strFast.length();
    inBuffer.value =  (byte*)&strFast[0];
    
    outBuffer.length = 0;
    outBuffer.value = NULL;
    ret = ::ossxFastInfoset2XML(m_pEnv, &inBuffer, &outBuffer);

    //m_strRawXml = (char*)outBuffer.value;
    m_strRawXml.resize(outBuffer.length);
    if(m_strRawXml.size() != outBuffer.length){
        assert(false);
        return false;
    }
    memcpy_s(&m_strRawXml[0], outBuffer.length, outBuffer.value, outBuffer.length);
    
    ::ossxFreeBuf(m_pEnv, outBuffer.value);

    bool fok = (ret==0);
    return fok ;
}

const std::string&   COssxWrapper::GetStrFastInfoSet() const
{
    return m_strFastInfoSet;
}

const std::string&   COssxWrapper::GetStrRawXml()      const
{
    return m_strRawXml;
}