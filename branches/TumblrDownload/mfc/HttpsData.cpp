#include "stdafx.h"
#include "httpsdata.h"

using namespace std;

CHttpsData::CHttpsData()
{
	//Url = "https://www.tenpay.com/v2/";
	nPort = INTERNET_DEFAULT_HTTPS_PORT;
}

CHttpsData::~CHttpsData()
{
}

bool CHttpsData::GetData(const CString& strFileURL, const CString & strFileLocalFullPath)
{
	if(strFileURL.IsEmpty() )
		return false;

	CString strHeaders = _T("Content-Type: application/x-www-form-urlencoded");
	AfxParseURL(strFileURL,dwSeviceType,strServerName,strTarget,nPort);
	CInternetSession sess;
	sess.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT,1000*20);
	sess.EnableStatusCallback(TRUE);
	CHttpConnection* pHttpConnect = sess.GetHttpConnection(strServerName, nPort);
	CHttpFile* pHttpFile = pHttpConnect->OpenRequest(CHttpConnection::HTTP_VERB_GET,
		strTarget,NULL,1,NULL,NULL,INTERNET_FLAG_SECURE);
	try
	{
		BOOL bResult =pHttpFile->SendRequest(strHeaders);
	}
	catch (CInternetException* pException)
	{
		pException->m_dwError;
		pException->Delete();
		return false;
	}
	pHttpFile->SetReadBufferSize(2048);
	CString str;
	CString strGetData;
	while(pHttpFile->ReadString(strGetData))
	{
		str +="\r\n";
		str +=strGetData;
	}
	CFile file(strFileLocalFullPath, CFile::modeCreate | CFile::modeWrite);
	file.Write(str,str.GetLength());
	file.Close();

	pHttpFile->Close();
	delete pHttpFile;
	pHttpConnect->Close();
	delete pHttpConnect;
	sess.Close();

	return true;
}
