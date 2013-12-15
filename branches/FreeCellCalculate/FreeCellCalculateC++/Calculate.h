#pragma once

#include "card.h"
using namespace Card;

class CCalculate
{
    typedef std::vector<int>    VecIdx;
public:
    CCalculate();

    void InitData();
    void InputData();
    void Run();

    bool CheckInputDataLegal() const;
    bool FWin() const;
    bool FSorted(const VecIdx& vecIdx) const;
    bool FCanMove() const;
    bool FCanMove(int colIdx) const;
    bool FCanMoveToSorted(const VecIdx& vecIdx) const;
    bool FCanPushToEnd(const VecIdx& vecIdx, int cardIdx) const;
    bool IsBenchHaveBlank() const;  
    int  GetCurMoveCardAmount() const;
    void GetLastSortedList(const VecIdx& vecIdx,  int moveMax, VecIdx& vecIdxSorted) const;

public:

    std::vector<VecIdx>         m_vecVecIdx;
    VecIdx                      m_vecIdxSorted;   //4 vec,save show card
    VecIdx                      m_vecBench;       //4 card  

    //VecCard             m_vecCardAll;   //4*13
};

