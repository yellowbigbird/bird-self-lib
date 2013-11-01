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
	//memset(m_requestheader,0,MAX_HEADER_SIZE);
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

bool CHttpSocket::Socket()
{
	if(m_bConnected)
        return 0;

	struct protoent *ppe; 
	ppe=getprotobyname("tcp"); 
    if(!ppe)
    {
        DWORD rr = GetLastError();
        assert(false);
        return 0;
    }
	
	///����SOCKET����
	m_s=socket(PF_INET,SOCK_STREAM,ppe->p_proto);
	if(m_s==INVALID_SOCKET)
	{
		//MessageBox(NULL,"socket() running error.!","Error",MB_OK);
		return 0;
	}

	return true;

}

bool CHttpSocket::Connect(const std::string& szHostName,int nPort)
{
    if(szHostName.length()< 1
        || nPort<1)
		return false;

	///���Ѿ�����,���ȹر�
	if(m_bConnected)
	{
		CloseSocket();
	}

	///����˿ں�
	m_port=nPort;

	///�������������IP��ַ
    m_phostent=gethostbyname(szHostName.c_str() );
	if(m_phostent==NULL)
	{
		//MessageBox(NULL,"gethostbyname()ERROR!","error",MB_OK);
		return false;
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
		//MessageBox(NULL,"connect()?err","error",MB_OK);
		return false;
	}

	///�����Ѿ����ӵı�־
	m_bConnected=TRUE;
	return true;
}

///������������URL���HTTP����ͷ
const string& CHttpSocket::FormatRequestHeader(const string& strServer
                                             ,char *pObject
                                             //,long &Length
                                             ,char *pCookie
                                             ,char *pReferer
                                             ,long nFrom
                                             ,long nTo
                                             ,int nServerType)
{
    m_requestheader = "";

	///��1��:����,�����·��,�汾
    m_requestheader += "GET ";
    m_requestheader += pObject;
    //m_requestheader += " HTTP/1.1\r\n";

	/////��2��:����
 //   strcat(m_requestheader,"Host:");
	//strcat(m_requestheader,pServer);
 //   strcat(m_requestheader,"\r\n");

	/////��3��:
	//if(pReferer != NULL)
	//{
	//	strcat(m_requestheader,"Referer:");
	//	strcat(m_requestheader,pReferer);
	//	strcat(m_requestheader,"\r\n");		
	//}

	/////��4��:���յ���������
 //   strcat(m_requestheader,"Accept:*/*");
 //   strcat(m_requestheader,"\r\n");

	/////��5��:���������
 //   strcat(m_requestheader,"User-Agent:Mozilla/4.0 (compatible; MSIE 5.00; Windows 98)");
 //   strcat(m_requestheader,"\r\n");

	/////��6��:��������,����
	//strcat(m_requestheader,"Connection:Keep-Alive");
	//strcat(m_requestheader,"\r\n");

	/////��7��:Cookie.
	//if(pCookie != NULL)
	//{
	//	strcat(m_requestheader,"Set Cookie:0");
	//	strcat(m_requestheader,pCookie);
	//	strcat(m_requestheader,"\r\n");
	//}

	/////��8��:�����������ʼ�ֽ�λ��(�ϵ������Ĺؼ�)
	//if(nFrom > 0)
	//{
	//	strcat(m_requestheader,"Range: bytes=");
	//	_ltoa(nFrom,szTemp,10);
	//	strcat(m_requestheader,szTemp);
	//	strcat(m_requestheader,"-");
	//	if(nTo > nFrom)
	//	{
	//		_ltoa(nTo,szTemp,10);
	//		strcat(m_requestheader,szTemp);
	//	}
	//	strcat(m_requestheader,"\r\n");
	//}
	m_requestheader += "\r\n";

    return m_requestheader;
}

const std::string&       CHttpSocket::FormatRequestHeaderSoap(const std::string& strServer
        ,const std::string& pObject
        ,const std::string& strData   
        ,long nFrom
        ,long nTo
        ,int nServerType)
{
    m_requestheader = "";
    const string strRet = "\r\n";

	///��1��:����,�����·��,�汾
    m_requestheader += "POST ";
    m_requestheader += pObject;
    m_requestheader += " HTTP/1.1";
    m_requestheader += strRet;
    
	//User-Agent
    m_requestheader += "User-Agent:Mozilla/4.0 (compatible; MSIE 5.00; Windows 98)" ; //
    m_requestheader += strRet;

    //SOAPAction
    m_requestheader += "SOAPAction: \"\"";  ///
    m_requestheader += strRet;

    //contenttype
    m_requestheader += "Content-Type: application/fastinfoset";  
    m_requestheader += strRet;

	//host
    m_requestheader += "Host: ";
    m_requestheader += strServer;
    m_requestheader += strRet;
	

	//Content-Length: 3921
    char carray[10];
    memset(carray, 0, 10);
    sprintf_s(carray,10, "%d", strData.size() );
    m_requestheader += "Content-Length: ";
    m_requestheader += carray;
    m_requestheader += strRet;

	///��6��:��������,����
	m_requestheader += "Connection:Keep-Alive";
	m_requestheader += strRet;

    m_requestheader += strRet;

    const int oldSize = m_requestheader.size();
    m_requestheader += strData;
    const int newSize = m_requestheader.size();
    const int addSize = newSize- oldSize;
    if(addSize != strData.size()){
        assert(false);
    }

    return m_requestheader;
}

void  CHttpSocket::SetRequestHeader(const std::string& str)
{
    m_requestheader = str;
}

///��������ͷ
bool CHttpSocket::SendRequest()
{
    return SendRequest(m_requestheader);
}

bool CHttpSocket::SendRequest(const std::string& strRequest)
{
	if(!m_bConnected)
        return 0;

	const char* pRequestHeader = m_requestheader.c_str();
	
	int Length= m_requestheader.length();

	if(SOCKET_ERROR == send(m_s, pRequestHeader, Length,0) )
	{
		//MessageBox(NULL,"send() error.!","Error",MB_OK);
		return false;
	}
    ::Sleep(400); //get full response,

	//int nLength;
	GetResponseHeader();
	return true;
}

long CHttpSocket::Receive(std::string& strResponse)
{
	if(!m_bConnected)
        return 0;

	///��������
	long nLength;
    const int nMaxLength = 1024* 1024;
    char pBuffer[nMaxLength];
	nLength=recv(m_s, pBuffer, nMaxLength, 0);
	
    strResponse = pBuffer;
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
			//MessageBox(NULL,"closesocket() Error!","Error",MB_OK);
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
    const int reqlen = m_requestheader.length();
    if(reqlen>nMaxLength)
	{
		nLength=nMaxLength;
	}
	else
	{
		nLength=reqlen;
	}
    memcpy(pHeader, m_requestheader.c_str() ,nLength);
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
    dwErr= setsockopt(m_s,SOL_SOCKET,nType,(char*)&nTime,sizeof(nTime)); 
	if(dwErr)
	{
		//MessageBox(NULL,"setsockopt() Error!","Error",MB_OK);
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
    m_strContent.clear();

    while(true)
    {
        ::Sleep(1);
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

    bool ifget = ParseContent();

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
int CHttpSocket::GetField(const char *szSession,  std::string& strValue)
{
	if(!m_bResponsed)
        return -1;

    //memset(szValue, 0, nMaxLength);
	
	string strRespons;
    strRespons = m_ResponseHeader;
	int nPos = -1;
	nPos = strRespons.find(szSession,0);
	if(nPos != -1)
	{
		nPos += strlen(szSession);
		nPos += 2;
		int nCr = strRespons.find("\r\n", nPos);
		strValue = strRespons.substr(nPos,nCr - nPos);
		//strcpy(szValue,strValue);
		return (nCr - nPos);
	}
	else
	{
		return -1;
	}
}

bool CHttpSocket::ParseContent()
{
    bool ifok = false;
    int datalen = 0;

    const int c_valuelen = 30;
    string strValue; //[c_valuelen];
	int ret = GetField(c_strContentlen, strValue);

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
        //int datalen = 0;
        ifok = GetValueFromHexString(strDataLen, datalen);
        //string strData;
        if(ifok)
            m_strContent = strSub.substr(pos +c_strrn.length(), datalen );
        else
            m_strContent = strSub.substr(pos +c_strrn.length() );
    }
    else{
        datalen = atoi(strValue.c_str() );
        if(datalen < 1
            || datalen > m_ResponseHeader.size())
            return false;
        int dataStartPos = m_ResponseHeader.size() - datalen;
        m_strContent = m_ResponseHeader.substr(dataStartPos );        
    }
    ifok = true;

    return ifok;
}

const std::string&  CHttpSocket::GetContent() const
{
    return m_strContent;
}

bool  CHttpSocket::GetValueFromHexString(const string& str, int& value) const
{
    const int strlen0 = str.length();
    char cc = 0;
    bool ifErr = false;
    value = 0;
    int curValue = 0;
    //for(int idx= strlen0-1, pos = 0; idx>-1; idx--, pos++)
    for(int idx = 0; idx< strlen0; idx++)
    {
        cc = str[idx];
        if(cc >= '0' && cc <= '9')
        {
            curValue = cc - '0';
        }
        else if(cc >= 'a' && cc <= 'f')
        {
            curValue = cc - 'a'+ 10;
        }
        else if(cc >= 'A' && cc <= 'F')
        {
            curValue = cc - 'A' + 10;
        }
        else{
            ifErr = true;
            continue;
        }
        value = value* 16 + curValue;
    }
    if(ifErr)
        return false;
    
    return true;
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
