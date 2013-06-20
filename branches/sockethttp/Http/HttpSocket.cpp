// HttpSocket.cpp: implementation of the CHttpSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HttpSocket.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MAXHEADERSIZE 1024
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CHttpSocket::CHttpSocket()
{
	m_s=NULL;
	m_phostent=NULL;
	m_port=80;

	m_bConnected=FALSE;

	for(int i=0;i<256;i++)
		m_ipaddr[i]='\0';
	memset(m_requestheader,0,MAXHEADERSIZE);
	memset(m_ResponseHeader,0,MAXHEADERSIZE);

	m_nCurIndex = 0;		//
	m_bResponsed = FALSE;
	m_nResponseHeaderSize = -1;
/*
	m_nBufferSize = nBufferSize;
	m_pBuffer = new char[nBufferSize];
	memset(m_pBuffer,0,nBufferSize);*/
}

CHttpSocket::~CHttpSocket()
{
	CloseSocket();
}

BOOL CHttpSocket::Socket()
{
	if(m_bConnected)return FALSE;

	struct protoent *ppe; 
	ppe=getprotobyname("tcp"); 
	
	///´´½¨SOCKET¶ÔÏó
	m_s=socket(PF_INET,SOCK_STREAM,ppe->p_proto);
	if(m_s==INVALID_SOCKET)
	{
		MessageBox(NULL,"socket()º¯ÊýÖ´ÐÐÊ§°Ü!","´íÎó",MB_OK);
		return FALSE;
	}

	return TRUE;

}

BOOL CHttpSocket::Connect(char *szHostName,int nPort)
{
	if(szHostName==NULL)
		return FALSE;

	///ÈôÒÑ¾­Á¬½Ó,ÔòÏÈ¹Ø±Õ
	if(m_bConnected)
	{
		CloseSocket();
	}

	///±£´æ¶Ë¿ÚºÅ
	m_port=nPort;

	///¸ù¾ÝÖ÷»úÃû»ñµÃIPµØÖ·
	m_phostent=gethostbyname(szHostName);
	if(m_phostent==NULL)
	{
		MessageBox(NULL,"gethostbyname()º¯ÊýÖ´ÐÐÊ§°Ü!","´íÎó",MB_OK);
		return FALSE;
	}
	
	///Á¬½Ó
	struct in_addr ip_addr;
	memcpy(&ip_addr,m_phostent->h_addr_list[0],4);///h_addr_list[0]Àï4¸ö×Ö½Ú,Ã¿¸ö×Ö½Ú8Î»
	
	struct sockaddr_in destaddr;
	memset((void *)&destaddr,0,sizeof(destaddr)); 
	destaddr.sin_family=AF_INET;
	destaddr.sin_port=htons(80);
	destaddr.sin_addr=ip_addr;

	///±£´æÖ÷»úµÄIPµØÖ·×Ö·û´®
	sprintf(m_ipaddr,"%d.%d.%d.%d",
		destaddr.sin_addr.S_un.S_un_b.s_b1,
		destaddr.sin_addr.S_un.S_un_b.s_b2,
		destaddr.sin_addr.S_un.S_un_b.s_b3,
		destaddr.sin_addr.S_un.S_un_b.s_b4);
	/*inet_addr();°Ñ´øµãµÄIPµØÖ·×Ö·û´®×ª»¯Îªin_addr¸ñÊ½;
	  inet_ntoa();×÷ÓÃÏà·´*/
	
	/*×¢ÒâÀí½âsturct in_addr µÄ½á¹¹:Ò»¸ö32Î»µÄÊý;Ò»¹²Í¬ÌåµÄÐÎÊ½Ê¹ÓÃ
	(1)Ã¿8Î»Ò»¸ö¼´s_b1~s_b4;
	(2)Ã¿16Î»Ò»¸ö¼´s_w1~s_w2;
	(3)32Î»s_addr
	struct   in_addr {
    union   {
			  struct{
				unsigned  char   s_b1,
								 s_b2,
								 s_b3,
								 s_b4;
					} S_un_b;
              struct{
				unsigned  short  s_w1,
                                 s_w2
			        }S_un_w;      
               unsigned long  S_addr;
			} S_un;
		};
	*/

	if(connect(m_s,(struct sockaddr*)&destaddr,sizeof(destaddr))!=0)
	{
		//CloseSocket();
		//m_s=NULL;
		MessageBox(NULL,"connect()º err","error",MB_OK);
		return FALSE;
	}

	///ÉèÖÃÒÑ¾­Á¬½ÓµÄ±êÖ¾
	m_bConnected=TRUE;
	return TRUE;
}

///¸ù¾ÝÇëÇóµÄÏà¶ÔURLÊä³öHTTPÇëÇóÍ·
const char *CHttpSocket::FormatRequestHeader(char *pServer,char *pObject, long &Length,
									  char *pCookie,char *pReferer,long nFrom,
									  long nTo,int nServerType)
{
	char szPort[10];
	char szTemp[20];
	sprintf(szPort,"%d",m_port);
	memset(m_requestheader,'\0',1024);

	///µÚ1ÐÐ:·½·¨,ÇëÇóµÄÂ·¾¶,°æ±¾
	strcat(m_requestheader,"GET ");
	strcat(m_requestheader,pObject);
	strcat(m_requestheader," HTTP/1.1");
    strcat(m_requestheader,"\r\n");

	///µÚ2ÐÐ:Ö÷»ú
    strcat(m_requestheader,"Host:");
	strcat(m_requestheader,pServer);
    strcat(m_requestheader,"\r\n");

	///µÚ3ÐÐ:
	if(pReferer != NULL)
	{
		strcat(m_requestheader,"Referer:");
		strcat(m_requestheader,pReferer);
		strcat(m_requestheader,"\r\n");		
	}

	///µÚ4ÐÐ:½ÓÊÕµÄÊý¾ÝÀàÐÍ
    strcat(m_requestheader,"Accept:*/*");
    strcat(m_requestheader,"\r\n");

	///µÚ5ÐÐ:ä¯ÀÀÆ÷ÀàÐÍ
    strcat(m_requestheader,"User-Agent:Mozilla/4.0 (compatible; MSIE 5.00; Windows 98)");
    strcat(m_requestheader,"\r\n");

	///µÚ6ÐÐ:Á¬½ÓÉèÖÃ,±£³Ö
	strcat(m_requestheader,"Connection:Keep-Alive");
	strcat(m_requestheader,"\r\n");

	///µÚ7ÐÐ:Cookie.
	if(pCookie != NULL)
	{
		strcat(m_requestheader,"Set Cookie:0");
		strcat(m_requestheader,pCookie);
		strcat(m_requestheader,"\r\n");
	}

	///µÚ8ÐÐ:ÇëÇóµÄÊý¾ÝÆðÊ¼×Ö½ÚÎ»ÖÃ(¶ÏµãÐø´«µÄ¹Ø¼ü)
	if(nFrom > 0)
	{
		strcat(m_requestheader,"Range: bytes=");
		_ltoa(nFrom,szTemp,10);
		strcat(m_requestheader,szTemp);
		strcat(m_requestheader,"-");
		if(nTo > nFrom)
		{
			_ltoa(nTo,szTemp,10);
			strcat(m_requestheader,szTemp);
		}
		strcat(m_requestheader,"\r\n");
	}
	
	///×îºóÒ»ÐÐ:¿ÕÐÐ
	strcat(m_requestheader,"\r\n");

	///·µ»Ø½á¹û
	Length=strlen(m_requestheader);
	return m_requestheader;
}

///·¢ËÍÇëÇóÍ·
BOOL CHttpSocket::SendRequest(const char *pRequestHeader, long Length)
{
	if(!m_bConnected)return FALSE;

	if(pRequestHeader==NULL)
		pRequestHeader=m_requestheader;
	if(Length==0)
		Length=strlen(m_requestheader);

	if(send(m_s,pRequestHeader,Length,0)==SOCKET_ERROR)
	{
		MessageBox(NULL,"send()º¯ÊýÖ´ÐÐÊ§°Ü!","´íÎó",MB_OK);
		return FALSE;
	}
	int nLength;
	GetResponseHeader(nLength);
	return TRUE;
}

long CHttpSocket::Receive(char* pBuffer,long nMaxLength)
{
	if(!m_bConnected)return NULL;

	///½ÓÊÕÊý¾Ý
	long nLength;
	nLength=recv(m_s,pBuffer,nMaxLength,0);
	
	if(nLength <= 0)
	{
		//MessageBox(NULL,"recv()º¯ÊýÖ´ÐÐÊ§°Ü!","´íÎó",MB_OK);
		CloseSocket();
	}
	return nLength;
}

///¹Ø±ÕÌ×½Ó×Ö
BOOL CHttpSocket::CloseSocket()
{
	if(m_s != NULL)
	{
		if(closesocket(m_s)==SOCKET_ERROR)
		{
			MessageBox(NULL,"closesocket()º¯ÊýÖ´ÐÐÊ§°Ü!","´íÎó",MB_OK);
			return FALSE;
		}
	}
	m_s = NULL;
	m_bConnected=FALSE;
	return TRUE;
}

int CHttpSocket::GetRequestHeader(char *pHeader, int nMaxLength) const
{
	int nLength;
	if(int(strlen(m_requestheader))>nMaxLength)
	{
		nLength=nMaxLength;
	}
	else
	{
		nLength=strlen(m_requestheader);
	}
	memcpy(pHeader,m_requestheader,nLength);
	return nLength;
}

//ÉèÖÃ½ÓÊÕ»òÕß·¢ËÍµÄ×î³¤Ê±¼ä
BOOL CHttpSocket::SetTimeout(int nTime, int nType)
{
	if(nType == 0)
	{
		nType = SO_RCVTIMEO;
	}
	else
	{
		nType = SO_SNDTIMEO;
	}

	DWORD dwErr;
    dwErr=setsockopt(m_s,SOL_SOCKET,nType,(char*)&nTime,sizeof(nTime)); 
	if(dwErr)
	{
		MessageBox(NULL,"setsockopt()º¯ÊýÖ´ÐÐÊ§°Ü!","´íÎó",MB_OK);
		return FALSE;
	}
	return TRUE;
}

//»ñÈ¡HTTPÇëÇóµÄ·µ»ØÍ·
const char* CHttpSocket::GetResponseHeader(int &nLength)
{
	if(!m_bResponsed)
	{
		char c = 0;
		int nIndex = 0;
		BOOL bEndResponse = FALSE;
		while(!bEndResponse && nIndex < MAXHEADERSIZE)
		{
			recv(m_s,&c,1,0);
			m_ResponseHeader[nIndex++] = c;
			if(nIndex >= 4)
			{
				if(m_ResponseHeader[nIndex - 4] == '\r' && m_ResponseHeader[nIndex - 3] == '\n'
					&& m_ResponseHeader[nIndex - 2] == '\r' && m_ResponseHeader[nIndex - 1] == '\n')
				bEndResponse = TRUE;
			}
		}
		m_nResponseHeaderSize = nIndex;
		m_bResponsed = TRUE;
	}
	
	nLength = m_nResponseHeaderSize;
	return m_ResponseHeader;
}

//·µ»ØHTTPÏìÓ¦Í·ÖÐµÄÒ»ÐÐ.
int CHttpSocket::GetResponseLine(char *pLine, int nMaxLength)
{
	if(m_nCurIndex >= m_nResponseHeaderSize)
	{
		m_nCurIndex = 0;
		return -1;
	}

	int nIndex = 0;
	char c = 0;
	do 
	{
		c = m_ResponseHeader[m_nCurIndex++];
		pLine[nIndex++] = c;
	} while(c != '\n' && m_nCurIndex < m_nResponseHeaderSize && nIndex < nMaxLength);
	
	return nIndex;
}

int CHttpSocket::GetField(const char *szSession, char *szValue, int nMaxLength)
{
	//È¡µÃÄ³¸öÓòÖµ
	if(!m_bResponsed) return -1;
	
	CString strRespons;
	strRespons = m_ResponseHeader;
	int nPos = -1;
	nPos = strRespons.Find(szSession,0);
	if(nPos != -1)
	{
		nPos += strlen(szSession);
		nPos += 2;
		int nCr = strRespons.Find("\r\n",nPos);
		CString strValue = strRespons.Mid(nPos,nCr - nPos);
		strcpy(szValue,strValue);
		return (nCr - nPos);
	}
	else
	{
		return -1;
	}
}

int CHttpSocket::GetServerState()
{
	//ÈôÃ»ÓÐÈ¡µÃÏìÓ¦Í·,·µ»ØÊ§°Ü
	if(!m_bResponsed) return -1;
	
	char szState[3];
	szState[0] = m_ResponseHeader[9];
	szState[1] = m_ResponseHeader[10];
	szState[2] = m_ResponseHeader[11];

	return atoi(szState);
}
