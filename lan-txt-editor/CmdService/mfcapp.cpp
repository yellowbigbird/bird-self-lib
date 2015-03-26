#include "stdafx.h"
#include "mfcapp.h"

#include "resource.h"
#include "CmdServiceWinService.h"

#include <atlcom.h>

using namespace std;


BEGIN_MESSAGE_MAP(CMfcApp, CWinApp)
    //{{AFX_MSG_MAP(CMfc1App)
    // NOTE - the ClassWizard will add and remove mapping macros here.
    //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

CMfcApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CMfc1App construction

CMfcApp::CMfcApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMfc1App object


/////////////////////////////////////////////////////////////////////////////
// CMfc1App initialization

BOOL CMfcApp::InitInstance()
{
    AfxEnableControlContainer();

    // Standard initialization
    // If you are not using these features and wish to reduce the size
    //  of your final executable, you should remove from the following
    //  the specific initialization routines you do not need.

#ifdef _AFXDLL
//    Enable3dControls();			// Call this when using MFC in a shared DLL
#else
    Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

    _Module.Init(ObjectMap, this->m_hInstance, IDS_SERVICENAME, NULL);
    _Module.m_bService = TRUE;
    _Module.Start();

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}
