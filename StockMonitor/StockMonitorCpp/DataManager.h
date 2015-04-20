#pragma once

class CDataManager
{
public:	
	virtual ~CDataManager();
	static CDataManager&	Get();

	void	Init();

	bool	StartThread();
	bool	SendRequest();

protected:
	CDataManager();
	DWORD	WINAPI Thtreadfunc_updateData();
	
public:
	HWND	m_hWnd; //to send update msg
	static CDataManager*	m_pThis;
	CHttpSocket*	m_pHttp;
};
