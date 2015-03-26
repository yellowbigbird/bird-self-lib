// CmdService.cpp : main Windows Service project file.

#include "stdafx.h"
#include "CmdServiceWinService.h"

//#include <string.h>
#include <common/DebugFile.h>
#include <common/config.h>
#include <common/UdpServer.h>
#include <common/HandleMsg.h>
#include "common\threadpool\UtilThreadPool.h"
#include "common\ActiveWin.h"
//#include <common/FileManager.h>
#include <util/UtilsFile.h>
#include "ProcessWatch.h"

using namespace UtilThreadPool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

//////////////////////////////////////////////////////////////////////////

CServiceModule _Module;

const int            c_eventId = 9527;
const std::wstring   c_strConfigFile = _T("config_server.ini");
const std::wstring   c_strSrvName = _T("CmdService");

#define POOLSIZE 10
#define LOGPATH L"d:\\log.txt"

//////////////////////////////////////////////////////////////////////////

CServiceModule::CServiceModule()
:m_fInited(false)
,m_bService(false)
,dwThreadID(0)
{
}

void CServiceModule::Init(_ATL_OBJMAP_ENTRY* p, HINSTANCE h, UINT nServiceNameID, const GUID* plibid)
{
    //CComModule::Init(p, h, plibid);
	int nPoolSize = POOLSIZE;

    m_bService = TRUE;

    LoadString(h, nServiceNameID, m_szServiceName, sizeof(m_szServiceName) / sizeof(TCHAR));

    // set up the initial service status 
    m_hServiceStatus = NULL;
    m_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    m_status.dwCurrentState = SERVICE_STOPPED;
    m_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    m_status.dwWin32ExitCode = 0;
    m_status.dwServiceSpecificExitCode = 0;
    m_status.dwCheckPoint = 0;
    m_status.dwWaitHint = 0;

    InitDll();
}

void CServiceModule::Init()
{
    //CComModule::Init(p, h, plibid);
	int nPoolSize = POOLSIZE;

    m_bService = TRUE;

	wsprintf(m_szServiceName, L"%s", c_strSrvName);

    // set up the initial service status 
    m_hServiceStatus = NULL;
    m_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    m_status.dwCurrentState = SERVICE_STOPPED;
    m_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    m_status.dwWin32ExitCode = 0;
    m_status.dwServiceSpecificExitCode = 0;
    m_status.dwCheckPoint = 0;
    m_status.dwWaitHint = 0;

    InitDll();

	CDebug::GetInstance().SetLogFilePathName(LOGPATH);
}

void CServiceModule::InitDll()
{
    AddDebug();
    AddDebug("CLanServerApp::InitAll 0");

    if (!AfxSocketInit() )
    {
//        AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
        return ;
    }

    //config must be 1st
    CConfig::GetInstance().SetIsServer( true);
    wstring wstrPath = UtilFile::GetFilePath();
    wstrPath += _T("\\");
    wstrPath += c_strConfigFile;
    const bool fLoad = CConfig::GetInstance().LoadFile(wstrPath);
    AddDebug(_T("load config file = %s."), wstrPath.c_str() );

//    CFileManager::GetInstance().StartSearchTread();
    CHandleMsg::GetInstance().StartThread();

    CUdpServer::GetInstance().StartThreadListen();
    CUdpServer::GetInstance().StartThreadSend();

    AddDebug("CLanServerApp::InitAll end");
}

LONG CServiceModule::Unlock()
{
    LONG l = CComModule::Unlock();
    if (l == 0 && !m_bService)
        PostThreadMessage(dwThreadID, WM_QUIT, 0, 0);
    return l;
}

BOOL CServiceModule::IsInstalled()
{
    BOOL bResult = FALSE;

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (hSCM != NULL)
    {
        SC_HANDLE hService = ::OpenService(hSCM, m_szServiceName, SERVICE_QUERY_CONFIG);
        if (hService != NULL)
        {
            bResult = TRUE;
            ::CloseServiceHandle(hService);
        }
        ::CloseServiceHandle(hSCM);
    }
    return bResult;
}

BOOL CServiceModule::Install()
{
    if (IsInstalled())
        return TRUE;

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCM == NULL)
    {
        MessageBox(NULL, _T("Couldn't open service manager"), m_szServiceName, MB_OK);
        return FALSE;
    }

    // Get the executable file path
    TCHAR szFilePath[_MAX_PATH];
    ::GetModuleFileName(NULL, szFilePath, _MAX_PATH);

    SC_HANDLE hService = ::CreateService(
        hSCM, m_szServiceName, m_szServiceName,
        SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
        SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
        szFilePath, NULL, NULL, _T("RPCSS\0"), NULL, NULL);

    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
        MessageBox(NULL, _T("Couldn't create service"), m_szServiceName, MB_OK);
        return FALSE;
    }

    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);
    return TRUE;
}

BOOL CServiceModule::Uninstall()
{
    if (!IsInstalled())
        return TRUE;

    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (hSCM == NULL)
    {
        MessageBox(NULL, _T("Couldn't open service manager"), m_szServiceName, MB_OK);
        return FALSE;
    }

    SC_HANDLE hService = ::OpenService(hSCM, m_szServiceName, SERVICE_STOP | DELETE);

    if (hService == NULL)
    {
        ::CloseServiceHandle(hSCM);
        MessageBox(NULL, _T("Couldn't open service"), m_szServiceName, MB_OK);
        return FALSE;
    }
    SERVICE_STATUS status;
    ::ControlService(hService, SERVICE_CONTROL_STOP, &status);

    BOOL bDelete = ::DeleteService(hService);
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);

    if (bDelete)
        return TRUE;

    MessageBox(NULL, _T("Service could not be deleted"), m_szServiceName, MB_OK);
    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////
// Logging functions
void CServiceModule::LogEvent(LPCTSTR pFormat, ...)
{
    TCHAR    chMsg[256];
    HANDLE  hEventSource;
    LPTSTR  lpszStrings[1];
    va_list pArg;

    va_start(pArg, pFormat);
    _vstprintf_s(chMsg, pFormat, pArg);
    va_end(pArg);

    lpszStrings[0] = chMsg;

    if (m_bService)
    {
        /* Get a handle to use with ReportEvent(). */
        hEventSource = RegisterEventSource(NULL, m_szServiceName);
        if (hEventSource != NULL)
        {
            /* Write to event log. */
            
            ReportEvent(hEventSource, EVENTLOG_INFORMATION_TYPE, 0, c_eventId, NULL, 1, 0, (LPCTSTR*) &lpszStrings[0], NULL);
            DeregisterEventSource(hEventSource);
        }
    }
    else
    {
        // As we are not running as a service, just write the error to the console.
        _putts(chMsg);
    }
}
// Service startup and registration
void CServiceModule::Start()
{
    SERVICE_TABLE_ENTRY st[] =
    {
        { m_szServiceName, _ServiceMain },
        { NULL, NULL }
    };
    if (m_bService && !::StartServiceCtrlDispatcher(st))
    {
        m_bService = FALSE;
    }
    if (m_bService == FALSE)
        Run();
}

void CServiceModule::ServiceMain(DWORD /* dwArgc */, LPTSTR* /* lpszArgv */)
{
    InitDll();

    // Register the control request handler
    m_status.dwCurrentState = SERVICE_START_PENDING;
    m_hServiceStatus = RegisterServiceCtrlHandler(m_szServiceName, _Handler);
    if (m_hServiceStatus == NULL)
    {
        LogEvent(_T("Handler not installed"));
        return;
    }
    SetServiceStatus(SERVICE_START_PENDING);

    m_status.dwWin32ExitCode = S_OK;
    m_status.dwCheckPoint = 0;
    m_status.dwWaitHint = 0;

    // When the Run function returns, the service has stopped.
    Run();

    SetServiceStatus(SERVICE_STOPPED);
    LogEvent(_T("Service stopped"));
}

inline void CServiceModule::Handler(DWORD dwOpcode)
{
    switch (dwOpcode)
    {
    case SERVICE_CONTROL_STOP:
        SetServiceStatus(SERVICE_STOP_PENDING);
        PostThreadMessage(dwThreadID, WM_QUIT, 0, 0);
        break;
    case SERVICE_CONTROL_PAUSE:
        break;
    case SERVICE_CONTROL_CONTINUE:
        break;
    case SERVICE_CONTROL_INTERROGATE:
        break;
    case SERVICE_CONTROL_SHUTDOWN:
        break;
    default:
        LogEvent(_T("Bad service request"));
    }
}

void WINAPI CServiceModule::_ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
    _Module.ServiceMain(dwArgc, lpszArgv);
}
void WINAPI CServiceModule::_Handler(DWORD dwOpcode)
{
    _Module.Handler(dwOpcode); 
}

void CServiceModule::SetServiceStatus(DWORD dwState)
{
    m_status.dwCurrentState = dwState;
    ::SetServiceStatus(m_hServiceStatus, &m_status);
}

void CServiceModule::Run()
{
    _Module.dwThreadID = GetCurrentThreadId();
    LogEvent(_T("Service started"));
    if (m_bService)
        SetServiceStatus(SERVICE_RUNNING);

//    CMfc1Dlg dlg;
//    //m_pMainWnd = &dlg;
//    int nResponse = dlg.DoModal();
//    if (nResponse == IDOK)
//    {
//        // TODO: Place code here to handle when the dialog is
//        //  dismissed with OK
//    }
//    else if (nResponse == IDCANCEL)
//    {
//        // TODO: Place code here to handle when the dialog is
//        //  dismissed with Cancel
//    }

	CThreadPool::GetInstance().start(&(CActiveWin::GetInstance()));

	ProcessWatch processWatch;
	DWORD threadId;
	HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)processWatch.KillHeavyProgram(), NULL, 0, &threadId);
	if(hThread == NULL)
	{

	}
	CloseHandle(hThread);

    MSG msg;
    while (GetMessage(&msg, 0, 0, 0))
        DispatchMessage(&msg);
}

//////////////////////////////////////////////////////////////////////////
//To install/uninstall the service, type: "CmdService.exe -Install [-u]"
int _tmain(int argc, _TCHAR* argv[])
{
	_Module.Init();

	if (argc >= 2)
	{
		if (argv[1][0] == _T('/'))
		{
			argv[1][0] = _T('-');
		}

		if (_tcsicmp(argv[1], _T("-Install")) == 0)
		{
			_Module.Install();
//			array<String^>^ myargs = System::Environment::GetCommandLineArgs();
//			array<String^>^ args = gcnew array<String^>(myargs->Length - 1);
//
//			// Set args[0] with the full path to the assembly,
//			Assembly^ assem = Assembly::GetExecutingAssembly();
//			args[0] = assem->Location;
//
//			Array::Copy(myargs, 2, args, 1, args->Length - 1);
//			AppDomain^ dom = AppDomain::CreateDomain(L"execDom");
//			Type^ type = System::Object::typeid;
//			String^ path = type->Assembly->Location;
//			StringBuilder^ sb = gcnew StringBuilder(path->Substring(0, path->LastIndexOf(L"\\")));
//			sb->Append(L"\\InstallUtil.exe");
//			Evidence^ evidence = gcnew Evidence();
//			dom->ExecuteAssembly(sb->ToString(), evidence, args);
		}
		else if(_tcsicmp(argv[1], _T("-Uninstall")) == 0)
		{
			_Module.Uninstall();
		}
	}
	else
	{
//		ServiceBase::Run(gcnew CmdServiceWinService());

		SERVICE_TABLE_ENTRY ServiceTable[2];
		ServiceTable[0].lpServiceName = L"CmdService";
		ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)_Module._ServiceMain;

		ServiceTable[1].lpServiceName = NULL;
		ServiceTable[1].lpServiceProc = NULL;

		StartServiceCtrlDispatcher(ServiceTable); 
	}
}
