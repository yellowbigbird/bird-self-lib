#include "stdafx.h"
#include "dcdata.h"

#include "httpSocket.h"
#include "ossWrapper/ossWrapper.h"

#include "util/UtilString.h"
#include "util/UtilsFile.h"

#include <afxinet.h>

using namespace std;
////////////////////
CDcData::CDcData()
:m_pSocket(NULL)
,m_port(80)
,m_pOssWrapper(NULL)
{    
}

CDcData::~CDcData()
{
    SAFE_DELETE(m_pSocket);
    SAFE_DELETE(m_pOssWrapper);    
}

///////////////////////

bool CDcData::Init()
{
    if(!m_pSocket)
        m_pSocket = new CHttpSocket();
    if(!m_pSocket)
        return false;
	bool fok = m_pSocket->Socket();    
    if(!fok)
    {
        ASSERT(FALSE);
        return 0;
    }

	m_pSocket->SetTimeout(15,0);

    if(!m_pOssWrapper)
        m_pOssWrapper = new COssxWrapper();
    if(!m_pOssWrapper){
        ASSERT(false);
        return false;
    }
    //if(!m_pOssWrapper->Init() )
    //    return false;

    return true;
}

bool CDcData::LoadRequest(const CString& filePathName)
{
	if(filePathName.GetLength() < 1)
		return false;
	
	string strData;
	wstring wstrFile = filePathName.GetString();
	bool fok = UtilFile::ReadFileAsString(wstrFile, strData);
	if(fok){
		m_strRequest = strData;
	}
	return true;
}

void	CDcData::SetUrl(const std::string& strUrl)
{
    m_strUrl = strUrl;
    wstring wstr = UtilString::ConvertMultiByteToWideChar(strUrl);
    CString cstrUrl = wstr.c_str();

    m_strHost = "chellilisrc101";
    m_port = 8080;
    
    DWORD dwServiceType = 0;
    CString cstrObject;
    CString cstrServer;
    WORD port = 0;
    //BOOL AFXAPI AfxParseURL(LPCTSTR pstrURL, DWORD& dwServiceType,
	//CString& strServer, CString& strObject, INTERNET_PORT& nPort);
    if(AfxParseURL(cstrUrl, dwServiceType, cstrServer, cstrObject, port) ){
        m_port = port; 
        wstr = cstrServer;
        m_strHost = UtilString::ConvertWideCharToMultiByte(wstr);
    }
}

bool CDcData::SendRequest()
{
    if(!m_pSocket
        || !m_pOssWrapper
        || m_strHost.length() < 1
        || m_port < 1        
        )
        return false;

    CHttpSocket& rsocket = *m_pSocket;
    bool fok = rsocket.Connect(m_strHost, m_port);
    if(!fok)
        return false;

    string strFast;
    strFast.resize(m_strRequest.size() );
    fok = m_pOssWrapper->XmlToFastInfoSet(m_strRequest, strFast);

    rsocket.FormatRequestHeader(m_strHost, "");
    fok = rsocket.SendRequest();

    return fok;
}