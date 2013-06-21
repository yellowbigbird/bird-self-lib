// Http.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Http.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CHttpApp

BEGIN_MESSAGE_MAP(CHttpApp, CWinApp)
	//{{AFX_MSG_MAP(CHttpApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHttpApp construction

CHttpApp::CHttpApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	///初始化Socket函数库
	int err;
	WORD wVersion;
	WSADATA WSAData;
	wVersion=MAKEWORD(2,0);
	err=WSAStartup(wVersion,&WSAData);
	if(err!=0)
	{
		AfxMessageBox("can't load Socket lib.");
	}
	if(LOBYTE( WSAData.wVersion ) != 2)
	{
		AfxMessageBox("can't find right Socket lib.");
		WSACleanup();
	}
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CHttpApp object

CHttpApp theApp;

int CHttpApp::ExitInstance() 
{
	///清除Socket库
	WSACleanup();
	return CWinApp::ExitInstance();
}
