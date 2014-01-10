#pragma once

#include "State.h"
//#include "Calculate.h"

class CCalculate;
typedef std::list<CCalculate*>   ListCalc;

class CThreadPool
{
public:
    CThreadPool();
    virtual ~CThreadPool();

    void StartCalc(UINT gameNum);
   
    void Stop();

protected:
    bool StartThread();

    void Run();

    void Init();
    void Clear();
    void AddThreads();
    void StartAllThreads();

    void OutputResult();

    static DWORD  WINAPI ThreadFunc(void* pServer);

public:
    bool        m_fThreadRunning;
    bool        m_fEnd;
    CState      m_stateStart;

    UINT        m_gameNum;
    ListCalc    m_threadArray;
    VecStr      m_vecStrStep;
};