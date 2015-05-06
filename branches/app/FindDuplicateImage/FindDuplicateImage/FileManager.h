#pragma once

#include "FileInfo.h"
//#include "comDll.h"

class CFileManager
{
public:
	static  CFileManager& Get();
    static void DetroyInstance();

	//void	SetPath(const std::wstring& strPath);
	void	StartSearchPath();
	bool	StartSearchTread();
	void	SearchAllFiles();
	void	FindAllFile(const std::wstring& strPath, std::vector<CFileInfo>& vecFiles);

	bool	FindFileByName(const std::string& strFileName, UINT& fileId);

	//bool	StartThread();
	bool	GetFile(CFileInfo& rfino, const std::string& strFileName);
    bool	GetFile(CFileInfo& rfino, const UINT fileIdx);
    bool    AddFile(const CFileInfo& rfino, UINT& fileIdx);

	void	GetFileList(std::vector<CFileInfo>& vecFiles);
	bool	ReadFileByIdx(UINT fileIdx);

protected:
	CFileManager();
	virtual ~CFileManager();

	static DWORD  WINAPI ThreadFunc(void* pServer);

protected:
	CMutex						m_mutex;
    bool                        m_fThreadRunning;
    bool                        m_fSerching;
    bool                        m_fHasNewSearch;
	//std::wstring				m_strRootPath;	//"x:\\fdsfd
	static CFileManager*		g_pThis;

	std::vector<CFileInfo>		m_vecFiles;
	//std::vector<std::string>	m_vecFileData;
};