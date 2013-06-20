class _declspec(dllexport) CHttpSocket  
{
	friend class CSocketBuffer;
public:
	int GetHeader(char *pHeader,int nMaxLength);
	CSocketBuffer* OpenURL(char *pServer,char *pObject,int nPort=80);
	
	CHttpSocket();
	virtual ~CHttpSocket();
	
	
protected:
	long Receive(char* pBuffer,long nMaxLength);
	BOOL SendRequest(const char* pRequestHeader=NULL,long Length=0);
	void FormatRequestHeader(char *pServer,char *pObject,char *&pOutputHeader,long &Length,long nFrom=0,int nServerType=0);
	BOOL Connect(char* szHostName,int nPort=80);
	BOOL Socket();
	BOOL CloseSocket();
	
	
	
protected:	
	CSocketBuffer* m_pSocketBuffer;
	char m_requestheader[1024];
	int m_port;
	char m_ipaddr[256];
	//char m_requestheader[1024];
	BOOL m_bConnected;
	SOCKET m_s;
	hostent *m_phostent;
};

class _declspec(dllexport) CSocketBuffer  
{
public:
	BOOL IsEOF();
	long Read(BYTE *pBuffer,long nMaxLength);
	int ReadLine(char* pLine,int nMaxLength);
	CSocketBuffer(CHttpSocket* pSocket);
	virtual ~CSocketBuffer();
	
protected:
	char *m_pBuffer;
	long m_nBufferSize;
	long m_nValidBufferSize;
	long m_nCurPos;
	CHttpSocket* m_pSocket;
	BOOL m_bEOF;
};
