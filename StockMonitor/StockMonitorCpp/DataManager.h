#pragma once

class CDataManager
{
public:
	CDataManager();
	virtual ~CDataManager();

	void	SendRequest();

	HWND	m_hWnd; //to send update msg
};
