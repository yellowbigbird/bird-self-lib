#pragma once

#include "card.h"
using namespace Card;

class CState
{
public:
    CState();
    
    void InitData();
    void InputData();

    bool CheckInputDataLegal() const;
    bool FWin() const;
    bool FSorted(const VecCard& vecIdx) const;
    bool FCanMove() const;
    bool FCanMove(int colIdx) const;
    bool FCanMoveToSorted(const VecCard& vecIdx) const;
    bool FCanPushToEnd(const VecCard& vecIdx, CCard cardIdx) const;
    bool IsBenchHaveBlank() const;  
    int  GetCurMoveCardAmount() const;
    void GetLastSortedList(const VecCard& vecIdx,  int moveMax, VecCard& vecIdxSorted) const;

public:
    std::vector<VecCard>         m_vecVecIdx;
    VecCard                      m_vecIdxSorted;   //4 vec,save show card
    VecCard                     m_vecBench;       //4 card 
};