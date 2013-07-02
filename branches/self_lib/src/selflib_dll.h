#pragma once

#ifdef  _SELF_LIB_DLL_
#define SELFLIB_DLL     __declspec(dllexport)
#else
#define SELFLIB_DLL     __declspec(dllimport)
#endif
