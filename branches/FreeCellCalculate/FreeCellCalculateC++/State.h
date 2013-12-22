#pragma once

#include "card.h"
using namespace Card;

typedef std::vector<int>       VecInt;
typedef VecInt::iterator       VecIntIt;

class CState
{
public:
    CState();
    CState(const CState& other);
    
    void InitData();
    void InputData();

    double GetValue() const;
    double CalculateValue();
    bool operator == (const CState& other) const;

    void GenerateSonState(std::vector<CState>& vecState);

    bool MoveColToBench     (std::vector<CState>& vecState, UINT colIdx);
    bool MoveColToSorted    (std::vector<CState>& vecState, UINT colIdx);
    bool MoveColToCol       (std::vector<CState>& vecState, UINT colIdxSrc, UINT colIdxDest);
    bool MoveBenchToSorted  (std::vector<CState>& vecState, UINT benchIdx);
    bool MoveBenchToCol     (std::vector<CState>& vecState, UINT benchIdx, UINT colIdx);

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
    UINT                    m_id;
    int                     m_idxFather;
    VecInt                  m_idxSon;
    double                  m_value;

    VecCard                 m_vecIdxSorted;   //4 vec, save show card
    VecCard                 m_vecBench;       //4 card 
    std::vector<VecCard>    m_vecVecIdx;
};

typedef std::vector<CState>     VecState;

