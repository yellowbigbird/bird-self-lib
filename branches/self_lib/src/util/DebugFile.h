#pragma once

#include <string>
#include <vector>
#include <afxmt.h>

#include "selflib_dll.h"

//////////////////////////////////////////////////////////////////////////
#define WM_UPDATE_DEBUG     (WM_USER+100)

//////////////////////////////////////////////////////////////////////////
class SELFLIB_DLL CDebug
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

void SELFLIB_DLL AddDebug(const std::string& str = "");
void SELFLIB_DLL AddDebug(const std::wstring& wstr );
void SELFLIB_DLL AddDebug(const CString& cstr );
void SELFLIB_DLL AddDebug(const char* strParam, ...);
void SELFLIB_DLL AddDebug(const TCHAR* strParam, ...);
