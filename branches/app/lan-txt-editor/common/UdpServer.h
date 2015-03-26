#pragma once

#include <afxmt.h>

#include "message.h"
#include "comDll.h"


class COM_DLL CUdpServer
{
public:
	static CUdpServer& GetInstance();
    static void DetroyInstance();

	bool	StartThreadListen();
	bool	StartThreadSend();

	int		ThreadLoopListen();
	int		ThreadLoopSend();

	static DWORD  WINAPI ThreadFunc_listen(void* pServer);
	static DWORD  WINAPI ThreadFunc_send(void* pServer);

	//void	SetSendIp(const std::string& strip);
	//void	SetListenPort(UINT port);
	//void	SetSendPort(UINT port);

	//bool	SendMsg(const CMessage& msg);
    void    AddMsg(const CMessage& msg);
    bool    GetMsg(CMessage& msg);

protected:
	CUdpServer();
	virtual ~CUdpServer();

	bool	InitSocket();
	//void	CloseSocket();

protected:
	static CUdpServer* g_pThis;
	bool			m_ifRunningThreadListen;
	bool			m_ifRunningThreadSend;
	//std::string		m_strSendIp;
	//UINT			m_portListen;
	//UINT			m_portSend;

	SOCKET*			m_pSocket;
	CMutex			m_mutexSocket;
    CMutex          m_mutexMsg;

    std::vector<CMessage>	    m_vecMsg;
};