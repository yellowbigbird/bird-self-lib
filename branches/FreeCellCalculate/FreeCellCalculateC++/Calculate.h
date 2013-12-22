#pragma once

#include "card.h"
using namespace Card;

#include "State.h"



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
    void SortInser(VecInt& vecToInsert, int stateIdx) const;

    bool AddToAll(const CState& st);
    bool FindStInAll(const CState& st, UINT& id) const;

    double      m_valueOpen;    //save lowest value of open
    CState      m_stateStart;
    VecState    m_vecStateAll;
    VecInt      m_vecIdxOpen;
    VecInt      m_vecIdxClose;
};

