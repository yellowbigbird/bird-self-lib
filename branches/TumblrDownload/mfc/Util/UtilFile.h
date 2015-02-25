
#pragma once

#ifndef tstring
#include <string>
#if defined(_UNICODE) || defined(UNICODE)
#define tstring	std::wstring
#else
#define tstring	std::string
#endif
#endif

#include "wtypes.h"
#include <set>

//#include "selflib_dll.h"
//#define SELFLIB_DLL

namespace  UtilFile{

#define ASIMOVLIB_API 
    
    ASIMOVLIB_API std::wstring    GetFilePath() ;

	ASIMOVLIB_API bool        HasWildcards(const tstring& path);

    ASIMOVLIB_API bool        FileExists(const tstring& path);
    ASIMOVLIB_API bool        DirectoryExists(const tstring& path);

    ASIMOVLIB_API UINT64      GetFileSize(const tstring& path);

    ASIMOVLIB_API UINT64      GetFileTimeGmt(const tstring& path);
    ASIMOVLIB_API UINT64      GetFileTimeGmtByMfc(const tstring& path);

    ASIMOVLIB_API bool        SetFileTimeGmt(const tstring& path, UINT64 gmtTime);

    ASIMOVLIB_API UINT64      ConvertFileTimeToUint64(const FILETIME& fileTime);
    
    ASIMOVLIB_API bool        CreateDirectory(const tstring& path);
	ASIMOVLIB_API bool        ClearDirectory(const tstring& path);

    ASIMOVLIB_API bool        CopyFile(const tstring& pathOld, const tstring& pathDest);
    ASIMOVLIB_API bool        MoveFile(const tstring& pathOld, const tstring& pathDest);
	ASIMOVLIB_API bool        DeleteFile(const tstring& path);
	ASIMOVLIB_API bool        DeleteFileThroughPostfix(const tstring& path, const tstring& postfixName);

    ASIMOVLIB_API tstring	    ExtractFilePath(const tstring& path);

    ASIMOVLIB_API tstring		ExtractFileName(const tstring& path);
	ASIMOVLIB_API std::string   ExtractFileName(const std::string& path);

    ASIMOVLIB_API tstring     ExtractFileExt(const tstring& path);

    ASIMOVLIB_API bool        ReadFileAsString(const std::wstring& fileName, std::string& strData);
    ASIMOVLIB_API bool        WriteFileAsString(const std::wstring& fileName, const std::string& strData);

	ASIMOVLIB_API bool        GetFileList(const tstring& mask, std::set<tstring>& files);
	ASIMOVLIB_API bool        GetFileList(const tstring& mask, std::set<std::string>& files);

	ASIMOVLIB_API bool        CompareFileTime(const tstring& file1, const tstring& file2, int& result);
}