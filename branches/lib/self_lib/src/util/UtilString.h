#pragma once

#include <string>
#include "selflib_dll.h"
//#include <WTypes.h>

namespace UtilString{
  SELFLIB_DLL std::wstring ConvertMultiByteToWideChar(const std::string& strSrc);
  SELFLIB_DLL std::wstring ConvertMultiByteToWideCharUTF8(const std::string& strSrc);

  SELFLIB_DLL std::string ConvertWideCharToMultiByte(const std::wstring& wstrSrc);
  SELFLIB_DLL std::string ConvertWideCharToMultiByteUTF8(const std::wstring& wstrSrc);

  SELFLIB_DLL bool  ConvertHexStringToValue(const std::string& str, int& value) ;
}
