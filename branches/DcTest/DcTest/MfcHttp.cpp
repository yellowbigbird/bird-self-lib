#include "stdafx.h"
#include "MfcHttp.h"

#include <afxinet.h>

using namespace std;
//////////////////////////////////////////////////////////////////////////

CMfcHttp::CMfcHttp()
:m_pSession(NULL)
,m_pConnection(NULL)
,m_port(80)
{
    m_pSession = new CInternetSession();
    ASSERT(m_pSession);
    if(!m_pSession)
        return;
    CInternetSession& rSession = *m_pSession;
    rSession.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 1000 * 20);
    rSession.SetOption(INTERNET_OPTION_CONNECT_BACKOFF, 1000);
    rSession.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 1);
}

CMfcHttp::~CMfcHttp()
{
    if(m_pConnection)
        m_pConnection->Close();
    if(m_pSession)
        m_pSession->Close();
    SAFE_DELETE(m_pConnection);
    SAFE_DELETE(m_pSession);
    //myfile=0;
    //myconn=0;
}

//////////////////////////////////////////////////////////////////////////

bool  CMfcHttp::Connect(const std::wstring& strUrl)
{
    if(!m_pSession)
        return false;

    WORD port = 8080;
    
    try
    {
        CString cstrObject;
        CString cstrServer;
        DWORD dwServiceType;

        m_strUrl = strUrl.c_str();
        if(AfxParseURL(strUrl.c_str(), dwServiceType, cstrServer, cstrObject, port) ){
            m_port = port; 
            m_strServer = cstrServer;
            //m_strObject = UtilString::ConvertWideCharToMultiByte(cstrObject.GetString() );
        }
        else{
        }

        //each time must clear
        m_strRequestHeaderAdd.Empty();

        m_pConnection = m_pSession->GetHttpConnection(m_strServer, (INTERNET_PORT)port);
        if(!m_pConnection)
            return false;
    }
    catch (CException* )
    {
    }
    
    return true;
}

bool CMfcHttp::SendRequest()
{
    if(!m_pConnection)
        return false;
    //OpenRequest( CHttpConnection::HTTP_VERB_GET,    TEXT("/Practice/index.jsp"))

    CHttpFile * pFile = m_pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/") );
    if(!pFile)
        return false;
    CHttpFile& rFile = *pFile;

    CString szHeaders = L"Accept: text/htm\r\n";
    pFile->AddRequestHeaders(szHeaders);

    try
    {
        rFile.SendRequest();        
    }
    catch (CException* )
    {
    }

    DWORD dwRet;
    rFile.QueryInfoStatusCode(dwRet);

    if(dwRet != HTTP_STATUS_OK)
    {
        CString errText;
        errText.Format(L"POST出错，错误码：%d", dwRet);
        //AfxMessageBox(errText);
    }
    else
    {
        //int len = rFile.GetLength();       
        //while ((numread = pFile->Read(buf,sizeof(buf)-1)) > 0)
       
    }
    rFile.Close();

    //("GET","/index.html");
    return true;
}

bool CMfcHttp::SendRequestFastInfoSet(const string& strData)
{
    if(!m_pConnection)
        return false;

    bool fok = true;
    CHttpFile * pFile = m_pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, _T("/") );
    if(!pFile)
        return false;
    CHttpFile& rFile = *pFile;

    //CString szHeaders = L"Accept: text/htm\r\n";
    pFile->AddRequestHeaders(m_strRequestHeaderAdd);
    CString cstrHeader;

    try {
        //rFile.SendRequest(cstrHeader, (LPVOID)&strData[0], (DWORD)strData.size() );   
        pFile->SendRequestEx((DWORD)strData.size());
        pFile->Write(&strData[0], (UINT)strData.size());
        pFile->EndRequest();
    }
    catch (CException* )    {
        fok = false;
    }

    DWORD dwRet;
    rFile.QueryInfoStatusCode(dwRet);

    if(dwRet == HTTP_STATUS_OK)    {
        UINT64 datalen = rFile.GetLength();
        m_strContent.clear();
        m_strContent.resize(datalen);
        UINT readlen = rFile.Read(&m_strContent[0], (UINT)datalen);
        if(readlen != datalen){
            ASSERT(false);
            fok = false;
        }
    }
    else    {
        //err
        ASSERT(false);
        fok = false;
    }
    rFile.Close();

    return fok;
}

bool CMfcHttp::GetContent(std::string&  strData) const
{
    strData = m_strContent;
    return true;
}

bool  CMfcHttp::FormatRequestHeaderSoap(const std::string& strData  )
{
    m_strRequestHeaderAdd.Empty();

    const CString strRet = _T("\r\n");

    //User-Agent
    m_strRequestHeaderAdd += "User-Agent:Mozilla/4.0 (compatible; MSIE 5.00; Windows 98)" ; //
    m_strRequestHeaderAdd += strRet;

    //SOAPAction
    m_strRequestHeaderAdd += "SOAPAction: \"\"";  ///
    m_strRequestHeaderAdd += strRet;

    //contenttype
    m_strRequestHeaderAdd += "Content-Type: application/fastinfoset";  
    m_strRequestHeaderAdd += strRet;

    //host
    //m_strRequestHeaderAdd += "Host: ";
    //m_strRequestHeaderAdd += strServer;
    //m_strRequestHeaderAdd += strRet;


    //Content-Length: 3921
    char carray[10];
    memset(carray, 0, 10);
    sprintf_s(carray,10, "%d", strData.size() );
    m_strRequestHeaderAdd += "Content-Length: ";
    m_strRequestHeaderAdd += carray;
    m_strRequestHeaderAdd += strRet;

    ///第6行:连接设置,保持
    m_strRequestHeaderAdd += "Connection:Keep-Alive";
    m_strRequestHeaderAdd += strRet;

    return true;
}








