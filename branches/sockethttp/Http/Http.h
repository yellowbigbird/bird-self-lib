// Http.h : main header file for the HTTP DLL
//

#if !defined(AFX_HTTP_H__2396D2EE_3865_454D_82CD_1D2812AA9CBB__INCLUDED_)
#define AFX_HTTP_H__2396D2EE_3865_454D_82CD_1D2812AA9CBB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CHttpApp
// See Http.cpp for the implementation of this class
//

class CHttpApp : public CWinApp
{
public:
	CHttpApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHttpApp)
	public:
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CHttpApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HTTP_H__2396D2EE_3865_454D_82CD_1D2812AA9CBB__INCLUDED_)
