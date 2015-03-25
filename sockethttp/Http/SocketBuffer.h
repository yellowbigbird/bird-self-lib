// SocketBuffer.h: interface for the CSocketBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SOCKETBUFFER_H__22AA80EF_C963_4DE1_ABB8_02BA38CB68AC__INCLUDED_)
#define AFX_SOCKETBUFFER_H__22AA80EF_C963_4DE1_ABB8_02BA38CB68AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class CHttpSocket;
class _declspec(dllexport) CSocketBuffer  
{
public:
	void Seek(int nFileCurPos=0);
	int GetFileCurPos();
	int GetValidBufferSize();
	int GetBufferSize();
	char* Detach();
	void Attach(char *pBuffer,int nBufferSize);
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

#endif // !defined(AFX_SOCKETBUFFER_H__22AA80EF_C963_4DE1_ABB8_02BA38CB68AC__INCLUDED_)
