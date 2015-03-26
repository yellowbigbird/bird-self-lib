#include "stdafx.h"
#include "FileManager.h"

#include <util/UtilString.h>
#include "util/UtilsFile.h"

#include "config.h"
#include "DebugFile.h"

//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

CFileManager*	CFileManager::g_pThis = NULL;

void	FindAllFile(const std::wstring& strPath, std::vector<CFileInfo>& vecFiles);
//////////////////////////////////////////////////////////////////////////
CFileManager& CFileManager::GetInstance()
{
	if(!g_pThis)
	{
		g_pThis = new CFileManager();
        atexit(DetroyInstance);
		if(!g_pThis)
		{
			//log
		}
	}
	return *g_pThis;
}

void CFileManager::DetroyInstance()
{
    SAFE_DELETE(g_pThis);
}

CFileManager::CFileManager()
:m_mutex(FALSE, _T("CFileManager") )
,m_fSerching(false)
,m_fHasNewSearch(false)
{
}

CFileManager::~CFileManager()
{
}

//////////////////////////////////////////////////////////////////////////

void	CFileManager::StartSearchPath()
{
    const wstring wstr = CConfig::GetInstance().GetRootPath();
    if(wstr.length() < 1)
        return;
	m_fHasNewSearch = true;    
}

bool	CFileManager::StartSearchTread()
{
	DWORD dwThreadId = 0;
	HANDLE hThread = ::CreateThread(NULL, 0, CFileManager::ThreadFunc, (LPVOID*)this, 0, &dwThreadId);
	if (!hThread)
	{
		m_fThreadRunning = false;
		return false;
	}    
    m_fThreadRunning = true;
	CloseHandle(hThread);	
	return true;

}

DWORD  WINAPI CFileManager::ThreadFunc(void* pServer)
{
	CFileManager* pthis = (CFileManager*)(pServer);
	if(!pthis)
		return 0;
	while(pthis->m_fThreadRunning)
	{        
        pthis->SearchAllFiles();
		Sleep(1000);
	}
	return 0;
}


void	CFileManager::SearchAllFiles()
{
    const wstring wstrRootPath = CConfig::GetInstance().GetRootPath();

    if(m_fHasNewSearch){
        m_fHasNewSearch = false;
        m_fSerching = true;
        
        //m_vecFiles.clear();

        vector<CFileInfo> vecFiles;
        FindAllFile(wstrRootPath, vecFiles);   

        m_mutex.Lock(100);
        m_vecFiles = vecFiles;
        m_mutex.Unlock();

        const size_t filenum = m_vecFiles.size();
        CString cstr;
        cstr.Format(_T("SearchAllFiles ok, %d files found."), filenum);
        AddDebug(cstr);
    }
    m_fSerching = false;
}

void CFileManager::FindAllFile(const wstring& strPath,  vector<CFileInfo>& vecFiles)  
{   
    if(strPath.length() < 1)
        return;

	CString cstrFind = strPath.c_str();
		cstrFind +=  _T("\\*.*");

	WIN32_FIND_DATA wfd;  
	HANDLE hFind = ::FindFirstFile(cstrFind, &wfd);  
	if(hFind==INVALID_HANDLE_VALUE)  
		return ;

	string str;
	TCHAR tcharFile[MAX_PATH]; 
	CFileInfo entry;
	bool ifok = false;	
	const wstring wstrRoot = CConfig::GetInstance().GetRootPath();
    
	do  
	{  
        if(m_fHasNewSearch)
            goto End;
		//如果你所在的不是根目录，你将会看到“.”与“..”这两个目录——这是在资源管理器中看不到的。  
		//dos下一个点代表的是当前目录，两个点代表的是上一级目录。若查找的到的是当前文件夹和上一级文件夹，则忽略。  
		if(wfd.cFileName[0]=='.')  
			continue;  
		if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)  
		{  
			//dir  
			wsprintf(tcharFile,_T("%s\\%s"), strPath.c_str(), wfd.cFileName);  
			FindAllFile(tcharFile, vecFiles);   //recursive call
		}  
		else   
		{  
			//file 
			wsprintf(tcharFile,_T("%s\\%s"), strPath.c_str() , wfd.cFileName); 
			
			entry.Clear();
			entry.m_strFullPathName = tcharFile;
			ifok = entry.ReadAllInfo(wstrRoot);

			vecFiles.push_back(entry);
			//m_vecFileData.push_back("");
             
		}  
	}while(::FindNextFile(hFind,&wfd));  

End:
	::FindClose(hFind);  
}  

void	CFileManager::GetFileList(vector<CFileInfo>& vecFiles)
{
	m_mutex.Lock(60*1000) ;
	do
	{
		vecFiles.clear();
		vecFiles = m_vecFiles;

	}while(false);
	m_mutex.Unlock();
}

//bool	CFileManager::ReadFileByIdx(UINT fileIdx)
//{
//	if(fileIdx>= m_vecFileData.size() )
//		return false;
//	m_mutex.Lock(10*1000);
//
//	string& str = m_vecFileData[fileIdx];
//	CFileInfo& entry = m_vecFiles[fileIdx];
//
//	m_mutex.Unlock();
//}

bool	CFileManager::FindFileByName(const std::string& strFileName, UINT& fileId)
{
	bool iffind = false;

	m_mutex.Lock(10*1000) ;
	do
	{
		CString cstr0, cstr1;
		
		for(UINT fileIdx =0; fileIdx< m_vecFiles.size(); fileIdx++)
		{
			const CFileInfo& entry = m_vecFiles[fileIdx];
			//all to lowcase
			cstr0 = strFileName.c_str();
			cstr1 = entry.m_strName.c_str();
			cstr0.MakeLower();
			cstr1.MakeLower();
			if(cstr0 == cstr1)
			{
				fileId = fileIdx;
				iffind = true;
				break;
			}
		}
	}while(false);
	m_mutex.Unlock();
	return iffind;
}

bool	CFileManager::GetFile(CFileInfo& infoDest, const std::string& strFileName)
{
	UINT fileid = 0;
	bool iffind = FindFileByName(strFileName, fileid);
	if(!iffind)
		return iffind;
	bool ifok = false;
	
	m_mutex.Lock(10*1000) ;
	{
		CFileInfo& infoSrc = m_vecFiles[fileid];
		ifok = infoSrc.ReadFileToBuffer();
		if(ifok)
			infoDest = infoSrc;
	}
	m_mutex.Unlock();	

	return ifok;

}

bool	CFileManager::GetFile(CFileInfo& rfino, const UINT fileIdx)
{
    bool ifok = false;
    if(fileIdx>= m_vecFiles.size() )
        return ifok;

    m_mutex.Lock(10*1000) ;
    {
        rfino = m_vecFiles[fileIdx];
        ifok = true;
    }
    m_mutex.Unlock();	

    return ifok;
}

//////////////////////////////////////////////////////////////////////////
bool   CFileManager::AddFile(const CFileInfo& rfino, UINT& fileIdx)
{
    bool ifok = false;

    m_mutex.Lock(5*1000) ;
    {
        m_vecFiles.push_back(rfino);
        fileIdx = (UINT)m_vecFiles.size() -1;        
    }
    m_mutex.Unlock();	
    ifok =true;

    return ifok;
}

//////////////////////////////////////////////////////////////////////////

