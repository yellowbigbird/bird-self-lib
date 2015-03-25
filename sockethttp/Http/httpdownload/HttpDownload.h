#ifndef _HTTPDOWNLOAD_H
#define _HTTPDOWNLOAD_H

//#include "InitSock.h"

//static  CInitSock  initsock;

/************************************************************************
Description:  Base64���롢������
Function:     �����ڽ��httpЭ�����Ȩ��֤����
************************************************************************/
class  CBase64
{
private:
	CBase64();
	~CBase64();
	CBase64(const CBase64&);
	CBase64& operator = (const CBase64&);

public:
    static  int  Base64Encode(LPCTSTR lpszEncoding, CString& strEncoded);
    static  int  Base64Decode(LPCTSTR lpszDecoding, CString& strDecoded);

public:
	static  int		s_nBase64Mask[];
	static  CString	s_strBase64TAB;
};

/************************************************************************
Description: ��httpЭ�������ļ�
Function:    ֧��http�����ļ�,��socketʵ��, �򵥵ķ�������ʱ����
************************************************************************/
class CHttpDownload
{
public:
	CHttpDownload();
	~CHttpDownload();

public:
	enum 
	{ 
		DEFAULT_PORT = 80 ,
		DEFAULT_CONNECTTIMEOUT = 10,
		DEFAULT_SENDTIMEOUT = 10,
	    DEFAULT_RECVTIMEOUT = 10, 
	};

	bool  Download(LPCTSTR lpszDownloadUrl, LPCTSTR lpszSavePath);

protected:
	bool  CreateSocket();
	void  CloseSocket();

    void  FormatRequestHeader(const char* pszServer, int nPort, const char* pszObject, const char *pAuthorization = NULL);
	bool  SendRequest();
	
private:
	bool  Connect(const char* pszHostIP, int nPort = DEFAULT_PORT, long ltimeout = DEFAULT_CONNECTTIMEOUT);
	int   SelectRecv(char *pData, int len, long timeout = DEFAULT_RECVTIMEOUT);
	int   SelectSend(char *pData, int len, long timeout = DEFAULT_SENDTIMEOUT);

    bool  GetResponseHeader();
    int   GetServerInfo(DWORD &dwContentLength, DWORD &dwStatusCode) const;
	

private:
	SOCKET  m_hSocket;
	char    m_szRequestHeader[1024];      //��������ͷ
	char    m_szResponseHeader[1024];     //������Ӧͷ
	int     m_nResponseHeaderSize;        //��Ӧͷ��С
};

static bool ParseURL(LPCTSTR lpszURL, CString &strServer, CString &strObject, int& nPort)
{
    CString  strURL(lpszURL);
	strURL.TrimLeft();
	strURL.TrimRight();
	
	// �������
	strServer = _T("");
	strObject = _T("");
	nPort	  = 0;

	int nPos = strURL.Find("://");
	if( nPos == -1 )
		return false;

	// ��һ����֤�Ƿ�Ϊhttp://
	CString strTemp = strURL.Left( nPos + lstrlen("://") );
	strTemp.MakeLower();
	if( strTemp.Compare("http://") != 0 )
		return  false;

	strURL = strURL.Mid( strTemp.GetLength() );
	nPos = strURL.Find('/');
	if ( nPos == -1 )
		return  false;

	strObject = strURL.Mid(nPos);
	strTemp   = strURL.Left(nPos);
	
	///////////////////////////////////////////////////////////////
	/// ע�⣺��û�п���URL�����û����Ϳ�������κ������#������
	/// ���磺http://abc@def:www.yahoo.com:81/index.html#link1
	/// 
	//////////////////////////////////////////////////////////////

	// �����Ƿ��ж˿ں�
	nPos = strTemp.Find(":");
	if( nPos == -1 )
	{
		strServer = strTemp;
		nPort	  = CHttpDownload::DEFAULT_PORT;
	}
	else
	{
		strServer = strTemp.Left( nPos );
		strTemp	  = strTemp.Mid( nPos+1 );
		nPort	  = _ttoi((LPCTSTR)strTemp);
	}

	return  true;
}

#endif