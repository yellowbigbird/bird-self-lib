// ThreadPool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "UtilThreadPool.h"
#include "UtilRunable.h"

#include <assert.h>
#include <map>
#include <list>

#ifdef _AFX
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#endif 

#if _MSC_VER > 1200
#pragma warning(disable:4996)
#else 
#pragma warning(disable:4786)
#endif // _MSC_VER > 1200

using std::map;
using std::list;

namespace UtilThreadPool
{

class CThreadPool_Private
{
private:
    friend class CThreadPool;
    typedef struct tagFunctionData
    {
        IRunable * runObject;
        LPTHREAD_START_ROUTINE pThreadProc;
        LPVOID pData;
    }FunctionData;

    typedef struct _tagThreadData {
        bool bFree;
        DWORD dwThreadId;
        HANDLE hThread;
        HANDLE hWaitHandle;
    }ThreadData;

    typedef std::map<DWORD, ThreadData, std::less<DWORD>, std::allocator<ThreadData> > ThreadMap;
    typedef std::list<FunctionData > FunctionList;

    void init();
    void create();
    void destory();
    void releaseMemory();
    HANDLE getWaitHandle(DWORD dwThreadId);

    bool busyThread(DWORD);
    void finishThread(DWORD);

    void start();

    bool getThreadFunctionProc(DWORD dwThreadId, FunctionData & functionData);

    static DWORD WINAPI ThreadProc(_In_  LPVOID lpParameter);

    int getWorkingThreadCount();

public:
    CThreadPool_Private(int poolSize, CThreadPool * d);
    ~CThreadPool_Private();
    bool start(LPTHREAD_START_ROUTINE funcProc, LPVOID pData);
    bool start(IRunable * runable);

private:
    int m_poolSize;
    CRITICAL_SECTION m_csFunctionList;
    CRITICAL_SECTION m_csThreadMap;
    ThreadMap m_threadMap;
    FunctionList m_funtionList;
    HANDLE m_hNotifyShutdown;
    CThreadPool *  threadPool_d;
};


CThreadPool_Private::CThreadPool_Private(int poolSize, CThreadPool * d) : 
    m_poolSize(poolSize),
    threadPool_d(d)
{
    init();
    create();
}

void CThreadPool_Private::init()
{
    ::InitializeCriticalSection(&m_csFunctionList);
    ::InitializeCriticalSection(&m_csThreadMap);
}

CThreadPool_Private::~CThreadPool_Private()
{
    destory();
    ::DeleteCriticalSection(&m_csFunctionList);
    ::DeleteCriticalSection(&m_csThreadMap);
}

void CThreadPool_Private::create()
{
    DWORD dwThreadId;
    HANDLE hThread;

    m_hNotifyShutdown = ::CreateEvent(NULL, TRUE, FALSE, NULL);
    assert(m_hNotifyShutdown != NULL);
    if(!m_hNotifyShutdown)
        return;

    for(int i = 0; i < m_poolSize; ++i) {
        hThread = ::CreateThread(0, 0, ThreadProc, (LPVOID)this, CREATE_SUSPENDED, &dwThreadId);
        assert(hThread != NULL);
        if (NULL == hThread)
            return;

        ThreadData threadData;
        threadData.bFree = true;
        threadData.dwThreadId = dwThreadId;
        threadData.hThread = hThread;
        threadData.hWaitHandle = ::CreateEvent(0, TRUE, FALSE, NULL);
        assert(NULL != threadData.hWaitHandle);
        if (NULL == threadData.hWaitHandle) {
            return;
        }

        m_threadMap.insert(ThreadMap::value_type(dwThreadId, threadData));
        
        ::ResumeThread(hThread);
    }
}

void CThreadPool_Private::releaseMemory()
{
    FunctionList::const_iterator cIter;

    for(cIter = m_funtionList.begin(); cIter != m_funtionList.end(); ++ cIter) {

        IRunable* runable = cIter->runObject;
        if (runable) {
            delete runable;
        }
    }

    m_funtionList.clear();
    m_threadMap.clear();
}

void CThreadPool_Private::destory()
{
    ::SetEvent(m_hNotifyShutdown);
    ::Sleep(1000);

    if(getWorkingThreadCount() > 0)
        ::Sleep(5000);

    ThreadMap::const_iterator cIter;

    for(cIter = m_threadMap.begin(); cIter != m_threadMap.end(); ++cIter) {
        ::CloseHandle(cIter->second.hThread);
        ::CloseHandle(cIter->second.hWaitHandle);
    }

    ::CloseHandle(m_hNotifyShutdown);

    releaseMemory();
}

HANDLE CThreadPool_Private::getWaitHandle(DWORD dwThreadId)
{
    HANDLE hWaitHanle = NULL;
    ThreadMap::iterator iter = m_threadMap.end();

    ::EnterCriticalSection(&m_csThreadMap);

    iter = m_threadMap.find(dwThreadId);
    if (iter != m_threadMap.end())
        hWaitHanle = m_threadMap[dwThreadId].hWaitHandle;

    ::LeaveCriticalSection(&m_csThreadMap);
    return hWaitHanle;
}

bool CThreadPool_Private::busyThread(DWORD dwThreadId)
{
    ThreadMap::iterator iter = m_threadMap.end();
    bool bRet = false;

    ::EnterCriticalSection(&m_csThreadMap);

    iter = m_threadMap.find(dwThreadId);

    if (iter != m_threadMap.end()) {
        m_threadMap[dwThreadId].bFree = false;
        bRet = true;
    }

    ::LeaveCriticalSection(&m_csThreadMap);
    return bRet;
}

void CThreadPool_Private::finishThread(DWORD dwThreadId)
{
    ThreadMap::iterator iter = m_threadMap.end();

    ::EnterCriticalSection(&m_csThreadMap);

    iter = m_threadMap.find(dwThreadId);
    if (iter != m_threadMap.end()) {
        if (!m_funtionList.empty()) { // task is not empty  this thread also busy 
            m_threadMap[dwThreadId].bFree = false;
            LeaveCriticalSection(&m_csThreadMap);
            return;
        } else {
            m_threadMap[dwThreadId].bFree = true;
        }
    }

    ::LeaveCriticalSection(&m_csThreadMap);
    ::ResetEvent(m_threadMap[dwThreadId].hWaitHandle);
}

bool CThreadPool_Private::getThreadFunctionProc(DWORD dwThreadId, FunctionData & functionData)
{
    bool bRet = false;

    ::EnterCriticalSection(&m_csFunctionList);

    if (!m_funtionList.empty()) {
        functionData = m_funtionList.front();
        m_funtionList.pop_front();
        bRet = true;
    }
    ::LeaveCriticalSection(&m_csFunctionList);
    return true;
}

DWORD WINAPI CThreadPool_Private::ThreadProc(LPVOID lpParameter)
{
    DWORD dwThreadId = ::GetCurrentThreadId();
    CThreadPool_Private * pool = (CThreadPool_Private *)lpParameter;

    assert(pool != NULL);
    if (NULL == pool) 
        return 1;

    DWORD dwWait = 0;
    HANDLE handles[2] = {pool->getWaitHandle(dwThreadId), pool->m_hNotifyShutdown};

    while(1) {
        dwWait = ::WaitForMultipleObjects(2, handles, FALSE, INFINITE);

        if (0 == (dwWait - WAIT_OBJECT_0)) { // thread event

            if (pool->busyThread(dwThreadId)) {
                FunctionData functionData = {0};

                if (pool->getThreadFunctionProc(dwThreadId, functionData)) {
                    if (functionData.runObject) {
                        bool bAutoDelete = functionData.runObject->autoDelete();

                        functionData.runObject->run();
                        if (bAutoDelete)
                            delete functionData.runObject;
                    } else if (functionData.pThreadProc) {
                        functionData.pThreadProc(functionData.pData);
                    } else {
                        // TODO: run empty  
                        // bug run this code 
#if _AFX
                        TRACE1("1 thread count: %d\n", pool->getWorkingThreadCount());
#else 
#if _DEBUG
                        printf("1 thread count: %d\n", pool->getWorkingThreadCount());
#endif
#endif
                    }
                } else {
#if _AFX
                        TRACE1("2 thread count: %d\n", pool->getWorkingThreadCount());
#else 
#if _DEBUG
                        printf("2 thread count: %d\n", pool->getWorkingThreadCount());
#endif
#endif
                }
                pool->finishThread(dwThreadId);
            }
        } else { // shutdown event
            break;
        }
    }

    return 0;
}

void CThreadPool_Private::start()
{
    // See if any threads are free
    CThreadPool_Private::ThreadMap::iterator iter;
    CThreadPool_Private::ThreadData ThreadData;

    ::EnterCriticalSection(&m_csThreadMap);
    for(iter = m_threadMap.begin(); iter != m_threadMap.end(); iter++) {
        ThreadData = (*iter).second;
        
        if(ThreadData.bFree) {
            m_threadMap[ThreadData.dwThreadId].bFree = false;
            ::SetEvent(ThreadData.hWaitHandle); 
            break;
        }
    }

    ::LeaveCriticalSection(&m_csThreadMap);
}

int CThreadPool_Private::getWorkingThreadCount()
{
    ThreadMap::iterator iter;
    ThreadData ThreadData;
    int nCount = 0;
    ::EnterCriticalSection(&m_csThreadMap);
    for(iter = m_threadMap.begin(); iter != m_threadMap.end(); iter++) 
    {
        ThreadData = (*iter).second;

        if(!ThreadData.bFree) 
            nCount++;
    }
    ::LeaveCriticalSection(&m_csThreadMap);
    return nCount;
}

bool CThreadPool_Private::start(LPTHREAD_START_ROUTINE funcProc, LPVOID pData)
{
    CThreadPool_Private::FunctionData funcdata = {0};

    funcdata.pThreadProc = funcProc; // the function object
    funcdata.pData = pData;

    // add it to the list
    ::EnterCriticalSection(&m_csFunctionList);
    m_funtionList.push_back(funcdata);
    ::LeaveCriticalSection(&m_csFunctionList);

    start();
    return true;
}

bool CThreadPool_Private::start(IRunable * runable)
{
    FunctionData funcdata = {0};

    funcdata.runObject = runable; // the function object

    // add it to the list
    ::EnterCriticalSection(&m_csFunctionList);
     m_funtionList.push_back(funcdata);
    ::LeaveCriticalSection(&m_csFunctionList);

    start();
    return true;
}

//-------------------------------------------------end CThreadPool_Private

CThreadPool* CThreadPool::g_pThis = NULL;

CThreadPool& CThreadPool::GetInstance()
{
	if(!g_pThis)
	{
		g_pThis = new CThreadPool();
        atexit(DestroyInstance);        
	}
	return *g_pThis;
}

void CThreadPool::DestroyInstance()
{
	SAFE_DELETE(g_pThis);
}

CThreadPool::CThreadPool(int poolSize)
{
    threadPool_p = new CThreadPool_Private(poolSize, this);
}

CThreadPool::~CThreadPool()
{
    if (threadPool_p) {
        delete threadPool_p;
    }
}

int CThreadPool::getPoolSize()
{
    return threadPool_p->m_poolSize;
}

void CThreadPool::setPoolSize(int size)
{
    if (size == threadPool_p->m_poolSize || size <= 0) 
        return;

    threadPool_p->destory();

    threadPool_p->m_poolSize = size;

    threadPool_p->create();
}

int CThreadPool::getWorkingThreadCount()
{
    return threadPool_p->getWorkingThreadCount();
}

bool CThreadPool::start(IRunable * runable)
{
    assert(NULL != runable);
    if (NULL == runable)
        return false;
    runable->threadPool = this;

    threadPool_p->start(runable);
    return true;
}


bool CThreadPool::start(LPTHREAD_START_ROUTINE funcProc, LPVOID pData)
{
    assert(NULL != funcProc);
    if (NULL == funcProc)
        return false;

    threadPool_p->start(funcProc, pData);
    return true;
}

} // end namespace UtilThreadPool

