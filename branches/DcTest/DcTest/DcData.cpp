#include "stdafx.h"
#include "dcdata.h"

//#include "httpSocket.h"
#include "MfcHttp.h"
#include "ossWrapper/ossWrapper.h"

#include "util/UtilString.h"
#include "util/UtilsFile.h"



using namespace std;
////////////////////
CDcData::CDcData()
:m_pSocket(NULL)
//,m_port(80)
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
        m_pSocket = new CMfcHttp(); //CHttpSocket
    if(!m_pSocket)
        return false;
	
	//m_pSocket->SetTimeout(15,0);

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

bool	CDcData::SetUrl(const std::string& strUrl)
{
    if(!m_pSocket   )
        return false;
    CMfcHttp& rsocket = *m_pSocket;

    m_strUrl = strUrl;
    wstring wstrUrl = UtilString::ConvertMultiByteToWideChar(strUrl);
    //CString cstrUrl = wstr.c_str();

    bool fok = rsocket.Connect(wstrUrl);
    return fok;

 //   m_strHost = "chellilisrc101";
 //   m_port = 8080;
 //   
 //   DWORD dwServiceType = 0;
 //   CString cstrObject;
 //   CString cstrServer;
 //   WORD port = 0;
 //   //BOOL AFXAPI AfxParseURL(LPCTSTR pstrURL, DWORD& dwServiceType,
	////CString& strServer, CString& strObject, INTERNET_PORT& nPort);
 //   if(AfxParseURL(cstrUrl, dwServiceType, cstrServer, cstrObject, port) ){
 //       m_port = port; 
 //       wstr = cstrServer;
 //       m_strHost = UtilString::ConvertWideCharToMultiByte(wstr);
 //       m_strObject = UtilString::ConvertWideCharToMultiByte(cstrObject.GetString() );
 //   }
}

bool CDcData::SendRequest()
{
    if(!m_pSocket
        || !m_pOssWrapper
        || m_strUrl.length() < 1
        //|| m_strHost.length() < 1
        //|| m_port < 1        
        )
        return false;

    CMfcHttp& rsocket = *m_pSocket;
    wstring wstrUrl = UtilString::ConvertMultiByteToWideChar(m_strUrl);
    bool fok = rsocket.Connect(wstrUrl); //m_strHost, m_port);
    if(!fok)
        return false;

    string strFast;
    //strFast.reserve(m_strRequest.size() );
    fok = m_pOssWrapper->XmlToFastInfoSet(m_strRequest);
    if(fok){
        strFast = m_pOssWrapper->GetStrFastInfoSet();
    }
    else{
        return false;
    }

    rsocket.FormatRequestHeaderSoap(strFast);

    fok = rsocket.SendRequestFastInfoSet(strFast);
    if(!fok)
        return false;

    string strResponseFast;
    fok = rsocket.GetContent(strResponseFast);
    fok = m_pOssWrapper->FastInfoSetToXml(strResponseFast);
    if(fok){
        m_strResponse = m_pOssWrapper->GetStrRawXml();
    }

    return fok;
}