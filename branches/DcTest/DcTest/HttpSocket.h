// HttpSocket.h: interface for the CHttpSocket class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
//_MSC_VER

#include "winsock2.h"
//#include <afxinet.h>
#include <string>

class CHttpSocket  
{
public:
    CHttpSocket();
	virtual ~CHttpSocket();

	int				GetServerState();						//���ط�����״̬�� -1��ʾ���ɹ�
    int				GetField(const char* szSession, std::string& strValue);	//����ĳ����ֵ,-1��ʾ���ɹ�
	int				GetResponseLine(char *pLine,int nMaxLength);				//��ȡ����ͷ��һ��
    std::string		GetResponseHeader();								//��ȡ�����ķ���ͷ
	const char *	FormatRequestHeader(char *pServer,char *pObject,long &Length,
						char* pCookie=NULL,char *pReferer=NULL,
						long nFrom=0,long nTo=0,
						int nServerType=0);									//��ʽ������ͷ
	int				GetRequestHeader(char *pHeader,int nMaxLength) const;
    void            SetRequest(const std::string& str);
	BOOL			SendRequest(const char* pRequestHeader = NULL,long Length = 0);
	

	BOOL			SetTimeout(int nTime,int nType=0);  //seconds
	long			Receive(char* pBuffer,long nMaxLength);
	BOOL			Connect(const std::string& strHostName,int nPort=80);
	BOOL			Socket();
	BOOL			CloseSocket();

    const std::string&      GetContent() const;
    bool            ParseContent();
    bool            GetValueFromHexString(const std::string& str, int& value) const;


protected:	
	//char m_requestheader[1024];		
	//char m_ResponseHeader[1024];	
    std::string     m_requestheader;
    std::string     m_ResponseHeader;
    std::string     m_strContent;

	int m_port;						//�˿�
	char m_ipaddr[256];				//IP��ַ
	BOOL m_bConnected;
	SOCKET m_s;
	hostent *m_phostent;

	int m_nCurIndex;				//GetResponsLine()�������α��¼
	BOOL m_bResponsed;				//�Ƿ��Ѿ�ȡ���˷���ͷ
	int m_nResponseHeaderSize;		//��Ӧͷ�Ĵ�С
/*
	int m_nBufferSize;
	char *m_pBuffer;*/

};

