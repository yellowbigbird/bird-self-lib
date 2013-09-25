
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
#include "selflib_dll.h"

namespace  UtilFile{

    
    SELFLIB_DLL std::wstring    GetFilePath() ;

	SELFLIB_DLL bool        HasWildcards(const tstring& path);

    SELFLIB_DLL bool        FileExists(const tstring& path);

    SELFLIB_DLL UINT64      GetFileSize(const tstring& path);
    SELFLIB_DLL UINT64      GetFileTime(const tstring& path);

    SELFLIB_DLL bool        DirectoryExists(const tstring& path);

    SELFLIB_DLL bool        CreateDirectory(const tstring& path);

	SELFLIB_DLL bool        DeleteFile(const tstring& path);

    SELFLIB_DLL tstring		ExtractFilePath(const tstring& path);

    SELFLIB_DLL tstring		ExtractFileName(const tstring& path);
	SELFLIB_DLL std::string ExtractFileName(const std::string& path);

    SELFLIB_DLL tstring     ExtractFileExt(const tstring& path);

    SELFLIB_DLL bool        ReadFileAsString(const std::wstring& fileName, std::string& strData);
    SELFLIB_DLL bool        WriteFileAsString(const std::wstring& fileName, const std::string& strData);

	SELFLIB_DLL bool        GetFileList(const tstring& mask, std::set<tstring>& files);

	SELFLIB_DLL bool        CompareFileTime(const tstring& file1, const tstring& file2, int& result);
}