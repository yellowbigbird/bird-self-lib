#pragma once

#ifdef LZMALIB_EXPORTS
#define LZMA_DLL    __declspec(dllexport)
#else
#define LZMA_DLL    __declspec(dllimport)
#endif 