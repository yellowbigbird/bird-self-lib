namespace CUtils
{
	typedef std::wstring tstring;

	tstring ExtractFilePath(const tstring& path);
	tstring ExtractFileName(const tstring& path);
	tstring ExtractFileExt(const tstring& path);
	bool FileExists(const tstring& path);
};