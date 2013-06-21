// HttpSocket.cpp: implementation of the CHttpSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HttpSocket.h"

#include <ASSERT.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MAX_HEADER_SIZE 1024

//const int c_port = 8100;
using namespace std;

const char* c_strContentlen = "Content-Length";

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
	memset(m_requestheader,0,MAX_HEADER_SIZE);
	//memset(m_ResponseHeader,0,MAXHEADERSIZE);

	m_nCurIndex = 0;		//
	m_bResponsed = FALSE;
	m_nResponseHeaderSize = -1;

    WORD wVersion;
	WSADATA WSAData;
	wVersion=MAKEWORD(2,0);
	int err=WSAStartup(wVersion,&WSAData);
}

CHttpSocket::~CHttpSocket()
{
	CloseSocket();
}

BOOL CHttpSocket::Socket()
{
	if(m_bConnected)
        return FALSE;

	struct protoent *ppe; 
	ppe=getprotobyname("tcp"); 
    if(!ppe)
    {
        DWORD rr = GetLastError();
        assert(false);
        return FALSE;
    }
	
	///����SOCKET����
	m_s=socket(PF_INET,SOCK_STREAM,ppe->p_proto);
	if(m_s==INVALID_SOCKET)
	{
		MessageBox(NULL,"socket() running error.!","Error",MB_OK);
		return FALSE;
	}

	return TRUE;

}

BOOL CHttpSocket::Connect(char *szHostName,int nPort)
{
	if(szHostName==NULL)
		return FALSE;

	///���Ѿ�����,���ȹر�
	if(m_bConnected)
	{
		CloseSocket();
	}

	///����˿ں�
	m_port=nPort;

	///�������������IP��ַ
	m_phostent=gethostbyname(szHostName);
	if(m_phostent==NULL)
	{
		MessageBox(NULL,"gethostbyname()ERROR!","error",MB_OK);
		return FALSE;
	}
	
	///����
	struct in_addr ip_addr;
	memcpy(&ip_addr,m_phostent->h_addr_list[0],4);///h_addr_list[0]��4���ֽ�,ÿ���ֽ�8λ
	
	struct sockaddr_in destaddr;
	memset((void *)&destaddr,0,sizeof(destaddr)); 
	destaddr.sin_family=AF_INET;
	destaddr.sin_port=htons(m_port);  //80
	destaddr.sin_addr=ip_addr;

	///����������IP��ַ�ַ���
	sprintf(m_ipaddr,"%d.%d.%d.%d",
		destaddr.sin_addr.S_un.S_un_b.s_b1,
		destaddr.sin_addr.S_un.S_un_b.s_b2,
		destaddr.sin_addr.S_un.S_un_b.s_b3,
		destaddr.sin_addr.S_un.S_un_b.s_b4);
	/*inet_addr();�Ѵ����IP��ַ�ַ���ת��Ϊin_addr��ʽ;
	  inet_ntoa();�����෴*/
	
	/*ע�����sturct in_addr �Ľṹ:һ��32λ����;һ��ͬ�����ʽʹ��
	(1)ÿ8λһ����s_b1~s_b4;
	(2)ÿ16λһ����s_w1~s_w2;
	(3)32λs_addr
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
		MessageBox(NULL,"connect()?err","error",MB_OK);
		return FALSE;
	}

	///�����Ѿ����ӵı�־
	m_bConnected=TRUE;
	return TRUE;
}

///������������URL���HTTP����ͷ
const char *CHttpSocket::FormatRequestHeader(char *pServer,char *pObject, long &Length,
									  char *pCookie,char *pReferer,long nFrom,
									  long nTo,int nServerType)
{
	char szPort[10];
	char szTemp[20];
	sprintf(szPort,"%d",m_port);
	memset(m_requestheader,'\0',1024);

	///��1��:����,�����·��,�汾
	strcat(m_requestheader,"GET ");
	strcat(m_requestheader,pObject);
	strcat(m_requestheader," HTTP/1.1");
    strcat(m_requestheader,"\r\n");

	///��2��:����
    strcat(m_requestheader,"Host:");
	strcat(m_requestheader,pServer);
    strcat(m_requestheader,"\r\n");

	///��3��:
	if(pReferer != NULL)
	{
		strcat(m_requestheader,"Referer:");
		strcat(m_requestheader,pReferer);
		strcat(m_requestheader,"\r\n");		
	}

	///��4��:���յ���������
    strcat(m_requestheader,"Accept:*/*");
    strcat(m_requestheader,"\r\n");

	///��5��:���������
    strcat(m_requestheader,"User-Agent:Mozilla/4.0 (compatible; MSIE 5.00; Windows 98)");
    strcat(m_requestheader,"\r\n");

	///��6��:��������,����
	strcat(m_requestheader,"Connection:Keep-Alive");
	strcat(m_requestheader,"\r\n");

	///��7��:Cookie.
	if(pCookie != NULL)
	{
		strcat(m_requestheader,"Set Cookie:0");
		strcat(m_requestheader,pCookie);
		strcat(m_requestheader,"\r\n");
	}

	///��8��:�����������ʼ�ֽ�λ��(�ϵ������Ĺؼ�)
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
	
	///���һ��:����
	strcat(m_requestheader,"\r\n");

	///���ؽ��
	Length=strlen(m_requestheader);
	return m_requestheader;
}

///��������ͷ
BOOL CHttpSocket::SendRequest(const char *pRequestHeader, long Length)
{
	if(!m_bConnected)
        return FALSE;

	if(pRequestHeader==NULL)
		pRequestHeader=m_requestheader;
	if(Length==0)
		Length=strlen(m_requestheader);

	if(send(m_s,pRequestHeader,Length,0)==SOCKET_ERROR)
	{
		MessageBox(NULL,"send() error.!","Error",MB_OK);
		return FALSE;
	}
	//int nLength;
	GetResponseHeader();
	return TRUE;
}

long CHttpSocket::Receive(char* pBuffer,long nMaxLength)
{
	if(!m_bConnected)return NULL;

	///��������
	long nLength;
	nLength=recv(m_s,pBuffer,nMaxLength,0);
	
	if(nLength <= 0)
	{
		//MessageBox(NULL,"recv()����ִ��ʧ��!","����",MB_OK);
		CloseSocket();
	}
	return nLength;
}

///�ر��׽���
BOOL CHttpSocket::CloseSocket()
{
	if(m_s != NULL)
	{
		if(closesocket(m_s)==SOCKET_ERROR)
		{
			MessageBox(NULL,"closesocket() Error!","Error",MB_OK);
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

//���ý��ջ��߷��͵��ʱ��
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
		MessageBox(NULL,"setsockopt() Error!","Error",MB_OK);
		return FALSE;
	}
	return TRUE;
}

//��ȡHTTP����ķ���ͷ
string CHttpSocket::GetResponseHeader()
{
    if(m_bResponsed)
        return m_ResponseHeader;

    int nIndex = 0;
    BOOL bEndResponse = FALSE;
    int recevlen = 0 ;
    DWORD err = 0;
    const int buffersize = 512;
    char buff[buffersize+1];
    m_ResponseHeader.clear();

    while(true)
    {
        memset(buff, 0, buffersize+1);
        recevlen = ::recv(m_s, buff, buffersize, 0 );  //0 MSG_DONTWAIT  MSG_WAITALL
        if(recevlen>0){
            m_ResponseHeader += buff;
            int strsize = m_ResponseHeader.size();
            strsize = m_ResponseHeader.length();
        }
        if(recevlen<1
            || recevlen < buffersize){
                err = ::GetLastError();
                bEndResponse = TRUE;  
                break;
        }

        //if(nIndex >= 4)
        //{
        //	if(m_ResponseHeader[nIndex - 4] == '\r' 
        //                 && m_ResponseHeader[nIndex - 3] == '\n'
        //		&& m_ResponseHeader[nIndex - 2] == '\r' 
        //                 && m_ResponseHeader[nIndex - 1] == '\n')
        //	bEndResponse = TRUE;
        //}

    }
    m_nResponseHeaderSize = m_ResponseHeader.length();
    m_bResponsed = TRUE;

    int len = 0;
    bool ifget = GetContentLength(len);


    //nLength = m_nResponseHeaderSize;
    return m_ResponseHeader;
}

//����HTTP��Ӧͷ�е�һ��.
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

//return -1 =error.
int CHttpSocket::GetField(const char *szSession, char *szValue, int nMaxLength)
{
	//ȡ��ĳ����ֵ
	if(!m_bResponsed)
        return -1;

    memset(szValue, 0, nMaxLength);
	
	CString strRespons;
    strRespons = m_ResponseHeader.c_str();
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

bool CHttpSocket::GetContentLength(int& len)
{
    bool ifok = false;
    len = 0;

    const int c_valuelen = 30;
    char strValue[c_valuelen];
	int ret = GetField(c_strContentlen, strValue,c_valuelen);
    if(-1 == ret){
        //no content-length
        const string c_strrn = "\r\n";
        int pos = m_ResponseHeader.find(c_strrn + c_strrn);
        if(pos< 0 || pos >= (int)m_ResponseHeader.length())
            return ifok;
        string strSub = m_ResponseHeader.substr(pos + c_strrn.length()*2 );
        pos = strSub.find(c_strrn);
        if(pos< 0 || pos >= (int)m_ResponseHeader.length())
            return ifok;
        string strDataLen = strSub.substr(0, pos);
        string strData = strSub.substr(pos +c_strrn.length() );

        ifok = true;
    }
    else{
        len = atoi(strValue);
        ifok = true;
    }

    return ifok;
}

int CHttpSocket::GetServerState()
{
	//��û��ȡ����Ӧͷ,����ʧ��
	if(!m_bResponsed
        || m_ResponseHeader.length()< 12) 
        return -1;
	
	char szState[3];
	szState[0] = m_ResponseHeader[9];
	szState[1] = m_ResponseHeader[10];
	szState[2] = m_ResponseHeader[11];

	return atoi(szState);
}
