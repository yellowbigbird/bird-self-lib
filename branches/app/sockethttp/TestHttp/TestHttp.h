// TestHttp.h : main header file for the TESTHTTP application
//

#if !defined(AFX_TESTHTTP_H__F9449EB9_01C3_43E1_9B01_9F86C4CFED86__INCLUDED_)
#define AFX_TESTHTTP_H__F9449EB9_01C3_43E1_9B01_9F86C4CFED86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTestHttpApp:
// See TestHttp.cpp for the implementation of this class
//

class CTestHttpApp : public CWinApp
{
public:
	CTestHttpApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestHttpApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTestHttpApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTHTTP_H__F9449EB9_01C3_43E1_9B01_9F86C4CFED86__INCLUDED_)
