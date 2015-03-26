
// LanServer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "LanServer.h"
#include "LanServerDlg.h"

#include "common\HandleMsg.h"
#include "common\FileManager.h"
#include "common\UdpServer.h"
#include "common\config.h"
#include "common/DebugFile.h"

#include "util/UtilsFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//using namespace DebugFile;

CLanServerApp theApp;

const wstring c_strConfigFile = _T("config_server.ini");
//////////////////////////////////////////////////////////////////////////
// CLanServerApp

BEGIN_MESSAGE_MAP(CLanServerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CLanServerApp construction

CLanServerApp::CLanServerApp()
{
}

BOOL CLanServerApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	InitAll();

	CLanServerDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

void CLanServerApp::InitAll()
{
	AddDebug();
	AddDebug("CLanServerApp::InitAll 0");

    //config must be 1st
    CConfig::GetInstance().SetIsServer( true);
    wstring wstrPath = UtilFile::GetFilePath();
    wstrPath += _T("\\");
    wstrPath += c_strConfigFile;
    CConfig::GetInstance().LoadFile(wstrPath);
	AddDebug(_T("load config file = %s."), wstrPath.c_str() );

	CFileManager::GetInstance().StartSearchTread();
	CHandleMsg::GetInstance().StartThread();

	CUdpServer::GetInstance().StartThreadListen();
    CUdpServer::GetInstance().StartThreadSend();

	AddDebug("CLanServerApp::InitAll end");
}
