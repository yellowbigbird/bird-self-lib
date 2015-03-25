#pragma once

#include "card.h"
using namespace Card;

#include "State.h"
#include <map>
#include <hash_map>
#include <set>


typedef std::set<UINT>  SetId;

class CCalculate
{
    //typedef std::vector<int>    VecIdx;

public:
    CCalculate();
    virtual ~CCalculate();

    void Clear();
    
    void StartCalc();
    void Run();
    void Stop();
    void Init();

    bool IsStop() const;
    bool IsWin() const;

    bool SolutionAstar();
    bool SolutionDeep1st();

	void OutputResult();

    bool FindLowestValueState(int& stateIdx, UINT& idxInOpen) const;
 
    //UINT GenerateSonState(CState& st);
    bool CheckAndInsertState(CState& stSon,CState& stFather);

    void EraseStateFromOpen(UINT id);

    //void SortOpen();
    bool SortInsert(ListInt& vecToInsert, int stateIdx) const;

    bool AddToAll(CState& st);
    bool FindStInAll(const CState& st, UINT& id) const;
    bool FindStInDead(UINT id) const;

protected:
    static DWORD  WINAPI ThreadFunc(void* pServer);

public:

    CState      m_stateStart;
    ListState   m_stateFather;
    
    ListInt     m_vecIdxOpen;
    SetId       m_vecCloseHash; //save hash
    SetId       m_mapDeadId; //save hash
        
    //UINT        m_nextGenId;	
    UINT		m_curStateIdx ;    
        
    //VecState    m_vecStateAll;
    //MapHashId   m_mapStateId;
    MapIdState  m_vecStateAll;      //map<hash, state>
    VecStr      m_vecStrStep;

protected:
    bool        m_fThreadRunning;
    bool        m_fWin;
};



