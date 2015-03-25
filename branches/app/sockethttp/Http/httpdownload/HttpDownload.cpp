#include "StdAfx.h"
#include "HttpDownload.h"

/************************************************************************
                                                                     
************************************************************************/
int CBase64::s_nBase64Mask[] = {0, 1, 3, 7, 15, 31, 63, 127, 255};
CString CBase64::s_strBase64TAB = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");

int  CBase64::Base64Decode(LPCTSTR lpszDecoding, CString& strDecoded)
{
    int nIndex =0;
	int nDigit;
    int nDecode[ 256 ];
	int nSize;
	int nNumBits = 6;

	if( lpszDecoding == NULL )
		return 0;
	
	if( ( nSize = lstrlen(lpszDecoding) ) == 0 )
		return 0;

	// Build Decode Table
	for( int i = 0; i < 256; i++ ) 
		nDecode[i] = -2; // Illegal digit
	for( i=0; i < 64; i++ )
	{
		nDecode[ s_strBase64TAB[ i ] ] = i;
		nDecode[ '=' ] = -1; 
    }

	// Clear the output buffer
	strDecoded = _T("");
	long lBitsStorage  =0;
	int nBitsRemaining = 0;
	int nScratch = 0;
	UCHAR c;
	
	// Decode the Input
	for( nIndex = 0, i = 0; nIndex < nSize; nIndex++ )
	{
		c = lpszDecoding[ nIndex ];

		// 忽略所有不合法的字符
		if( c> 0x7F)
			continue;

		nDigit = nDecode[c];
		if( nDigit >= 0 ) 
		{
			lBitsStorage = (lBitsStorage << nNumBits) | (nDigit & 0x3F);
			nBitsRemaining += nNumBits;
			while( nBitsRemaining > 7 ) 
			{
				nScratch = lBitsStorage >> (nBitsRemaining - 8);
				strDecoded += (nScratch & 0xFF);
				i++;
				nBitsRemaining -= 8;
			}
		}
    }	

	return strDecoded.GetLength();
}

int  CBase64::Base64Encode(LPCTSTR lpszEncoding, CString& strEncoded)
{
    int nDigit;
	int nNumBits = 6;
	int nIndex = 0;
	int nInputSize;

	strEncoded = _T( "" );
	if( lpszEncoding == NULL )
		return 0;

	if( ( nInputSize = lstrlen(lpszEncoding) ) == 0 )
		return 0;

	int nBitsRemaining  = 0;
	long lBitsStorage	= 0;
	long lScratch		= 0;
	int nBits;
	UCHAR c;

	while( nNumBits > 0 )
	{
		while( ( nBitsRemaining < nNumBits ) &&  ( nIndex < nInputSize ) ) 
		{
			c = lpszEncoding[ nIndex++ ];
			lBitsStorage <<= 8;
			lBitsStorage |= (c & 0xff);
			nBitsRemaining += 8;
		}
		if( nBitsRemaining < nNumBits ) 
		{
			lScratch = lBitsStorage << ( nNumBits - nBitsRemaining );
			nBits    = nBitsRemaining;
			nBitsRemaining = 0;
		}	 
		else 
		{
			lScratch = lBitsStorage >> ( nBitsRemaining - nNumBits );
			nBits	 = nNumBits;
			nBitsRemaining -= nNumBits;
		}
		nDigit = (int)(lScratch & s_nBase64Mask[nNumBits]);
		nNumBits = nBits;
		if( nNumBits <=0 )
			break;
		
		strEncoded += s_strBase64TAB[ nDigit ];
	}
	// Pad with '=' as per RFC 1521
	while( strEncoded.GetLength() % 4 != 0 )
		strEncoded += '=';

	return strEncoded.GetLength();
}

/************************************************************************
                                                                     
************************************************************************/
CHttpDownload::CHttpDownload():
m_hSocket(INVALID_SOCKET),
m_nResponseHeaderSize(0)
{
     memset(m_szRequestHeader, 0, sizeof(m_szRequestHeader));
	 memset(m_szResponseHeader, 0, sizeof(m_szResponseHeader));
}

CHttpDownload::~CHttpDownload()
{
     CloseSocket();
}

bool CHttpDownload::CreateSocket()
{
	CloseSocket();

	m_hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_hSocket)
	{
		return  false;
	}
	return true;
}

void CHttpDownload::CloseSocket()
{
	if (INVALID_SOCKET != m_hSocket )
	{
		closesocket(m_hSocket);
		m_hSocket = INVALID_SOCKET;
	}
}

void CHttpDownload::FormatRequestHeader(const char* pszServer, int nPort, const char* pszObject, const char *pAuthorization /* = NULL */)
{
    memset(m_szRequestHeader, '\0', 1024);

	//第１行：请求的方法，路径，版本
	strcat(m_szRequestHeader, "GET ");
	strcat(m_szRequestHeader, pszObject);
	strcat(m_szRequestHeader, " HTTP/1.0");
	strcat(m_szRequestHeader, "\r\n");
	//第２行：请求的数据类型
	strcat(m_szRequestHeader, "Accept: */*");
	strcat(m_szRequestHeader, "\r\n");

	//第３行：主机，端口
	char  szPort[10];
	sprintf(szPort, ":%d", nPort);
	strcat(m_szRequestHeader, "Host: ");
	strcat(m_szRequestHeader, pszServer);
	strcat(m_szRequestHeader, szPort);
	strcat(m_szRequestHeader, "\r\n");
	
	//第４行：缓冲控制
	strcat(m_szRequestHeader, "Catch-control: no-cache");
	strcat(m_szRequestHeader, "\r\n");

	//第５行：访问权限
	if (pAuthorization)
	{
		CString  strAuth;
		strcat(m_szRequestHeader, "Authorization: Basic ");
		CBase64::Base64Encode(pAuthorization, strAuth);
	    strcat(m_szRequestHeader, strAuth);
	    strcat(m_szRequestHeader, "\r\n");
	}
	//最后一行：空行
	strcat(m_szRequestHeader, "\r\n");

}

bool CHttpDownload::SendRequest()
{
	int  len = strlen(m_szRequestHeader);

	//if(send(m_hSocket,m_szRequestHeader,len,0)==SOCKET_ERROR)
	//{
	//	return false;
	//}
    
	if (SelectSend(m_szRequestHeader, len ,DEFAULT_SENDTIMEOUT) <= 0)
	{
		return false;
	}

	if (!GetResponseHeader())
	{
		return false;
	}
	return true;
}

bool CHttpDownload::Connect(const char* pszHostIP, int nPort /* DEFAULT_PORT */, long ltimeout /* DEFAULT_CONNECTTIMEOUT*/)
{
    ASSERT(pszHostIP);

    sockaddr_in  addr;
	memset( &addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port   = htons(nPort);
	addr.sin_addr.S_un.S_addr = inet_addr(pszHostIP);

	unsigned   long   ul   =   1;
	int ret   =   ioctlsocket(m_hSocket, FIONBIO, (unsigned long*)&ul);  
	if( ret == SOCKET_ERROR)
	{
		return   false;
	}

	connect(m_hSocket, (sockaddr*)&addr, sizeof(addr));

	timeval   timeout = { 1, ltimeout }; 
	fd_set   r;  

	FD_ZERO(&r);  
	FD_SET(m_hSocket,  &r);  
		
	ret = select(0, 0, &r, 0, &timeout);  
	if( ret <=  0 )  
	{  
		return false;

	}  
	//一般非锁定模式套接比较难控制，可以根据实际情况考虑   再设回阻塞模式  
	unsigned   long   ul1=   0   ;  
	ret   =   ioctlsocket(m_hSocket, FIONBIO, (unsigned long*)&ul1);  
	if( ret == SOCKET_ERROR)
	{  
		return false;  
	}   

    return true;
}

bool CHttpDownload::GetResponseHeader()
{
     char c = 0;
     bool bEndResponse = false;
	 int nIndex = 0;  
     int ret;

	 while(!bEndResponse && nIndex < 1024)
	 {
	     ret = SelectRecv(&m_szResponseHeader[nIndex++], 1);  
	     if (ret <= 0)
		     return false;

		if(nIndex >= 4)
	     {
		     if(m_szResponseHeader[nIndex - 4] == '\r' && m_szResponseHeader[nIndex - 3] == '\n'
			  && m_szResponseHeader[nIndex - 2] == '\r' && m_szResponseHeader[nIndex - 1] == '\n')
			     bEndResponse = true;
	     }
	 }
	 m_nResponseHeaderSize = nIndex;
	
     return  true;
}

int CHttpDownload::GetServerInfo(DWORD &dwContentLength, DWORD &dwStatusCode) const
{
	if (0 == m_nResponseHeaderSize)
	{
		dwContentLength = -1; 
		dwStatusCode = -1;
		return -1;
	}		
	char     szState[3];
	szState[0] = m_szResponseHeader[9];
	szState[1] = m_szResponseHeader[10];
	szState[2] = m_szResponseHeader[11];

	dwStatusCode =  (DWORD)_ttol(szState);

	// 获取ContentLength
	CString   strResponseHeader = m_szResponseHeader;
	int nPos = strResponseHeader.Find("Content-length:");
	if (nPos == -1)
		return -1;

	CString strDownFileLen = strResponseHeader.Mid(nPos + strlen("Content-length:"));	
	nPos = strDownFileLen.Find("\r\n");
	if (nPos == -1)
		return -1;

	strDownFileLen = strDownFileLen.Left(nPos);	
	strDownFileLen.TrimLeft();
	strDownFileLen.TrimRight();

	// Content-Length:
	dwContentLength = (DWORD) _ttol( (LPCTSTR)strDownFileLen );

	return  0;
}

int CHttpDownload::SelectSend(char *pData, int len, long timeout /* = DEFAULT_RECVTIMEOUT */)
{
    	fd_set writefds;
		FD_ZERO(&writefds);
		FD_SET(m_hSocket, &writefds);

		timeval  tv = { timeout, 0 };   
	
		int  ret = select(0, NULL, &writefds, NULL, &tv);
		if ( ret == 0 || ret == SOCKET_ERROR)
		{
			return  0;
		}

		if (FD_ISSET(m_hSocket, &writefds) == 0)
			return 0;

		ret = send(m_hSocket, pData, len, 0);
		
		if (ret == SOCKET_ERROR)
		{
		//	DWORD dwError = WSAGetLastError();
			return 0;
		}
		else if (ret == 0)
		{
			return 0;
		}	
          
		return ret;
}


int CHttpDownload::SelectRecv(char *pData, int len, long timeout)
{
    	fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(m_hSocket, &readfds);

		timeval  tv = { timeout, 0 };   
	
		int  ret = select(0, &readfds, NULL, NULL, &tv);
		if ( ret == 0 || ret == SOCKET_ERROR)
		{
			return 0;
		}

		if (FD_ISSET(m_hSocket, &readfds) == 0)
			return 0;

		ret = recv(m_hSocket, pData, len, 0);
		
		if (ret == SOCKET_ERROR)
		{
		//	DWORD dwError = WSAGetLastError();
			return 0;
		}
		else if (ret == 0)
		{
			return 0;
		}	
          
		return ret;
}

/************************************************************************
下载历史事件告警记录文件                                                                      
http://172.16.0.108/histevent.xml
************************************************************************/
bool CHttpDownload::Download(LPCTSTR lpszDownloadUrl, LPCTSTR lpszSavePath)
{
	 int  nPort;
	 CString  strService, strObject;

     if (!ParseURL(lpszDownloadUrl, strService, strObject, nPort))
	 {
		 return false;
	 }
     
	 if (!CreateSocket())
		 return false;
     if (!Connect(strService, nPort))
		 return false;
	 
	 FormatRequestHeader(strService, nPort, strObject);

     if (!SendRequest())
		 return false;

	 DWORD  dwFileLen, dwStatuCode;
     if (-1 == GetServerInfo(dwFileLen, dwStatuCode))
	 {
		 return false;
	 }
	 
	 CFile  filedown;
	 if (!filedown.Open(lpszSavePath, CFile::modeCreate|CFile::modeWrite))
	 {
		 return false;
	 }

	 DWORD  dwTotalBytes = 0;
	 char   szdata[2048] = { 0 };
	 while (dwTotalBytes < dwFileLen)
	 {
           int nRecvBytes = SelectRecv(szdata, 2048);        
		   if (nRecvBytes <= 0 )
		   {
			   break;
		   }
		   filedown.Write(szdata, nRecvBytes);
		   dwTotalBytes += nRecvBytes;
	 }
     
	 CloseSocket();
	 filedown.Close();
	
     if (dwTotalBytes < dwFileLen)
	 {
		 try
		 {
             CFile::Remove(lpszSavePath);
		 }
		 catch (CFileException* e)
		 {
		 	 e->Delete();
		 }
		
		 return false;
	 }

	 return true;
}
  
