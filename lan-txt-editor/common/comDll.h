#pragma once

#ifdef  _COMMON_DLL_
#define COM_DLL     __declspec(dllexport)
#else
#define COM_DLL     __declspec(dllimport)
#endif
