#pragma once

#include "comDll.h"
#include<vector>
#include "threadpool\UtilRunable.h"
using namespace UtilThreadPool;

class COM_DLL CActiveWin : public IRunable
{
public:
	static CActiveWin& GetInstance();
	static void DestroyInstance();

	CActiveWin(void);
	~CActiveWin(void);

	virtual void run();

protected:
	HWND FindHwnd();
	BOOL ActiveWindows();
	void AddHwnd(HWND hwnd);
	static BOOL CALLBACK EnumWndsProc(HWND hwnd, LPARAM lParam);

private:
	static CActiveWin* g_pThis;
	std::vector<HWND> m_vecHwnd;
};

