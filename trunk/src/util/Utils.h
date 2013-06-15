#include <string>
#include <set>

namespace CUtils
{
	typedef std::wstring tstring;

	tstring ExtractFilePath(const tstring& path);
	
	tstring ExtractFileName(const tstring& path);
	std::string ExtractFileName(const std::string& path);

	tstring ExtractFileExt(const tstring& path);
	bool FileExists(const tstring& path);

	bool DirectoryExists(const tstring& path);
	bool CreateDirectory(const tstring& path);
	bool DeleteDirectory(const TCHAR* charDir);
	bool DeleteFile(const tstring& path);

	tstring ExpandPath(const tstring& path);

	UINT GetFileSize(const tstring path);

	bool HasWildcards(const tstring& path);

	bool GetFileList(const tstring& mask, std::set<tstring>& files);

	bool CompareFileTime(const tstring& file1, const tstring& file2, int& result);

	tstring GetTempPath( );
};