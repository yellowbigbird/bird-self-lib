#pragma once

//#include "comDll.h"
#include <list>

class CFileInfo
{
public:
	CFileInfo();
	~CFileInfo();

	void	Clear();
	//bool	ReadFileToBuffer();
    //bool    WriteBufferToFile();
    
    //bool    SetData(const std::string& data, bool f7z);  

    //const std::string&  GetDataRaw() const;
    //bool    GetData(std::string& strData, bool f7z) const;

	//bool	ReadAllInfo(const std::wstring& wstrRootPath);

    //void    GenerateFullPathName();

public:
    //bool			m_ifCached;
    bool            m_fDir;

    //UINT64			m_size;
    //UINT64			m_time;
	//UINT			m_fileDataIdx;
	std::wstring	m_strName;
	std::wstring	m_strRelativePath;  //"\src\cini",  
	std::wstring	m_strFullPathName;
    
    std::list<CFileInfo>        m_files; 
protected:
	//std::string		m_strData;          //raw data
  

};
