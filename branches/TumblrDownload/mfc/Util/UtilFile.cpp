//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//  Includes
//--------------------------------------------------------------------------------------------
#include <stdafx.h>
//#include <io.h>
#include "UtilFile.h"

#include <assert.h>
#include <set>
#include <io.h>

#include <windows.h>
#include <tchar.h>

#pragma warning(disable: 4996)

using namespace std;

///------------------------------------------------------------------------------
///  Module and debug definitions                                                
///------------------------------------------------------------------------------
//AT_DECLARE_FILE( Utils.cpp, "$Rev:$" )
namespace UtilFile
{
    //exe path, "x:\fdsfds" , no last "\\"
    wstring GetFilePath() 
    { 
		wstring wstr;
        TCHAR  exepath[MAX_PATH];
        //CString  strdir,tmpdir; 
        memset(exepath,0,MAX_PATH); 
        DWORD ret = GetModuleFileName(NULL, exepath, MAX_PATH); 
        wstr = exepath; 
		size_t pos = wstr.rfind('\\');
		if(pos < wstr.length() ){
			wstr = wstr.substr(0, pos);
		}
        //strdir=tmpdir.Left(tmpdir.ReverseFind('\\'));
        //wstring wstr = strdir.GetString();
        return wstr; 
    } 

    //return path like="xxx/xxx" , without "/" at last.
    tstring ExtractFilePath(const tstring& path)
    {
        size_t pos = path.find_last_of(_T("\\/"));

        if (pos != tstring::npos
			&& pos < path.size() )
        {
            return path.substr(0, pos);
        }
        return _T("");
    }

    tstring ExtractFileName(const tstring& path)
    {
        size_t pos1 = path.rfind('\\');
        size_t pos2 = path.rfind('/');

        pos1 = min(pos1, pos2);
        if (pos1 != tstring::npos)
        {
            return path.substr(pos1 + 1);
        }
        return path;
    }

	string ExtractFileName(const string& path)
	{
		size_t pos1 = path.rfind('\\');
		size_t pos2 = path.rfind('/');

		pos1 = min(pos1, pos2);
		if (pos1 != string::npos)
		{
			return path.substr(pos1 + 1);
		}
		return path;
	}

    tstring ExtractFileExt(const tstring& path)
    {
        size_t pos = path.find_last_of(_T("\\/."));

        if (pos != tstring::npos && path[pos] == '.')
        {
            return path.substr(pos + 1);
        }
        return _T("");
    }

    bool FileExists(const tstring& path)
    {
        DWORD dwAttribute  = 0;
        try
        {

            dwAttribute = ::GetFileAttributes(path.c_str());

            if (dwAttribute == INVALID_FILE_ATTRIBUTES)
            {
                return false;
            }
        }
        catch (...)
        {
			assert(false);
        }
        return (dwAttribute & (FILE_ATTRIBUTE_DEVICE | FILE_ATTRIBUTE_DIRECTORY)) == 0;
    }


    bool DirectoryExists(const tstring& path)
    {

        DWORD dwAttribute = 0;
        try
        {
            dwAttribute  = ::GetFileAttributes(path.c_str());

            if (dwAttribute == INVALID_FILE_ATTRIBUTES)
            {
                return false;
            }
        }
        catch (... )
        {
			assert(false);
        }
        return (dwAttribute & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
    }

    UINT64 GetFileSize(const tstring& path)
    {
        UINT64 fileSize64 = 0;

        try
        {
            HANDLE hFile = ::CreateFile(path.c_str(), 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            if(!hFile || INVALID_HANDLE_VALUE == hFile){
                return 0;
            }
            DWORD filesizeHi=0;
            DWORD fileSizeLow = ::GetFileSize(hFile, &filesizeHi);
            if(filesizeHi> 0){
                fileSize64 = filesizeHi;
                fileSize64 = fileSize64<<32;
                fileSize64 += fileSizeLow;
            }    
            else{
                fileSize64 = fileSizeLow;
            }
            ::CloseHandle(hFile);
        }
        catch (...)
        {
			assert(false);
        }      

        return fileSize64;
    }
    //////////////////////////////////////////////////////////////////////////

    UINT64 GetFileTimeGmtByMfc(const tstring& path)
    {
        UINT64 fileTime = 0;
        //        
        //if(path.length() < 1)
        //    return 0;
        //try
        //{
        //    CFileStatus fileSta;            
        //    BOOL IFOK = CFile::GetStatus(path.c_str(), fileSta) ;
        //    if(!IFOK){
        //        ASSERT(false);
        //        return 0;
        //    }

        //    //m_time = fileSta.m_mtime.GetTime();
        //    //SYSTEMTIME systime;
        //    //ifok = fileSta.m_mtime.GetAsSystemTime(systime);	
        //    tm tm0;
        //    tm* ptm = fileSta.m_mtime.GetGmtTm(&tm0);  // last modification date/time of file
        //    fileTime = mktime(ptm);
        //}
        //catch (CException* )
        //{
        //}
       
        return fileTime;
    }

    UINT64 ConvertFileTimeToUint64(const FILETIME& fileTime)
    {
        UINT64 time64;
        memcpy_s(&time64, sizeof(time64), &fileTime ,  sizeof(FILETIME) );
        return time64;
    }

    bool ConvertUint64ToFileTime(FILETIME& fileTime, UINT64 time64)
    {
        //UINT64 time64;
        memcpy_s(&fileTime, sizeof(FILETIME), &time64 ,  sizeof(time64) );
        return true;
    }

    UINT64 GetFileTimeGmt(const tstring& path)
    {
        UINT64 fileTime = 0;

        if(path.length() < 1)
            return 0;

        try
        {          
            HANDLE hFile = ::CreateFile(path.c_str(),              
                GENERIC_READ,  
                FILE_SHARE_READ,                      
                NULL,                   
                OPEN_EXISTING,         
                FILE_ATTRIBUTE_NORMAL,
                NULL);  
            if(!hFile 
                || hFile == INVALID_HANDLE_VALUE) 
                return 0;

            FILETIME modifyTime;
            BOOL IFOK  =::GetFileTime(hFile, NULL, NULL, &modifyTime);         
            if(!IFOK){
                //ASSERT(false);
                return 0;
            }
            fileTime = ConvertFileTimeToUint64(modifyTime);
            //CString strTime;

            //(1)
            //FileTimeToLocalFileTime(&fCreateTime,&localTime);//将文件时间转换为本地文件时间
            //FileTimeToSystemTime(&localTime, &sysTime);//将文件时间转换为本地系统时间

            //(2)
            //FileTimeToSystemTime(&fCreateTime, &sysTime);//将文件时间转换为标准系统时间
            //SystemTimeToTzSpecificLocalTime(&sysTime, &sysTime);//将标准系统时间转换为本地系统时间

            //strTime.Format(_T("%4d-%2d-%2d %2d:%2d:%2d"),
            //    sysTime.wYear,
            //    sysTime.wMonth,
            //    sysTime.wDay,
            //    sysTime.wHour,
            //    sysTime.wMinute,
            //    sysTime.wSecond
            //    );
            ::CloseHandle(hFile);

        }
        catch (... )
        {
			assert(false);
        }

        return fileTime;
    }

    bool SetFileTimeGmtByMfc(const tstring& path, UINT64 gmtTime)
    {
        if(path.length() < 1)
            return false;

        //BOOL fok = true;
        //try{
        //    CFileStatus fileSta;            
        //    fok = CFile::GetStatus(path.c_str(), fileSta) ;
        //    if(!fok){
        //        ASSERT(false);
        //        return false;
        //    }

        //    CFileTime fileTimeGmt(gmtTime);
        //    CFileTime fileTimeLocal = fileTimeGmt.UTCToLocal();
        //    UINT64 u64time = fileTimeLocal.GetTime();
        //    fileSta.m_mtime = u64time;

        //    CFile::SetStatus(path.c_str(), fileSta) ;
        //}
        //catch(CException* )
        //{
        //    return false;
        //}
        //return fok==TRUE;
		return false;
    }

    //use winapi
    bool SetFileTimeGmt(const tstring& path, UINT64 gmtTime )
    {
        if(path.length() < 1)
            return false;

        bool ifok = true;
        HANDLE hFile = ::CreateFile(path.c_str(),              
            GENERIC_WRITE|GENERIC_READ,  
            FILE_SHARE_READ,                      
            NULL,                   
            OPEN_EXISTING,         
            FILE_ATTRIBUTE_NORMAL,
            NULL);  
        if(!hFile 
            || hFile == INVALID_HANDLE_VALUE) 
            return 0;

        FILETIME modifyTime;
        bool fConvert = ConvertUint64ToFileTime(modifyTime, gmtTime);
        if(fConvert){
            BOOL IFOK  =::SetFileTime(hFile, NULL, NULL, &modifyTime);    
            if(!IFOK){
                //ASSERT(false);
                ifok = false;
            }
        }
        else{
            ifok = false;
        }             
        ::CloseHandle(hFile);
       
        return true;
    }
    //////////////////////////////////////////////////////////////////////////


    bool CreateDirectory(const tstring& path)
    {
        size_t  pos   = 0;
        int     index = 0;
        tstring subPath;

        try
        {
            while((pos = path.find_first_of(_T("\\/"), pos)) != tstring::npos)
            {
                index++;
                pos++;
                if (index == 1)
                {
                    continue;
                }
                subPath = path.substr(0, pos - 1);
                if (!DirectoryExists(subPath))
                {
                    if (::CreateDirectory(subPath.c_str(), NULL) == FALSE)
                    {
                        return false;
                    }
                }
            }
            if (!DirectoryExists(path))
            {
                return ::CreateDirectory(path.c_str(), NULL) != FALSE;
            }
        }
        catch (... )
        {
            assert(false);
        }
       
        return true;
    }

    bool ClearDirectory(const tstring& path)
    {
		tstring strDir = path;
        WIN32_FIND_DATA wfd;
        tstring strFileWildCard = strDir+_T("\\*.*");
        HANDLE hFile=FindFirstFile(strFileWildCard.c_str(),(WIN32_FIND_DATA*)&wfd);   
        if(hFile != INVALID_HANDLE_VALUE)   
        {   
            do   
            {   
                if(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)   
                {   
                    tstring   strFolderName=wfd.cFileName;   
                    if(strFolderName!=_T(".")
                        &&strFolderName!=_T("..") )   
                    {   
                        tstring strTempPath = strDir+_T("\\")+strFolderName;
                        ClearDirectory(strTempPath.c_str() );   

                        RemoveDirectory(strTempPath.c_str() );
                    }   
                }   
                else   
                    DeleteFile(strDir+_T("\\"+wfd.cFileName));
            }while(FindNextFile(hFile,&wfd));   
            FindClose(hFile);   
			return true;
        }
		return false;
        //RemoveDirectory(strDir.c_str() );
    }
    //////////////////////////////////////////////////////////////////////////

    //replace global value, /  //
    tstring ExpandPath(const tstring& path)
    {
        tstring  result = path;
        size_t   pos    = 0;

        while((pos = result.find(_T("$("), pos)) != tstring::npos)
        {
            size_t posEnd = result.find(_T(")"), pos);

            if (posEnd == tstring::npos)
            {
                break;
            }
            tstring envName   = result.substr(pos + 2, posEnd - pos - 2);
            TCHAR*  envValPtr = _tgetenv(envName.c_str());
            tstring envVal    = envValPtr ? envValPtr : _T("");
            result.replace(pos, posEnd - pos + 1, envVal);
            pos += envVal.length();
        }
        while((pos = result.find(_T("/"))) != tstring::npos)
        {
            result.replace(pos, 1, _T("\\"));
        }
        while((pos = result.find(_T("\\\\"))) != tstring::npos)
        {
            result.replace(pos, 2, _T("\\"));
        }
        return result;
    }

    bool DeleteFile(const tstring& path)
    {
        if(path.empty() )
            return false;
        BOOL IFOK = FALSE;
        try
        {
            IFOK = ::DeleteFile(path.c_str() );
        }
        catch (... )
        {
            assert(false);
        }
        return IFOK == TRUE;
    }

	bool DeleteFileThroughPostfix(const tstring& path, const tstring& postfixName)
	{
		std::wstring wstrDeleteFiles = path;
		wstrDeleteFiles = wstrDeleteFiles + _T("\\*.");
		wstrDeleteFiles = wstrDeleteFiles + postfixName;
		std::set<tstring> tstrSet;
		GetFileList(wstrDeleteFiles, tstrSet);
		std::set<tstring>::const_iterator it = tstrSet.begin();
		for(;it != tstrSet.end();it++)
		{
			DeleteFile(*it);
		}
		return true;
	}

    bool MoveFile(const tstring& pathOld, const tstring& pathDest)
    {
        if(pathOld.empty()
            || pathDest.empty()
			|| pathOld == pathDest
			)
            return false;

        BOOL fok = FALSE;
        try
        {	
			fok = FileExists(pathDest);
			if(fok){
				//delete it
				fok = DeleteFile(pathDest);
				if(!fok)
					return false;
			}
            fok = ::MoveFile(pathOld.c_str(), pathDest.c_str() );
        }
        catch (... )
        {
            assert(false);
        }
        return fok == TRUE;
    }

    bool CopyFile(const tstring& pathOld, const tstring& pathDest)
    {
        BOOL fok = FALSE;
        try
        {
            fok = ::CopyFile(pathOld.c_str(), pathDest.c_str(), TRUE );
        }
        catch (... )
        {
            assert(false);
        }
        return fok == TRUE;
    }

    bool CompareFileTime(const tstring& file1, const tstring& file2, int& result)
    {
        bool   res = false;

        try
        {
            HANDLE hFile1 = ::CreateFile(file1.c_str(), 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            HANDLE hFile2 = ::CreateFile(file2.c_str(), 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            
            result = 0;
            if (hFile1 != INVALID_HANDLE_VALUE && hFile2 != INVALID_HANDLE_VALUE)
            {
                ULARGE_INTEGER time1;
                ULARGE_INTEGER time2;

                if (::GetFileTime(hFile1, NULL, NULL, (PFILETIME)&time1) &&
                    ::GetFileTime(hFile2, NULL, NULL, (PFILETIME)&time2))
                {
                    res = true;
                    result = (time1.QuadPart < time2.QuadPart) ? -1 : ((time1.QuadPart > time2.QuadPart) ? 1 : 0);
                }
            }
            if (hFile1 != INVALID_HANDLE_VALUE)
            {
                CloseHandle(hFile1);
            }
            if (hFile2 != INVALID_HANDLE_VALUE)
            {
                CloseHandle(hFile2);
            }
        }
        catch (... )
        {
			assert(false);
        }
        
        return res;
    }

    bool HasWildcards(const tstring& path)
    {
        tstring name = ExtractFileName(path);
        return name.find_first_of(_T("*?")) != tstring::npos;
    }

    bool GetFileList(const tstring& maskSrc, std::set<tstring>& files)
    {
		tstring strMask = maskSrc;
		if(strMask.empty() ){
			strMask = _T("*.*");
		}

        struct _tfinddata_t file;
        intptr_t            hFile;
        tstring             dirPath = ExtractFilePath(strMask);
        size_t              initialSize = files.size();
		

        try
        {
            if( (hFile = _tfindfirst( strMask.c_str(), &file )) != -1L )
            {
                do 
                {
                    tstring fileName = dirPath + _T("\\") + file.name;
                    files.insert(fileName);
                } 
                while( _tfindnext( hFile, &file ) == 0 );
                _findclose( hFile );
            }
        }
        catch (... )
        {
			assert(false);
        }
        
        return initialSize != files.size();
    }

	bool        GetFileList(const tstring& mask, std::set<std::string>& files)
	{
		return false;
	}

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// Returns the path to the temporary folder. The path is terminated with a backslash, e.g.
    /// "c:\temp\"
    /// 
    /// @retval tstring     - the temporary path.
    //////////////////////////////////////////////////////////////////////////////////////////////
    tstring GetTempPath( )
    {
        tstring resultPath;
        // Reserve the size first.
        resultPath.resize( ::GetTempPath( 0, NULL ) + 1 ); // GetTempPath will return the size for the null terminator.

        DWORD size = ::GetTempPath( (DWORD)resultPath.capacity(), &resultPath[0] );

        resultPath.resize( size );

        return resultPath;
    }
    
    bool ReadFileAsString(const std::wstring& fileName, std::string& strData)
    {
        if(fileName.length()< 1)
            return false;

        bool ifok = true;
        try
        {
			const UINT64 fileLen64 = GetFileSize(fileName);

            HANDLE hFile = ::CreateFile(fileName.c_str(), GENERIC_READ , 0,NULL, OPEN_EXISTING,  FILE_ATTRIBUTE_NORMAL,
                NULL );    
            if(!hFile || INVALID_HANDLE_VALUE == hFile)
                return false;
            DWORD lasterr = ::GetLastError();
			

            DWORD readedLen = 0;
			UINT64 oldReadedLen = 0;

			//pageLen 2G-1
			const UINT64 pageLen = (0x80000000) - 1; //2g - 1

			UINT64 unreadLen = pageLen;
			if (unreadLen > fileLen64)
				unreadLen = fileLen64;
            const UINT64 toreadLen = fileLen64;

			//todo ,check if size > memsize.
            strData.resize(fileLen64);
            if(strData.size() < fileLen64)
            {
				assert(false);
                goto Done;
            }

			while(1)
			{
				BOOL ifokBIG = ::ReadFile(hFile, &(strData[oldReadedLen]), unreadLen, &readedLen, 0 );
				if (ifokBIG)
				{
					oldReadedLen += readedLen;
					unreadLen = toreadLen - oldReadedLen;
					if (unreadLen > pageLen)
						unreadLen = pageLen;
					if (oldReadedLen == toreadLen)
						break;
					else if(oldReadedLen > toreadLen)
					{
						lasterr = GetLastError();
						ifok = false;
						goto Done;
					}
					else
					{
						//::FlushFileBuffers(hFile);
						//::SetFilePointer(hFile,readedLen,NULL,FILE_CURRENT);
						continue;
					}
				}
				else
				{
					lasterr = GetLastError();
					ifok = false;
					goto Done;
				}
			}
Done:
            CloseHandle(hFile);
        }
        catch (... )
        {
            assert(false);
            return false;
        }
       
        return ifok;
    }

    bool WriteFileAsString(const std::wstring& fileName, const std::string& strData)
    {
        if(fileName.length()< 1)
            return false;

        bool ifok = true;
        try
        {
            //get path
            wstring wstrPath = ExtractFilePath(fileName);
            if(wstrPath.length() )   {
                bool fDirExist = DirectoryExists(wstrPath);
                if(!fDirExist){
                    fDirExist = CreateDirectory(wstrPath);
                    //ASSERT(fDirExist);
                }
            }

            HANDLE hFile = ::CreateFile(fileName.c_str(), GENERIC_WRITE , 0,NULL, 
                OPEN_ALWAYS,  FILE_ATTRIBUTE_NORMAL,   NULL );    
            if(!hFile || INVALID_HANDLE_VALUE == hFile)
                return false;
            DWORD lasterr = ::GetLastError();
            DWORD writtenLen = 0;
            DWORD towriteLen = (DWORD)strData.size();

            BOOL ifokBIG = ::WriteFile(hFile, &(strData[0]), towriteLen, &writtenLen, 0 );
            if(!ifokBIG || towriteLen!= writtenLen)
            {
                lasterr = GetLastError();
                goto Done;
            }
Done:
            CloseHandle(hFile);
        }
        catch (... )
        {
			assert(false);
			ifok = false;
        }
        
        return ifok;
    }


} //namespace
