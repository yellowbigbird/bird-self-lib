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

    bool            Connect(const std::wstring& strUrl);
	
	int				GetServerState();						//返回服务器状态码 -1表示不成功
    int				GetField(const char* szSession, std::string& strValue);	//返回某个域值,-1表示不成功

	int				GetResponseLine(char *pLine,int nMaxLength);				//获取返回头的一行
    std::string		GetResponseHeader();								//获取完整的返回头

    const std::string&      FormatRequestHeader(const std::string& strServer
        ,char *pObject
        //,long &Length,
        ,char* pCookie=NULL
        ,char *pReferer=NULL
        //long nFrom=0,long nTo=0,
        //int nServerType=0
        );									//格式化请求头

    const std::string&      FormatRequestHeaderSoap(const std::string& strServer
        ,const std::string& pObject
        ,const std::string& pData    
        //,long nFrom=0
        //,long nTo=0
        //,int nServerType=0
        );	

	int				GetRequestHeader(char *pHeader,int nMaxLength) const;

    void            SetRequestHeader(const std::string& strHeader);
	bool			SendRequest(const std::string& strHeader);
    bool			SendRequest();
    //bool		    SendRequestFastInfoSet();
	

	BOOL			SetTimeout(int nTime,int nType=0);  //seconds
    long			Receive(std::string& strResponse);  //char* pBuffer

	bool			Socket();
	BOOL			CloseSocket();

    bool            GetContent(std::string& str) const;
    bool            ParseContent();
    bool            GetValueFromHexString(const std::string& str, int& value) const;

protected:
    bool			ConnectHostPort(const std::string& strHostName,int nPort=80);

protected:	
	//char m_requestheader[1024];		
	//char m_ResponseHeader[1024];	
    std::string     m_requestheader;
    std::string     m_ResponseHeader;
    std::string     m_strContent;

    CString         m_strUrl;
    CString         m_strServer;
    CString         m_strObject;

	int             m_port;						//端口
	char            m_ipaddr[256];				//IP地址
	BOOL            m_bConnected;
	SOCKET          m_s;
	hostent*        m_phostent;

	int m_nCurIndex;				//GetResponsLine()函数的游标记录
	BOOL m_bResponsed;				//是否已经取得了返回头
	int m_nResponseHeaderSize;		//回应头的大小
/*
	int m_nBufferSize;
	char *m_pBuffer;*/

};

