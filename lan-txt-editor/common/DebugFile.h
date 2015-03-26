#pragma once

#include <string>
#include <vector>
#include <afxmt.h>

#include "comDll.h"

//////////////////////////////////////////////////////////////////////////
#define WM_UPDATE_DEBUG     (WM_USER+100)

//////////////////////////////////////////////////////////////////////////
class COM_DLL CDebug
{	
public:
	static CDebug& GetInstance();
    static void DetroyInstance();

	void AddDebug(const std::wstring& wstr);
	void SetLogFilePathName(const std::wstring& wstrName);

	std::wstring		GetTimeString();
    void                GetVecString(std::vector<std::wstring>& vecString) const;
    void                SetUpdateWindow(HWND hWindow);

protected:
	CDebug();
	virtual ~CDebug();

	static CDebug*      g_pThis;
	std::wstring		m_strFileFullPathName;
    HWND                m_hWndToUpdate;
	std::vector<std::wstring>		m_vecStr;
	CMutex	            m_mutex;
};

void COM_DLL AddDebug(const std::string& str = "");
void COM_DLL AddDebug(const std::wstring& wstr );
void COM_DLL AddDebug(const CString& cstr );
void COM_DLL AddDebug(const char* strParam, ...);
void COM_DLL AddDebug(const TCHAR* strParam, ...);
