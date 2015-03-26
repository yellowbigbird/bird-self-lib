#include "stdafx.h"
#include "HandleMsg.h"

#pragma warning(disable:4244 4267)
#include "proto/MessageProto.pb.h"
#include "proto/msgFile.pb.h"

#include "MsgCmd.h"
#include "FileInfo.h"
#include "FileManager.h"
#include "UdpServer.h"
#include "DebugFile.h"
#include "config.h"

#include "util/UtilString.h"
#include "util/UtilsFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
//using namespace DebugFile;
//////////////////////////////////////////////////////////////////////////
CHandleMsg* CHandleMsg::g_pThis = NULL;

CHandleMsg& CHandleMsg::GetInstance()
{
	if(!g_pThis){
		g_pThis = new CHandleMsg();
        atexit(DetroyInstance);
	}
	return *g_pThis;
}

void CHandleMsg::DetroyInstance()
{
    SAFE_DELETE(g_pThis);
}

CHandleMsg::CHandleMsg()
:m_ifRunningThread(false)
,m_mutex(FALSE, _T("CHandleMsg") )
{
}

CHandleMsg::~CHandleMsg()
{
}
//////////////////////////////////////////////////////////////////////////

bool CHandleMsg::StartThread()
{
	DWORD dwThreadId = 0;
	m_ifRunningThread = true;
	HANDLE hThread = ::CreateThread(NULL, 0, CHandleMsg::ThreadFunc, (LPVOID*)this, 0, &dwThreadId);
	if (!hThread)
	{
		m_ifRunningThread = false;
		return false;
	}
	CloseHandle(hThread);	
	return true;
}

DWORD  WINAPI CHandleMsg::ThreadFunc(void* pServer)
{
	CHandleMsg* pThis = (CHandleMsg*) pServer;
	if(!pThis)
		return 0;
    
    pThis->m_vecMsg.reserve(1000);

	while(true)
	{
		pThis->ThreadLoopHandleMsg();
		::Sleep(1000);
	}
	return 0;
}

bool	CHandleMsg::GetMsg(CMessage& msg)
{
	//AddDebug("CHandleMsg::GetMsg 0.");
	if(m_vecMsg.size() <1)
		return false;
	//AddDebug("CHandleMsg::GetMsg 1.");
	m_mutex.Lock(60*1000) ;
	{	
		msg = m_vecMsg[0];
		m_vecMsg.erase( m_vecMsg.begin() );
		//m_vecMsg.pop_back();	
	}
	m_mutex.Unlock();

	AddDebug("CHandleMsg::GetMsg ok.");
	return true;
}

void	CHandleMsg::AddMsg(const CMessage& msg)
{
	AddDebug("AddMsg 0.");
	m_mutex.Lock(60*1000) ;
	{
		m_vecMsg.push_back(msg);
	}
	m_mutex.Unlock();
}

void CHandleMsg::ThreadLoopHandleMsg()
{
	CMessage msg;
	bool ifget = GetMsg(msg);
	if(!ifget)
		return;

	MessageProto msgp;
	bool ifparseok = msgp.ParseFromString(msg.m_data );
		//msgp.ParseFromArray(&(msg.m_data[0]), msg.m_data.size() );
    if(!ifparseok){
        AddDebug("ParseFromString err!");
		return;
    }
	const int cmdtype = msgp.cmdtype();	
	const _E_CMD_TYPE ecmd = _E_CMD_TYPE(cmdtype);
	
	switch(ecmd)
	{
	case E_CMD_GET_FILE:
		AddDebug("msg type = E_CMD_GET_FILE.");
		HandleMsgGetFile(msg, &msgp);
		break;
	case E_CMD_GET_DIRECTORY:
		AddDebug("msg type = E_CMD_GET_DIRECTORY.");
		break;
	case E_CMD_GET_FILE_LIST:
		AddDebug("msg type = E_CMD_GET_FILE_LIST.");
		HandleMsgGetFileList(msg);
		break;
	case E_CMD_SEND_FILE:
		AddDebug("msg type = E_CMD_SEND_FILE.");
		HandleMsgSendFile(msg, &msgp);
		break;
	case E_CMD_SEND_DIRECTORY:
		AddDebug("msg type = E_CMD_SEND_DIRECTORY.");
		break;
	case E_CMD_SEND_FILE_LIST:
		AddDebug("msg type = E_CMD_SEND_FILE_LIST.");
		HandleMsgSendFileList(msg);
		break;
    case E_CMD_RESTART_COMPUTER:
        AddDebug("msg = E_CMD_RESTART_COMPUTER");
        HandleMsgRestart(msg);
        break;
    case E_CMD_START_IE:
        HandleMsgStartIe(msg);
        break;
	default:
		break;
	}	

	//msg.m_data;
	//msg.m_time;
}

void	CHandleMsg::HandleMsgGetFileList(const CMessage& )
{
	vector<CFileInfo> vecFiles;
	CFileManager::GetInstance().GetFileList(vecFiles);
    
}

void	CHandleMsg::HandleMsgSendFileList(const CMessage& )
{

}
//////////////////////////////////////////////////////////////////////////
//get file from other
void	CHandleMsg::HandleMsgSendFile(const CMessage& , const MessageProto* pMsgProto)
{
	if(!pMsgProto)
		return;
	const MessageProto& msgProto = *pMsgProto;

    //get file time
    const int fileNum = msgProto.msgfiledata_size();
    //save file.
    CFileInfo fileinfo;
    MsgFile_TYPE fileType = MsgFile_TYPE_FILE;
    string strname;
    string strRelativePath;
    UINT64 fileSize = 0;
    UINT64 fileTime = 0;
    bool file7z = false;
    UINT fileInfoIdx = 0;

    bool ifok =false;
    bool fFileInfoExist = false;

    //const bool f7z = CConfig::GetInstance().GetIs7z();

    for(int fileIdx = 0; fileIdx< fileNum; fileIdx++)
    {
        const ::MsgFile& msgFile = msgProto.msgfiledata(fileIdx);
        fileType = msgFile.filetype();
        strname = msgFile.filename();
        fileSize = msgFile.filesize();
        file7z = msgFile.file7z();
        strRelativePath = "";
        if(msgFile.has_filepathname() )
            strRelativePath = msgFile.filepathname();
        fileTime = 0;
        if(msgFile.has_filetime() )
            fileTime = msgFile.filetime();

        //check type
        if(MsgFile_TYPE_NotExist == fileType){
            //output
            CString cstr;
            wstring wstr = UtilString::ConvertMultiByteToWideChar(strname);
            cstr.Format(_T("file=%s not exist."), wstr.c_str() );
            AddDebug(cstr);
            continue;
        }
        
        fFileInfoExist = CFileManager::GetInstance().GetFile(fileinfo, strname);
        if(fFileInfoExist){
            //client already have this file, check if update            
            bool fFileExist = UtilFile::FileExists(fileinfo.m_strFullPathName);
            if(fFileExist){
                AddDebug("file already exist.");
                //continue;
            }
            //check if file is newer
            if(fileinfo.m_time > fileTime){
                //client is newer
                AddDebug("client file is news.");
                continue;
            }
        }
        else{
            //new file
            fileinfo.m_strName = UtilString::ConvertMultiByteToWideChar(strname);
        }
        
        fileinfo.m_size = fileSize;     
        fileinfo.m_strRelativePath = UtilString::ConvertMultiByteToWideChar(strRelativePath);
        fileinfo.m_strFullPathName.clear();
        fileinfo.m_time = fileTime;
        fileinfo.GenerateFullPathName();

        if(!msgFile.has_filedata() ){
            AddDebug("msgFile.has_filedata err.");
            continue;
        }
        const string& strData = msgFile.filedata();
        ifok = fileinfo.SetData(strData, file7z);
        if(!ifok){
            AddDebug("fileinfo.SetData fail.");
            continue;
        }

        //write file
        ifok = fileinfo.WriteBufferToFile();
        if(!ifok){
            AddDebug("WriteBufferToFile error.");
        }
        
        //add fileinfo into file-manager
        if(!fFileInfoExist){
            fileInfoIdx = 0;
            ifok = CFileManager::GetInstance().AddFile(fileinfo, fileInfoIdx);
            if(!ifok){
                AddDebug("AddFile fail.");
                continue;
            }
        }  
    }
}

//send file to other
void	CHandleMsg::HandleMsgGetFile(const CMessage& cmsgSrc, const MessageProto* pMsgProto)
{
	if(!pMsgProto)
		return;
	const MessageProto& msgProto = *pMsgProto;

	//get file name
	const int fileNum = msgProto.msgfiledata_size();
	MsgFile_TYPE fileType = MsgFile_TYPE_FILE;

	string strname;
    string strTemp;
    wstring wstrname;
    CString cstr;

	MessageProto protoSend;
	CFileInfo fileinfo;
	bool ifok = false;
    const bool f7z = CConfig::GetInstance().GetIs7z();

	protoSend.set_cmdtype(E_CMD_SEND_FILE);
	//strSend = protoSend.SerializeAsString();
	//ifok = TestMsg(strSend);

	for(int fileIdx = 0; fileIdx< fileNum; fileIdx++)
	{
		const ::MsgFile& msgFile = msgProto.msgfiledata(fileIdx);
		fileType = msgFile.filetype();
		strname = msgFile.filename();
        wstrname = UtilString::ConvertMultiByteToWideChar(strname);        
		
        MsgFile* pMsgFile = protoSend.add_msgfiledata();
        if(!pMsgFile){
            AddDebug("add_msgfiledata error.");
            continue;
        }
        pMsgFile->set_filename(strname);

		ifok = CFileManager::GetInstance().GetFile(fileinfo, strname);
        if(ifok){    
            cstr.Format(_T("file=%s found"), wstrname.c_str() );
            AddDebug(cstr);     
        }
        else{
            cstr.Format(_T("file=%s not found"), wstrname.c_str() );
            AddDebug(cstr);

            pMsgFile->set_filetype(MsgFile_TYPE_NotExist);
            continue;
        }

        pMsgFile->set_filetype(fileType);
        strTemp = UtilString::ConvertWideCharToMultiByte(fileinfo.m_strRelativePath);
        pMsgFile->set_filepathname(strTemp);
        pMsgFile->set_filesize(fileinfo.m_size);
        pMsgFile->set_filetime(fileinfo.m_time);
        pMsgFile->set_file7z(f7z);

        string strData;
        bool fGetData = fileinfo.GetData(strData, f7z);
        pMsgFile->set_filedata(strData );
	}
	
	strTemp = protoSend.SerializeAsString();
	ifok = TestMsg(strTemp);	

	CMessage cmsg = cmsgSrc;
	cmsg.m_data = strTemp;
	CUdpServer::GetInstance().AddMsg(cmsg);
}
//////////////////////////////////////////////////////////////////////////

bool	CHandleMsg::TestMsg(const string& str) const
{
	bool ifok =false;
	MessageProto prototest;
	ifok = prototest.ParseFromString(str);
	ASSERT(ifok);
	return ifok;
}

bool	CHandleMsg::SendGetFileRequest(const wstring& wstrFileName)
{
    CUdpServer& udp = CUdpServer::GetInstance();

    MessageProto msgproto;
    msgproto.set_cmdtype(E_CMD_GET_FILE);

    MsgFile* pMsgFile = msgproto.add_msgfiledata();
    if(!pMsgFile){
        AddDebug("msgproto.add_msgfiledata err.");
        return false;
    }
    const bool f7z = CConfig::GetInstance().GetIs7z();

    MsgFile& rMsgFile = *pMsgFile; 
    rMsgFile.set_filetype(MsgFile_TYPE_FILE);
    rMsgFile.set_file7z(f7z);

    const string strName = UtilString::ConvertWideCharToMultiByte(wstrFileName);
    rMsgFile.set_filename(strName);
    
    const string strproto = msgproto.SerializeAsString();

    MessageProto msgprotoTest;
    bool ifparse = msgprotoTest.ParseFromString(strproto);

    CMessage msg;
    msg.m_data = strproto;

    udp.AddMsg(msg);
    return true;
}

bool	CHandleMsg::SendGetFileListRequest()
{
    CUdpServer& udp = CUdpServer::GetInstance();

    MessageProto msgproto;
    msgproto.set_cmdtype(E_CMD_GET_FILE_LIST);

    const string strproto = msgproto.SerializeAsString();

    MessageProto msgproto1;
    bool ifparse = msgproto1.ParseFromString(strproto);
    if(!ifparse)
        return false;

    CMessage msg;
    msg.m_data = strproto;

    udp.AddMsg(msg);

    return true;
}

//////////////////////////////////////////////////////////////////////////

bool    CHandleMsg::SendRequestBase(UINT  ecmd)
{
    CUdpServer& udp = CUdpServer::GetInstance();

    MessageProto msgproto;
    msgproto.set_cmdtype(ecmd);

    const string strproto = msgproto.SerializeAsString();

    MessageProto msgproto1;
    bool ifparse = msgproto1.ParseFromString(strproto);
    if(!ifparse)
        return false;

    CMessage msg;
    msg.m_data = strproto;

    udp.AddMsg(msg);
    return true;
}

bool  CHandleMsg::SendRequstRestart()
{
    bool fsend = SendRequestBase(E_CMD_RESTART_COMPUTER);

    return fsend;
}

bool    CHandleMsg::SendRequstStartIe()
{
    bool fsend = SendRequestBase(E_CMD_START_IE);

    return fsend;
}

bool  CHandleMsg::HandleMsgRestart(const CMessage& cmsg)
{
    AddDebug("HandleMsgRestart.");

    //    ShellExecute(
    //hWnd: HWND;        {指定父窗口句柄}
    //Operation: PChar;  {指定动作, 譬如: open、print}
    //FileName: PChar;   {指定要打开的文件或程序}
    //Parameters: PChar; {给要打开的程序指定参数; 如果打开的是文件这里应该是 nil}
    //Directory: PChar;  {缺省目录}
    //ShowCmd: Integer   {打开选项}
    //    ): HINST;            {执行成功会返回应用程序句柄; 如果这个值 <= 32, 表示执行错误}

    //ShellExecute(Handle, 'open', 'notepad.exe', 'C:\WINDOWS\SchedLgU.Txt', nil, SW_SHOWNORMAL);
//    ShellExecute(Handle, 'open', 'IExplore.EXE', 'about:blank', nil, SW_SHOWNORMAL);

	try{
		//up privilege
		HANDLE hToken;  
		TOKEN_PRIVILEGES tkp;  
		OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,&tkp.Privileges[0].Luid);  
		tkp.PrivilegeCount = 1; // one privilege to set  
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;    
		AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,(PTOKEN_PRIVILEGES)NULL, 0);  
		ExitWindowsEx(EWX_REBOOT,0);
	}
	catch(...)
	{
	}   
    
    return true;
}

//////////////////////////////////////////////////////////////////////////


bool    CHandleMsg::HandleMsgStartIe(const CMessage& cmsg)
{
    AddDebug("HandleMsgStartIe.");

	ShellExecute(NULL, _T("open"), _T("IExplore.EXE"), _T("about:blank"), NULL, SW_SHOWNORMAL);
    return true;
}
//////////////////////////////////////////////////////////////////////////
