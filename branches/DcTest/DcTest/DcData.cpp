#include "stdafx.h"
#include "dcdata.h"

#include "httpSocket.h"

#include "util/UtilString.h"
#include "util/UtilsFile.h"

using namespace std;
////////////////////
CDcData::CDcData()
:m_pSocket(NULL)
,m_port(80)
{
    m_pSocket = new CHttpSocket();
    if(!m_pSocket)
        return;
	bool fok = m_pSocket->Socket();    
    if(!fok)
    {
        ASSERT(FALSE);
        //return 0;
    }

	m_pSocket->SetTimeout(15,0);
}

CDcData::~CDcData()
{
    SAFE_DELETE(m_pSocket);
}

///////////////////////
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

    m_strHost = "chellilisrc101";
    m_port = 8080;
    //AfxParseURL(m_strUrl, dwServiceType, m_strServer, m_strObject, m_port);
}

bool CDcData::SendRequest()
{
    if(!m_pSocket
        || m_strHost.length() < 1
        || m_port < 1
        )
        return false;
    CHttpSocket& rsocket = *m_pSocket;
    bool fok = rsocket.Connect(m_strHost, m_port);
    if(!fok)
        return false;

    rsocket.FormatRequestHeader(m_strHost, "");
    rsocket.SendRequest();

    return true;
}