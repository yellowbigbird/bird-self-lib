#pragma once

#include <string>

//#include <WTypes.h>

namespace UtilString{
  std::wstring ConvertMultiByteToWideChar(const std::string& strSrc);
  std::wstring ConvertMultiByteToWideCharUTF8(const std::string& strSrc);

  std::string ConvertWideCharToMultiByte(const std::wstring& wstrSrc);
  std::string ConvertWideCharToMultiByteUTF8(const std::wstring& wstrSrc);
}
