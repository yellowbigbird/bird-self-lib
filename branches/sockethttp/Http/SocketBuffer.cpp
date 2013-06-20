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
	///�����һ������
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

///��ȡһ��
int CSocketBuffer::ReadLine(char *pLine, int nMaxLength)
{
	if(m_nCurPos==-1)
	{
		///�����һ������
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
	///�ӵ�ǰλ�������һ�еĽ���
	int index=0;

	while(!m_bEOF && m_pBuffer[m_nCurPos]!='\n' && index<nMaxLength-1)
	{
		pLine[index]=m_pBuffer[m_nCurPos];
		m_nCurPos++;
		index++;
		
		///��ָ����ڻ���Ĵ�С
		if(m_nCurPos >= m_nValidBufferSize)
		{
			///����������л�������
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
			///��������û��������,˵���Ѿ���ȡ���
			else
			{
				m_bEOF=TRUE;
			}
		}
	}
	pLine[index++]=m_pBuffer[m_nCurPos];
	
	///ָ��������һλ����'\n'
	m_nCurPos++;
	return index;
}

///��ȡָ�����ֽ�,����ʵ�ʶ�ȡ���ֽ���.
long CSocketBuffer::Read(BYTE *pBuffer, long nMaxLength)
{
	long nLength=0;
	if(m_bEOF)
	{
		return 0;
	}
	if(m_nCurPos==-1)
	{
		///�����һ������
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
		if(m_nCurPos>=m_nValidBufferSize)		//�������е����ݲ���,�������ȡ
		{
			///����������л�������
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
			///��������û��������,˵���Ѿ���ȡ���
			else
			{
				m_bEOF=TRUE;
			}
		}
	}
	return nLength;
}

///�ж��Ƿ��Ѿ���ȡ���
BOOL CSocketBuffer::IsEOF()
{
	return m_bEOF;
}
/*

///���������е����ݳ��ȴ�����������ݳ���
if(m_nValidBufferSize-m_nCurPos>=nMaxLength)
{
	memcpy(pBuffer,m_pBuffer+m_nCurPos,nMaxLength);
	nLength=nMaxLength;
	m_nCurPos+=nMaxLength;
	
	///���������е������Ѿ�������
	if(m_nCurPos>=m_nValidBufferSize)
	{
		///����ȡ
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
			///�ļ��Ѿ�����
			m_bEOF=TRUE;
		}
	}
}
///�������е����ݲ���
else
{
	///�ȶ�ȡ��������ʣ�������
	memcpy(pBuffer,m_pBuffer+m_nCurPos,m_nValidBufferSize-m_nCurPos);
	nLength=m_nValidBufferSize-m_nCurPos;
	if(m_nValidBufferSize<=0)
	{
		///�ļ�����
		m_bEOF=TRUE;
	}
	else
	{
		///�ݹ��ȡ��һ��
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
