#include "Stdafx.h"
#include "DebugFile.h"

#include "util/UtilFile.h"
#include "util/UtilString.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////////

CDebug* CDebug::g_pThis = NULL;

CDebug::CDebug()
:m_mutex(FALSE, _T("CDebug") )
,m_strFileFullPathName(_T("debug.log") )
,m_hWndToUpdate(NULL)
{
}

CDebug::~CDebug()
{
}

CDebug& CDebug::GetInstance()
{
	if(!g_pThis)
	{
		g_pThis = new CDebug();
        atexit(DetroyInstance);
	}
	return *g_pThis;
}

void CDebug::DetroyInstance()
{
    SAFE_DELETE(g_pThis);
}

void CDebug::SetLogFilePathName(const std::wstring& wstrName)
{
	m_strFileFullPathName = wstrName;
}


void CDebug::AddDebug(const std::wstring& wstrWithoutTime)
{
	const wstring wstr = GetTimeString() + _T(" ")+wstrWithoutTime ;

	if(m_strFileFullPathName.length()< 1)
		return;
	
	m_mutex.Lock(5000);
	try
	{
		//write file
		do 
		{		
			CStdioFile file0;
			BOOL ifok =FALSE;
			if(!UtilFile::FileExists(m_strFileFullPathName) )
			{
				ifok = file0.Open(m_strFileFullPathName.c_str(), 
					CFile::modeCreate); 
				file0.Close();
			}
			ifok = file0.Open(m_strFileFullPathName.c_str(), 
				CFile::modeWrite|CFile::typeText);  //|CFile::modeCreate
			if(!ifok)
				break;
			file0.Seek(0,CFile::end);
			file0.WriteString(wstr.c_str() );
			file0.WriteString(_T("\r") );
			file0.Close();

            m_vecStr.push_back(wstr);
		} while (false);		
	}
	catch (CFileException* )
	{
	}	
	m_mutex.Unlock();

    //update window
    if(m_hWndToUpdate)    {
        ::PostMessage(m_hWndToUpdate, WM_UPDATE_DEBUG, 0 , 0 );
    }
}

wstring CDebug::GetTimeString()
{
	wstring strtime;

	SYSTEMTIME systime;
	::GetLocalTime(&systime);

	//change to string
	CString cstr;
	cstr.Format(_T("[%02d:%02d_%02d:%02d:%02d]"),
		systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond);
	strtime = cstr;
	return strtime;
}

void CDebug::GetVecString(vector<wstring>& vecString) const
{
        vecString = m_vecStr;   
}

void  CDebug:: SetUpdateWindow(HWND hWindow)
{
    m_hWndToUpdate = hWindow;
}

//////////////////////////////////////////////////////////////////////////
//namespace DebugFile
//{
    void AddDebug(const std::string& str)
    {
        //add time
        wstring wstr = UtilString::ConvertMultiByteToWideChar(str);
        CDebug::GetInstance().AddDebug(wstr);
    }

    void AddDebug(const std::wstring& wstr)
    {
        CDebug::GetInstance().AddDebug(wstr);
    }

    void AddDebug(const CString& cstr)
    {
        wstring wstr = cstr.GetString();
        CDebug::GetInstance().AddDebug(wstr);
    }

	void AddDebug(const char* strMsgFormat, ...)
	{
		if(!strMsgFormat)
			return;

		va_list vargs;
		char	szMessage[1024];

		va_start(vargs, strMsgFormat);
		int ret = sprintf_s(szMessage, 1024, strMsgFormat, vargs);
		va_end(vargs);

		const string str = szMessage;
		AddDebug(str);
	}

	void AddDebug(const TCHAR* strMsgFormat, ...)
	{
		if(!strMsgFormat)
			return;

		va_list vargs;
		TCHAR	szMessage[1024];

		va_start(vargs, strMsgFormat);
		int ret = swprintf_s(szMessage, 1024, strMsgFormat, vargs);
		va_end(vargs);

		const wstring str = szMessage;
		AddDebug(str);
	}

//}