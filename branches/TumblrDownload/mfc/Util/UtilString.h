#pragma once

#include <string>
//#include "selflib_dll.h"
//#define  SELFLIB_DLL
//#include <WTypes.h>

#define ASIMOVLIB_API  

typedef std::vector<std::string> VecStr;

namespace UtilString{
  ASIMOVLIB_API std::wstring ConvertMultiByteToWideChar(const std::string& strSrc);
  ASIMOVLIB_API std::wstring ConvertMultiByteToWideCharUTF8(const std::string& strSrc);

  ASIMOVLIB_API std::string ConvertWideCharToMultiByte(const std::wstring& wstrSrc);
  ASIMOVLIB_API std::string ConvertWideCharToMultiByteUTF8(const std::wstring& wstrSrc);

  ASIMOVLIB_API bool  ConvertHexStringToValue(const std::string& str, int& value) ;


  ASIMOVLIB_API void ChangeStringLetter(std::string& str, bool fToUp);

  ASIMOVLIB_API bool SplitString(const std::string& strSrc, VecStr& vecStrDest, const std::string& strMark=",");
}
