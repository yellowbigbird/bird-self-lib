
// FreeCellCalculateC++.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "FreeCellCalculateC++.h"
#include "FreeCellCalculateC++Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFreeCellCalculateCApp

BEGIN_MESSAGE_MAP(CFreeCellCalculateCApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CFreeCellCalculateCApp construction

CFreeCellCalculateCApp::CFreeCellCalculateCApp()
{
}


// The one and only CFreeCellCalculateCApp object

CFreeCellCalculateCApp theApp;


// CFreeCellCalculateCApp initialization

BOOL CFreeCellCalculateCApp::InitInstance()
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

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CFreeCellCalculateCDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
