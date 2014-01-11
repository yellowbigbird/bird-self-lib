#include "stdafx.h"
#include "ThreadPool.h"

#include "Calculate.h"
#include "util/DebugFile.h"

using namespace std;

const int c_max_thread = 3; //max to cpu core number

CThreadPool::CThreadPool()
    :m_gameNum(0)
    ,m_fThreadRunning(false)
    ,m_fEnd(false)
{
}

CThreadPool::~CThreadPool()
{
   Clear();
}

void CThreadPool::StartCalc(UINT gameNum)
{
    m_gameNum = gameNum;

    //start thread
    StartThread();
   
}

void CThreadPool::Stop()
{
    CCalculate* pCal = NULL;
    for(ListCalc::iterator it = m_threadArray.begin();
        it != m_threadArray.end();
        it++)
    {
        pCal = *it;
        if(!pCal){
            ASSERT(false);
            continue;
        }
        pCal->Stop();
    }

    m_fThreadRunning = false;
}

bool CThreadPool::StartThread()
{
    DWORD dwThreadId = 0;
    HANDLE hThread = ::CreateThread(NULL, 0, CThreadPool::ThreadFunc, (LPVOID*)this, 0, &dwThreadId);
    if (!hThread)
    {
        m_fThreadRunning = false;
        return false;
    }    
    m_fThreadRunning = true;
    CloseHandle(hThread);	
    return true;
}

void CThreadPool::Init()
{
    Clear();

    m_stateStart.InitData();
    m_stateStart.GenerateCards(m_gameNum);

    m_fEnd = false;
}

void CThreadPool::Clear()
{
    CCalculate* pcal = NULL;
    try
    {
        for(ListCalc::iterator it = m_threadArray.begin();
            it != m_threadArray.end();
            it++)
        {
            pcal = *it;
            SAFE_DELETE(pcal);
        }
    }
    catch (CException* e)
    {
        ASSERT(false);
        //e.GetErrorMessage();
        AddDebug("SAFE_DELETE CException.");
    }
    
    m_threadArray.clear();

    m_vecStrStep.clear();
}

void CThreadPool::AddThreads()
{
    CCalculate tempCalc;
    tempCalc.m_stateStart = m_stateStart;
    tempCalc.Init();
    m_stateStart.GenerateSonState(&tempCalc);

    CCalculate* pCal= NULL;

    for(MapIdState::iterator it = tempCalc.m_vecStateAll.begin();
        it != tempCalc.m_vecStateAll.end() && m_threadArray.size() < c_max_thread ;
        it++
        ){   
            if(0 == it->first)
                continue;
            CState& st = it->second;
            //st.m_id = 0;

            pCal = new CCalculate();
            if(!pCal){
                ASSERT(false);
                break;
            }

            pCal->m_stateStart = st;
            pCal->Init();
            //pCal->m_stateStart.m_id = 0;
            pCal->m_stateFather.push_back(m_stateStart);
            pCal->StartCalc();

            m_threadArray.push_back(pCal);
    }//for
}

DWORD  CThreadPool::ThreadFunc(void* pServer)
{
    CThreadPool* pthis = (CThreadPool*)(pServer);
    if(!pthis)
        return 0;
    pthis->Run();
    return 0;
}

void CThreadPool::Run()
{
    Init(); 

    CCalculate* pCal = NULL;
    AddThreads();

    while(m_fThreadRunning
        && !m_fEnd
        ){
        ::Sleep(500);

        //check threads
        for(ListCalc::iterator it = m_threadArray.begin();
            it != m_threadArray.end();
            it++)
        {
            pCal = *it;
            if(!pCal){
                ASSERT(false);
                m_threadArray.erase(it);
                break;
            }
            if(pCal->IsWin() ){
                //win

                m_fThreadRunning = false; //must stop while 1st
                ::Sleep(1000);
                break;
            }
        }
    }// while

    //stop all
    m_vecStrStep.clear();
    for(ListCalc::iterator it = m_threadArray.begin();
        it != m_threadArray.end();
        it++)
    {
        pCal = *it;
        if(!pCal)
            continue;
        pCal->Stop();
        if(pCal->IsWin() ){
            if(m_vecStrStep.size() < 1
                || pCal->m_vecStrStep.size() <  m_vecStrStep.size()){
                m_vecStrStep = pCal->m_vecStrStep;
            }
        }
    }

    //clear
    for(ListCalc::iterator it = m_threadArray.begin();
        it != m_threadArray.end();
        it++)
    {
        pCal = *it;
        pCal->Clear();
    }

    OutputResult();

    m_fThreadRunning = false;
    m_fEnd = true;

}

void  CThreadPool::OutputResult()
{
    for(UINT idx=0; idx< m_vecStrStep.size(); idx++)
    {
        AddDebug(m_vecStrStep[idx]);
    }
}


