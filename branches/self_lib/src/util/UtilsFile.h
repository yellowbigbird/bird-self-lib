
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

namespace UtilFile{

	bool HasWildcards(const tstring& path);

    bool FileExists(const tstring& path);
    UINT GetFileSize(const tstring path);
    bool DirectoryExists(const tstring& path);

	bool DeleteFile(const tstring& path);

    tstring		ExtractFilePath(const tstring& path);

    tstring		ExtractFileName(const tstring& path);
	std::string ExtractFileName(const std::string& path);

    tstring ExtractFileExt(const tstring& path);

    bool GetFile(TCHAR* fileName, std::string& strData);

	bool GetFileList(const tstring& mask, std::set<tstring>& files);

	bool CompareFileTime(const tstring& file1, const tstring& file2, int& result);
}