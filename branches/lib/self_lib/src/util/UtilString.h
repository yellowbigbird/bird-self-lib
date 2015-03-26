#pragma once

#include <string>
#include <vector>
#include "selflib_dll.h"
//#include <WTypes.h>


typedef std::vector<std::string> VecStr;

namespace UtilString{
  SELFLIB_DLL std::wstring ConvertMultiByteToWideChar(const std::string& strSrc);
  SELFLIB_DLL std::wstring ConvertMultiByteToWideCharUTF8(const std::string& strSrc);

  SELFLIB_DLL std::string ConvertWideCharToMultiByte(const std::wstring& wstrSrc);
  SELFLIB_DLL std::string ConvertWideCharToMultiByteUTF8(const std::wstring& wstrSrc);

  SELFLIB_DLL bool  ConvertHexStringToValue(const std::string& str, int& value) ;


  SELFLIB_DLL void ChangeStringLetter(std::string& str, bool fToUp);
  SELFLIB_DLL void ChangeStringLetter(std::wstring& wstr, bool fToUp);

  SELFLIB_DLL bool SplitString(const std::string& strSrc, VecStr& vecStrDest, const std::string& strMark=",");
}
