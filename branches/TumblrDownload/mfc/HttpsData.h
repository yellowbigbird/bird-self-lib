#pragma once

//HttpsData.h
#include <afxInet.h>

class CHttpsData  
{
public:
	CHttpsData();
	virtual ~CHttpsData();

	bool GetData(const CString& strFileURL, const CString & strFileLocalFullPath);

public:
	//CString		Url;
	CString		strServerName;
	CString		strTarget;
	DWORD		dwSeviceType;
	INTERNET_PORT	nPort;

};
