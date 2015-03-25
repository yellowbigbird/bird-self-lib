// SocketBuffer.cpp: implementation of the CSocketBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HttpSocket.h"
#include "SocketBuffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
 CSocketBuffer::CSocketBuffer(CHttpSocket* pSocket)
{
	m_bEOF=FALSE;
	m_pSocket=pSocket;
	m_nCurPos=-1;
	m_nBufferSize=1024*10;
	m_nValidBufferSize=0;
	m_pBuffer=new char[m_nBufferSize+1];
	memset(m_pBuffer,'\0',m_nBufferSize+1);

	/*
	///读入第一段数据
	m_nValidBufferSize=m_pSocket->Receive(m_pBuffer,m_nBufferSize);
	m_pBuffer[m_nValidBufferSize]='\0';
	if(m_nValidBufferSize<=0)
	{
		m_bEOF=TRUE;
	}
	*/
}

CSocketBuffer::~CSocketBuffer()
{
	if(m_pBuffer != NULL)
		delete []m_pBuffer;
}

///读取一行
int CSocketBuffer::ReadLine(char *pLine, int nMaxLength)
{
	if(m_nCurPos==-1)
	{
		///读入第一段数据
		m_nValidBufferSize=m_pSocket->Receive(m_pBuffer,m_nBufferSize);
		
		if(m_nValidBufferSize<=0)
		{
			m_pBuffer[0]='\0';
			m_bEOF=TRUE;
		}
		else
		{
			m_pBuffer[m_nValidBufferSize]='\0';
			m_nCurPos=0;
		}

	}
	///从当前位置起读到一行的结束
	int index=0;

	while(!m_bEOF && m_pBuffer[m_nCurPos]!='\n' && index<nMaxLength-1)
	{
		pLine[index]=m_pBuffer[m_nCurPos];
		m_nCurPos++;
		index++;
		
		///若指针大于缓冲的大小
		if(m_nCurPos >= m_nValidBufferSize)
		{
			///如果服务器中还有数据
			if(m_nValidBufferSize > 0)
			{
				m_nValidBufferSize=m_pSocket->Receive(m_pBuffer,m_nBufferSize);
				
				if(m_nValidBufferSize <= 0)
				{
					m_pBuffer[0] = '\0';
					m_bEOF = TRUE;
				}
				else
				{
					m_pBuffer[m_nValidBufferSize] = '\0';	
					m_nCurPos = 0;
				}
			
			}
			///服务器中没有数据了,说明已经读取完毕
			else
			{
				m_bEOF=TRUE;
			}
		}
	}
	pLine[index++]=m_pBuffer[m_nCurPos];
	
	///指针再下移一位跳过'\n'
	m_nCurPos++;
	return index;
}

///读取指定的字节,返回实际读取的字节数.
long CSocketBuffer::Read(BYTE *pBuffer, long nMaxLength)
{
	long nLength=0;
	if(m_bEOF)
	{
		return 0;
	}
	if(m_nCurPos==-1)
	{
		///读入第一段数据
		m_nValidBufferSize=m_pSocket->Receive(m_pBuffer,m_nBufferSize);	
		if(m_nValidBufferSize <= 0)
		{
			m_pBuffer[0]='\0';
			m_bEOF=TRUE;
		}
		else
		{
			m_pBuffer[m_nValidBufferSize]='\0';
			m_nCurPos=0;
		}
	}

	while(!m_bEOF && nLength<nMaxLength)
	{
		pBuffer[nLength]=m_pBuffer[m_nCurPos];
		nLength++;
		m_nCurPos++;
		if(m_nCurPos>=m_nValidBufferSize)		//缓冲区中的数据不足,则继续读取
		{
			///如果服务器中还有数据
			if(m_nValidBufferSize > 0)
			{
				m_nValidBufferSize=m_pSocket->Receive(m_pBuffer,m_nBufferSize);				
				if(m_nValidBufferSize <= 0)
				{
					m_pBuffer[0]='\0';
					m_bEOF=TRUE;
				}
				else
				{
					m_pBuffer[m_nValidBufferSize]='\0';
					m_nCurPos=0;
				}
			}
			///服务器中没有数据了,说明已经读取完毕
			else
			{
				m_bEOF=TRUE;
			}
		}
	}
	return nLength;
}

///判断是否已经读取完毕
BOOL CSocketBuffer::IsEOF()
{
	return m_bEOF;
}
/*

///若缓冲区中的数据长度大于请求的数据长度
if(m_nValidBufferSize-m_nCurPos>=nMaxLength)
{
	memcpy(pBuffer,m_pBuffer+m_nCurPos,nMaxLength);
	nLength=nMaxLength;
	m_nCurPos+=nMaxLength;
	
	///若缓冲区中的数据已经读完了
	if(m_nCurPos>=m_nValidBufferSize)
	{
		///继续取
		if(m_nValidBufferSize>0)
		{
			m_nValidBufferSize=m_pSocket->Receive(m_pBuffer,m_nBufferSize);
			m_pBuffer[m_nValidBufferSize]='\0';
			if(m_nValidBufferSize<=0)
			{
				m_bEOF=TRUE;
			}
			m_nCurPos=0;
		}
		else
		{
			///文件已经结束
			m_bEOF=TRUE;
		}
	}
}
///缓冲区中的数据不够
else
{
	///先读取缓冲区中剩余的数据
	memcpy(pBuffer,m_pBuffer+m_nCurPos,m_nValidBufferSize-m_nCurPos);
	nLength=m_nValidBufferSize-m_nCurPos;
	if(m_nValidBufferSize<=0)
	{
		///文件结束
		m_bEOF=TRUE;
	}
	else
	{
		///递归读取下一段
		m_nValidBufferSize=m_pSocket->Receive(m_pBuffer,m_nBufferSize);
		if(m_nValidBufferSize<=0)
		{
			m_bEOF=TRUE;
		}
		m_nCurPos=0;
		m_pBuffer[m_nValidBufferSize]='\0';
		
		nLength+=Read(pBuffer+nLength,nMaxLength-nLength);
	}
	}*/

void CSocketBuffer::Attach(char *pBuffer, int nBufferSize)
{
	delete []m_pBuffer;
	m_nBufferSize=nBufferSize;
	m_pBuffer=pBuffer;
}

char* CSocketBuffer::Detach()
{
	char *pTemp;
	pTemp = m_pBuffer;
	m_pBuffer = NULL;
	return pTemp;
}

int CSocketBuffer::GetBufferSize()
{
	return m_nBufferSize;
}

int CSocketBuffer::GetValidBufferSize()
{
	return m_nValidBufferSize;
}

int CSocketBuffer::GetFileCurPos()
{
	return m_nCurPos;
}

void CSocketBuffer::Seek(int nFileCurPos)
{
	if(nFileCurPos >= 0 && nFileCurPos < m_nBufferSize)
	{
		m_nCurPos = nFileCurPos;
	}
	else
	{
		return ;
	}
}
