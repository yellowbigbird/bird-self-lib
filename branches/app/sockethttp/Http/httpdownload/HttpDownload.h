#ifndef _HTTPDOWNLOAD_H
#define _HTTPDOWNLOAD_H

//#include "InitSock.h"

//static  CInitSock  initsock;

/************************************************************************
Description:  Base64编码、解码类
Function:     可用于解决http协议的授权验证问题
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
Description: 用http协议下载文件
Function:    支持http下载文件,用socket实现, 简单的非阻塞超时机制
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
	char    m_szRequestHeader[1024];      //保存请求头
	char    m_szResponseHeader[1024];     //保存响应头
	int     m_nResponseHeaderSize;        //响应头大小
};

static bool ParseURL(LPCTSTR lpszURL, CString &strServer, CString &strObject, int& nPort)
{
    CString  strURL(lpszURL);
	strURL.TrimLeft();
	strURL.TrimRight();
	
	// 清除数据
	strServer = _T("");
	strObject = _T("");
	nPort	  = 0;

	int nPos = strURL.Find("://");
	if( nPos == -1 )
		return false;

	// 进一步验证是否为http://
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
	/// 注意：并没有考虑URL中有用户名和口令的情形和最后有#的情形
	/// 例如：http://abc@def:www.yahoo.com:81/index.html#link1
	/// 
	//////////////////////////////////////////////////////////////

	// 查找是否有端口号
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