#pragma once

#ifdef OSSWRAPPER_EXPORTS
#define OSS_DLL     __declspec(dllexport)
#else
#define OSS_DLL     __declspec(dllimport)
#endif

