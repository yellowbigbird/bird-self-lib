#include "stdafx.h"
#include "FileInfo.h"

#include "DebugFile.h"
#include "config.h"

#include <Util/Crc32.h>

#include "util/UtilString.h"
#include "util/UtilsFile.h"

#define  USE_7Z  0
#if USE_7Z
#include <lzmaWrapper.h>
#endif //USE_7Z

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
//using namespace DebugFile;
//////////////////////////////////////////////////////////////////////////
CFileInfo::CFileInfo()
:m_ifCached(false)
,m_time(0)
,m_size(0)
,m_fImage(0)
,m_hashCrc32(0)
,m_hashImage(0)
{

}

CFileInfo::~CFileInfo()
{
}

void	CFileInfo::Clear()
{
	m_ifCached = (false);
	m_time= (0);
	m_size = (0);
	m_strName = _T("");
	m_strRelativePath = m_strName;
	m_strFullPathName = m_strName;
	m_strData = "";
}
//////////////////////////////////////////////////////////////////////////

bool	CFileInfo::ReadFileToBuffer()
{
    bool ifok =false;
    //const bool f7z = CConfig::GetInstance().GetIs7z();

    m_ifCached = false;     
        
    m_size = UtilFile::GetFileSize(m_strFullPathName);
    ifok = UtilFile::ReadFileAsString(m_strFullPathName.c_str(), m_strData);  
    if(!ifok)
        return ifok;
    m_ifCached = true;

	return ifok;
}

bool   CFileInfo::SetData(const std::string& data, bool f7z)
{
    string strDataRaw = data;

    if(f7z){
#if USE_7Z
        //unwrap
        CLzmaWrapper wrapper;

        bool ifok = wrapper.Decode(data, strDataRaw);
        if(!ifok){
            AddDebug("file decode error.");
            wstring wstr = _T("decode_error.log");
            UtilFile::WriteFileAsString(wstr, data);
            return false;
        }
#endif
    }

    m_strData = strDataRaw;   

    return true;
}

bool  CFileInfo::GetData(std::string& strData, bool f7z) const
{
    if(f7z)    {
#if USE_7Z
        CLzmaWrapper wrapper;
        bool ifok = wrapper.Encode(m_strData, strData);
        if(!ifok){
            AddDebug("file encode error.");
            return false;
        }
        //test
        string strDecode;
        ifok = wrapper.Decode(strData, strDecode);
        if(!ifok
            || strDecode != m_strData
            ){
				AddDebug("file encode, test decode error.");
				wstring wstr = m_strName;
				wstr += _T(".encode.bin");
				UtilFile::WriteFileAsString(wstr, strData);
				wstr = m_strName;
				wstr += _T(".decode.bin");
				UtilFile::WriteFileAsString(wstr, strDecode);
        }
#endif    
    }
    else{
        strData = m_strData;
    }
    return true;;
}

void   CFileInfo::GenerateFullPathName()
{
    wstring wstr;
    wstr = CConfig::Get().GetRootPath();
    //wstr += _T("\\");
    wstr += m_strRelativePath;
    wstr += _T("\\");
    wstr += m_strName;
    if(wstr.length() < 1)
        return;
    m_strFullPathName = wstr;
}

bool CFileInfo::WriteBufferToFile()
{
    bool ifok = false;
    wstring wstr = m_strFullPathName;
    if(wstr.length() < 1)
    {
        GenerateFullPathName();
        wstr = m_strFullPathName;
    }
    if(wstr.length()< 1)
        return false;
    
    ifok = UtilFile::WriteFileAsString(wstr ,m_strData);
    if(!ifok){
        AddDebug("WriteFileAsString error.");
        //return false;
    }

    //write file time
    if(m_time >0){
        ifok = UtilFile::SetFileTimeGmt(wstr, m_time);
        if(!ifok)
            AddDebug("set file time error.");
    }

    CString cstr;
    cstr.Format(_T("write file=%s, ret=%d"), wstr.c_str(), ifok);
    wstr = cstr.GetString();
    AddDebug(wstr);

    return ifok;
}

//////////////////////////////////////////////////////////////////////////
bool	CFileInfo::ReadAllInfo(const std::wstring& wstrRootPath)
{
	bool ifok = false;
    
	try
	{
		do 
		{			
			//file size
			m_size = UtilFile::GetFileSize(m_strFullPathName);
                //static_cast<UINT>(fileSta.m_size);

			//name 
			m_strName = UtilFile::ExtractFileName(m_strFullPathName);
			wstring wstrPath = UtilFile::ExtractFilePath(m_strFullPathName);
			int pos = (int)wstrPath.find(wstrRootPath);
			if(pos >=0 &&  pos< (int)wstrPath.length() )
			{
				m_strRelativePath = wstrPath.substr(wstrRootPath.length() );                
			}

			//get gmt time
			m_time = UtilFile::GetFileTimeGmt(m_strFullPathName);

			//get hash

			//get imghash
	
			ifok = true;
		} while (false);
		
	}
	catch (CFileException* )
	{
	}
	catch (CException* )
	{
	}
	return ifok;


}

