#pragma once

#include <string>

class CInternetSession;
class CHttpConnection;

class CMfcHttp
{
public:
    CMfcHttp();
    virtual ~CMfcHttp();

    bool        Connect(const std::wstring& strUrl);

    bool		SendRequest();
    bool		SendRequestFastInfoSet(const std::string& strData);

    bool        GetContent(std::string&  strData) const;

    bool        FormatRequestHeaderSoap( const std::string& strServer
        ,const std::string& pObject
        ,const std::string& strData );

protected:
    CInternetSession*       m_pSession;
    CHttpConnection*        m_pConnection;

    //std::string     m_requestheader;
    std::string     m_ResponseHeader;
    std::string     m_strContent;

    CString         m_strServer;
    CString         m_strObject;
    CString         m_strUrl;
    CString         m_strRequestHeaderAdd;
    

    WORD            m_port;
};