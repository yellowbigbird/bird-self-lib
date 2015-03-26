
// LanServer.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CLanServerApp:
// See LanServer.cpp for the implementation of this class
//

class CLanServerApp : public CWinApp
{
public:
	CLanServerApp();

// Overrides
public:
	virtual BOOL InitInstance();
	void InitAll();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CLanServerApp theApp;