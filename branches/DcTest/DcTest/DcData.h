#pragma once

//class CHttpSocket;
class CMfcHttp;
class COssxWrapper;

class CDcData
{
public:
	CDcData();
	virtual ~CDcData();

    bool Init();

	bool LoadRequest(const CString& filePathName);
	bool SaveResponse();

	bool SendRequest();
	bool GetResponse();

	bool SetUrl(const std::string& strUrl);

public:
	std::string		m_strRequest;
	std::string		m_strResponse;
    std::string     m_strUrl;
    //std::string     m_strHost;
    //std::string     m_strObject;

    //int             m_port;
	
	//CHttpSocket*	m_pSocket;
    CMfcHttp*       m_pSocket;
    COssxWrapper*   m_pOssWrapper;
};