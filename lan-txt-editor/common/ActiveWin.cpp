#include "StdAfx.h"
#include "ActiveWin.h"

#include "common\DebugFile.h"
#include <util/utilString.h>

CActiveWin* CActiveWin::g_pThis = NULL;

const WCHAR* g_pLTargetName = L"esfvmvi";
//const WCHAR* g_pCTargetName = L"ESFVMVI";

std::vector<HWND> g_vecHwnd;

#define MAX_COUNT			1024
#define INTERVAL_TIME		(1*60*1000)

CActiveWin& CActiveWin::GetInstance()
{
	if(!g_pThis)
	{
		g_pThis = new CActiveWin();
        atexit(DestroyInstance);        
	}
	return *g_pThis;
}

void CActiveWin::DestroyInstance()
{
	SAFE_DELETE(g_pThis);
}

BOOL CALLBACK CActiveWin::EnumWndsProc(HWND hwnd, LPARAM lParam)
{
    if (!IsWindowVisible(hwnd))
        return TRUE;

    if (!IsWindowEnabled(hwnd))
        return TRUE;

    LONG gwl_style = GetWindowLong(hwnd,GWL_STYLE);
    if ((gwl_style & WS_POPUP) && !(gwl_style & WS_CAPTION))
        return TRUE;

    //HWND hParent = (HWND)GetWindowLong(hwnd,GWL_HWNDPARENT);
    //if (IsWindowEnabled(hParent))
    //    return TRUE;
    //if (IsWindowVisible(hParent))
    //    return TRUE;

	g_vecHwnd.push_back(hwnd);
	((CActiveWin*)lParam)->AddHwnd(hwnd);
	return TRUE;
}

CActiveWin::CActiveWin(void)
{
	m_vecHwnd.clear();
}


CActiveWin::~CActiveWin(void)
{
}

void CActiveWin::run()
{
	DWORD dwTimeout = -1;  
	SystemParametersInfo(SPI_GETFOREGROUNDLOCKTIMEOUT, 0, (LPVOID)&dwTimeout, 0);  
	if (dwTimeout >= 100) {  
		SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, (LPVOID)0, SPIF_SENDCHANGE | SPIF_UPDATEINIFILE);  
	}

	while (1)
	{
		Sleep(INTERVAL_TIME);
		BOOL bOk = ActiveWindows();
		if (!bOk)
		{
			//AddLog("Failed to active window.");
			continue;
		}
		//AddLog("Succeed to active window.");		
	}
}

void CActiveWin::AddHwnd(HWND hwnd)
{
	m_vecHwnd.push_back(hwnd);
}

HWND CActiveWin::FindHwnd()
{
	std::vector<HWND>::iterator it = m_vecHwnd.begin();
	int count = (int)m_vecHwnd.size();
	//WCHAR szLog[1024] = {0};

	//swprintf_s(szLog, L"windows' count: %d", count);
	//AddDebug(szLog);

	for (;it != m_vecHwnd.end(); it++)
	{
		HWND hwnd = *it;
		WCHAR szName[MAX_COUNT];
		//swprintf_s(szLog, L"window's Handle : %x", hwnd);
		//AddDebug(szLog);
		if (GetWindowText(hwnd, szName, MAX_COUNT))
		{
			std::wstring strWindowName(szName);

			//turn to lower case
			UtilString::ChangeStringLetter(strWindowName, false);

			if (strWindowName.find(g_pLTargetName,0) < strWindowName.length() )
				//|| strWindowName.find(g_pCTargetName,0) < strWindowName.length())
			{
				//AddLog("Succeed to find the specified window: %s", strWindowName.c_str());
				return hwnd;
			}
		}
		else if(GetClassName(hwnd, szName, MAX_COUNT))
		{
			std::wstring strWindowName(szName);

			//turn to lower case
			UtilString::ChangeStringLetter(strWindowName, false);

			if (strWindowName.find(g_pLTargetName,0) < strWindowName.length() )
				//|| strWindowName.find(g_pCTargetName,0) < strWindowName.length())
			{
				//AddLog("Succeed to find the specified window: %s", strWindowName.c_str());
				return hwnd;
			}
		}
		else
		{
			DWORD dwErr = GetLastError();
			//AddLog("Get windows  text failed, error is: %d", dwErr);
		}
	}
	return NULL;
}

BOOL CActiveWin::ActiveWindows()
{
	m_vecHwnd.clear();
	g_vecHwnd.clear();

	BOOL bOk = FALSE;
	bOk = EnumWindows(EnumWndsProc, (LPARAM)this);
	if (!bOk)
	{
		DWORD dwErr = GetLastError();
		//AddLog("enum windows failed, error is: %d", dwErr);
	}

	HWND hwnd = FindHwnd();
	bOk = FALSE;
	if (hwnd)
	{		
		HWND hForeWnd = NULL; 
		DWORD dwForeID; 
		DWORD dwCurID; 

		hForeWnd =  GetForegroundWindow(); 
		dwCurID =  GetCurrentThreadId(); 
		dwForeID =  GetWindowThreadProcessId( hForeWnd, NULL ); 
		AttachThreadInput( dwCurID, dwForeID, TRUE); 

		ShowWindow( hwnd, SW_SHOWNORMAL ); 
		//SetWindowPos( hwnd, HWND_TOPMOST, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE ); 
		//SetWindowPos( hwnd, HWND_NOTOPMOST, 0,0,0,0, SWP_NOSIZE|SWP_NOMOVE ); 

		//bOk = SetForegroundWindow(hwnd);
		ShowWindow( hwnd, SW_SHOWMINIMIZED ); 

		AttachThreadInput( dwCurID, dwForeID, FALSE);
	}
	return bOk;
}