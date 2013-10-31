#pragma once

class CHttpSocket;

class CDcData
{
public:
	CDcData();
	virtual ~CDcData();

	bool LoadRequest(const CString& filePathName);
	bool SaveResponse();

	bool SendRequest();
	bool GetResponse();

	void	SetUrl(const std::wstring& strUrl);

public:
	std::string		m_strRequest;
	std::string		m_strResponse;
	
	CHttpSocket*	m_pSocket;
};