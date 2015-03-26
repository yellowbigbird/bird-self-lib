#pragma once

//#include <ThrdBase.h>
#include <afxmt.h>

#include "Message.h"
#include "comDll.h"

//#pragma warning(disable:4244 4267)
//#include "proto/MessageProto.pb.h"
class MessageProto;

class COM_DLL CHandleMsg
{
public:
	static CHandleMsg&	GetInstance();
    static void DetroyInstance();

	bool StartThread();
	void ThreadLoopHandleMsg();

	static DWORD  WINAPI ThreadFunc(void* pServer);
	void	AddMsg(const CMessage& buffer);
	bool	GetMsg(CMessage& msg);

	void	HandleMsgGetFileList(const CMessage& cmsg);
	void	HandleMsgSendFileList(const CMessage& cmsg);

    bool    SendGetFileRequest(const std::wstring& strFileName);
    bool    SendGetFileListRequest();

    bool    SendRequstRestart();
    bool    HandleMsgRestart(const CMessage& cmsg);

    bool    SendRequstStartIe();
    bool    HandleMsgStartIe(const CMessage& cmsg);

	bool	TestMsg(const std::string& str) const;

protected:
	CHandleMsg();
	virtual ~CHandleMsg();
	
	void	HandleMsgSendFile	(const CMessage& cmsg, const MessageProto* pMsgProto);
	void	HandleMsgGetFile	(const CMessage& cmsg, const MessageProto* pmsgProto);

    bool    SendRequestBase(UINT  ecmd);

protected:
	static CHandleMsg* g_pThis;
	bool	m_ifRunningThread;
	CMutex	m_mutex;
	std::vector<CMessage>	m_vecMsg;
};

