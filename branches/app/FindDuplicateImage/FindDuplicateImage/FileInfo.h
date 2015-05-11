#pragma once

class CFileInfo
{
public:
	CFileInfo();
	~CFileInfo();

	void	Clear();
	bool	ReadFileToBuffer();
    bool    WriteBufferToFile();
    
    bool    SetData(const std::string& data, bool f7z);  

    //const std::string&  GetDataRaw() const;
    bool    GetData(std::string& strData, bool f7z) const;

	bool	ReadAllInfo(const std::wstring& wstrRootPath);
	
    void    GenerateFullPathName();

protected:
	bool	ReadImageInfo();

public:
    bool			m_ifCached;
	
    UINT64			m_size;
    UINT64			m_time;
	UINT32			m_hashCrc32;
	
	//image
	bool			m_fImage;
	UINT32			m_imgW;
	UINT32			m_imgH;
	UINT64			m_hashImage;

	//UINT			m_fileDataIdx;
	std::wstring	m_strName;
	std::wstring	m_strRelativePath;  //"\src\cini",  
	std::wstring	m_strFullPathName;
    

protected:
	std::string		m_strData;          //raw data

};