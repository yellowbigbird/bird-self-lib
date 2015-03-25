#include <string>
#include <set>
#include "selflib_dll.h"

namespace Utils
{
	typedef std::wstring tstring;

	//tstring ExtractFilePath(const tstring& path);
	
	//tstring ExtractFileName(const tstring& path);
	//std::string ExtractFileName(const std::string& path);

	//tstring ExtractFileExt(const tstring& path);
	//bool FileExists(const tstring& path);

	//bool DirectoryExists(const tstring& path);
	//bool CreateDirectory(const tstring& path);
	//bool DeleteDirectory(const TCHAR* charDir);
	//bool DeleteFile(const tstring& path);

	SELFLIB_DLL tstring ExpandPath(const tstring& path);

	//UINT GetFileSize(const tstring path);

	//bool HasWildcards(const tstring& path);

	

	//bool CompareFileTime(const tstring& file1, const tstring& file2, int& result);

	SELFLIB_DLL tstring GetTempPath( );
};