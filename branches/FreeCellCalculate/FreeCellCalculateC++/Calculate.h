#pragma once

#include "card.h"
using namespace Card;

#include "State.h"
#include <map>
#include <hash_map>
#include <set>




class CCalculate
{
    //typedef std::vector<int>    VecIdx;

public:
    CCalculate();
    
    void StartCalc(UINT gameNum);
    void Run();
    void Stop();
    void Init();

    bool SolutionAstar();
    bool SolutionDeep1st();

	void OutputResult();

    bool FindLowestValueState(int& stateIdx, UINT& idxInOpen) const;
 
    //UINT GenerateSonState(CState& st);
    bool CheckAndInsertState(CState& stSon,CState& stFather);

    void EraseStateFromOpen(UINT idxInOpen);

    //void SortOpen();
    bool SortInsert(ListInt& vecToInsert, int stateIdx) const;

    bool AddToAll(CState& st);
    bool FindStInAll(const CState& st, UINT& id) const;
    bool FindStInDead(UINT id) const;
    
    static DWORD  WINAPI ThreadFunc(void* pServer);

public:

    CState      m_stateStart;
    
    ListInt      m_vecIdxOpen;
    ListInt      m_vecIdxClose;

    //typedef stdext::hash_map<CState, UINT>  MapStateId;
    typedef std::set<UINT>  SetId;
    UINT        m_nextGenId;
	
    UINT		m_curStateIdx ;
    UINT        m_gameNum;
        
    //VecState    m_vecStateAll;
    MapHashId    m_mapStateId;
    SetId       m_mapDeadId;
    MapIdState  m_vecStateAll;
    VecStr      m_vecStrStep;
    bool        m_fThreadRunning;
    bool        m_fEnd;
};

