
// LanTxtEditor.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CLanTxtEditorApp:
// See LanTxtEditor.cpp for the implementation of this class
//

class CLanTxtEditorApp : public CWinAppEx
{
	DECLARE_MESSAGE_MAP()
public:
	CLanTxtEditorApp();

// Overrides
public:
	virtual BOOL InitInstance();

	void InitAll();
// Implementation

};

extern CLanTxtEditorApp theApp;