#pragma once

#include "card.h"
using namespace Card;

#include "State.h"
#include <map>
#include <hash_map>


class CCalculate
{
    //typedef std::vector<int>    VecIdx;

public:
    CCalculate();
    
    void Run();

    bool FindLowestValueState(int& stateIdx, UINT& idxInOpen) const;
 
    void GenerateSonState(CState& st);

    void EraseStateFromOpen(UINT idxInOpen);

    //void SortOpen();
    bool SortInsert(VecInt& vecToInsert, int stateIdx) const;

    bool AddToAll(CState& st);
    bool FindStInAll(const CState& st, UINT& id) const;

    double      m_valueOpen;    //save lowest value of open
    CState      m_stateStart;
    
    VecInt      m_vecIdxOpen;
    VecInt      m_vecIdxClose;

    //typedef stdext::hash_map<CState, UINT>  MapStateId;
    typedef std::map<std::string, UINT>  MapStateId;
    typedef std::map<UINT, CState>  MapIdState;
    
    //VecState    m_vecStateAll;
    MapStateId  m_mapStateId;
    MapIdState  m_vecStateAll;
};

