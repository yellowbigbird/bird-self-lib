//////////////////////////////////////////////////////////////////////////////////////////////
///
/// @file Utils.cpp
/// @brief 
///
/// Contains implementation for the utilities functions
///
/// @author Abalta Technologies, Inc.
/// @date January, 2007
///
/// @cond Copyright
///
/// COPYRIGHT 2007 "DECARTA INC" ALL RIGHTS RESERVED.<br>
/// This program may not be reproduced, in whole or in part in any form or any means whatsoever
/// without the written permission of "DECARTA INC".
///
/// @endcond
//////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------
//  Includes
//--------------------------------------------------------------------------------------------
#include <stdafx.h>
#include "Utils.h"

#include <io.h>
#include <assert.h>

#pragma warning(disable: 4996)

using namespace std;
using namespace CUtils;

///------------------------------------------------------------------------------
///  Module and debug definitions                                                
///------------------------------------------------------------------------------
//AT_DECLARE_FILE( Utils.cpp, "$Rev:$" )

//--------------------------------------------------------------------------------------------
//  Local type declarations and macros
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
//  Local Variables:  General Variables
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
//  Local Functions:  Prototypes
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
//  Implementation
//--------------------------------------------------------------------------------------------
//return path like="xxx/xxx" , without "/" at last.
tstring CUtils::ExtractFilePath(const tstring& path)
{
  size_t pos = path.find_last_of(_T("\\/"));

  if (pos != tstring::npos)
  {
    return path.substr(0, pos);
  }
  return path;
}

tstring CUtils::ExtractFileName(const tstring& path)
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

string CUtils::ExtractFileName(const string& path)
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

tstring CUtils::ExtractFileExt(const tstring& path)
{
  size_t pos = path.find_last_of(_T("\\/."));
 
  if (pos != tstring::npos && path[pos] == '.')
  {
    return path.substr(pos + 1);
  }
  return _T("");
}

bool CUtils::FileExists(const tstring& path)
{
  DWORD dwAttribute = ::GetFileAttributes(path.c_str());

  if (dwAttribute == INVALID_FILE_ATTRIBUTES)
  {
    return false;
  }
  return (dwAttribute & (FILE_ATTRIBUTE_DEVICE | FILE_ATTRIBUTE_DIRECTORY)) == 0;
}

UINT CUtils::GetFileSize(const tstring path)
{
  HANDLE hFile = ::CreateFile(path.c_str(), 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  if(!hFile || INVALID_HANDLE_VALUE == hFile){
    return 0;
  }
  DWORD filesizeHi=0;
  DWORD fileSize = ::GetFileSize(hFile, &filesizeHi);
  ::CloseHandle(hFile);
    
  return fileSize;
}

bool CUtils::DirectoryExists(const tstring& path)
{
  DWORD dwAttribute = ::GetFileAttributes(path.c_str());

  if (dwAttribute == INVALID_FILE_ATTRIBUTES)
  {
    return false;
  }
  return (dwAttribute & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
}

bool CUtils::CreateDirectory(const tstring& path)
{
  size_t  pos   = 0;
  int     index = 0;
  tstring subPath;

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
  return true;
}

//bool CUtils::DeleteDirectory(const TCHAR* charDir)
//{
//  assert(charDir);
//  if(!charDir)
//    return false;
//
//  tstring strDir = charDir;
//
//  WIN32_FIND_DATA   wfd;
//  tstring strFileWildCard = strDir+_T("\\*.*");
//  HANDLE			  hFile=FindFirstFile(strFileWildCard.c_str(),(WIN32_FIND_DATA*)&wfd);   
//  if(hFile!=INVALID_HANDLE_VALUE)   
//  {   
//    do   
//    {   
//      if(wfd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)   
//      {   
//        tstring   strFolderName=wfd.cFileName;   
//        if(strFolderName!=_T(".")
//          &&strFolderName!=_T("..") )   
//        {   
//          tstring strTempPath = strDir+_T("\\")+strFolderName;
//          DeleteDirectory(strTempPath.c_str() );   
//
//          RemoveDirectory(strTempPath.c_str() );
//        }   
//      }   
//      else   
//        DeleteFile(strDir+_T("\\"+wfd.cFileName));
//    }
//    while(FindNextFile(hFile,&wfd));   
//
//    FindClose(hFile);   
//  }
//
//  RemoveDirectory(strDir.c_str() );
//  return true;
//}
//////////////////////////////////////////////////////////////////////////

//replace global value, /  //
tstring CUtils::ExpandPath(const tstring& path)
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

bool CUtils::DeleteFile(const tstring& path)
{
  return ::DeleteFile(path.c_str()) == TRUE;
}

bool CUtils::CompareFileTime(const tstring& file1, const tstring& file2, int& result)
{
  HANDLE hFile1 = CreateFile(file1.c_str(), 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  HANDLE hFile2 = CreateFile(file2.c_str(), 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
  bool   res = false;

  res    = false;
  result = 0;
  if (hFile1 != INVALID_HANDLE_VALUE && hFile2 != INVALID_HANDLE_VALUE)
  {
    ULARGE_INTEGER time1;
    ULARGE_INTEGER time2;

    if (GetFileTime(hFile1, NULL, NULL, (PFILETIME)&time1) &&
        GetFileTime(hFile2, NULL, NULL, (PFILETIME)&time2))
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
  return res;
}

bool CUtils::HasWildcards(const tstring& path)
{
  tstring name = ExtractFileName(path);
  return name.find_first_of(_T("*?")) != tstring::npos;
}

bool CUtils::GetFileList(const tstring& mask, std::set<tstring>& files)
{
  struct _tfinddata_t file;
  intptr_t            hFile;
  tstring             dirPath = ExtractFilePath(mask);
  size_t              initialSize = files.size();

  if( (hFile = _tfindfirst( mask.c_str(), &file )) != -1L )
  {
    do 
    {
      tstring fileName = dirPath + _T("\\") + file.name;
      files.insert(fileName);
    } 
    while( _tfindnext( hFile, &file ) == 0 );
    _findclose( hFile );
  }
  return initialSize != files.size();
}

//////////////////////////////////////////////////////////////////////////////////////////////
/// Returns the path to the temporary folder. The path is terminated with a backslash, e.g.
/// "c:\temp\"
/// 
/// @retval tstring     - the temporary path.
//////////////////////////////////////////////////////////////////////////////////////////////
tstring CUtils::GetTempPath( )
{
  tstring resultPath;
  // Reserve the size first.
  resultPath.resize( ::GetTempPath( 0, NULL ) + 1 ); // GetTempPath will return the size for the null terminator.
  
  DWORD size = ::GetTempPath( (DWORD)resultPath.capacity(), &resultPath[0] );

  resultPath.resize( size );

  return resultPath;
}
