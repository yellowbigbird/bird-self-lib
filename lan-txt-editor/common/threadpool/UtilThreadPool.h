
#ifndef _THREAD_POOL_
#define _THREAD_POOL_

#if defined(_MSC_VER)  && ((_MSC_VER) >= 1020)
#pragma once
#endif  // defined(_MSC_VER)  && ((_MSC_VER) >= 1020)

#include "..\comDll.h"
#include <Windows.h>

#undef POOL_SIZE
#define POOL_SIZE 10

namespace UtilThreadPool
{
class IRunable;
class CThreadPool_Private;
class COM_DLL CThreadPool 
{
public:
	static CThreadPool& GetInstance();
	static void DestroyInstance();

    CThreadPool(int poolSize = POOL_SIZE);
    ~CThreadPool();
    int getPoolSize();
    void setPoolSize(int);
    bool start(IRunable * runable);
    bool start(LPTHREAD_START_ROUTINE funcProc, LPVOID pData);
    int getWorkingThreadCount();

private:
	static CThreadPool* g_pThis;
    CThreadPool_Private * threadPool_p;
};

} // end namespace UtilThreadPool


#endif // end _THREAD_POOL_