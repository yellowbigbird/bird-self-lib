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

    UINT16 GetValue() const;
    
    void Update();

    //CState& operater= (const CState& othe );
    bool operator == (const CState& other) const;
    bool operator < (const CState& other) const;

    void GenerateSonState(std::vector<CState>& vecState);
    
    bool CheckInputDataLegal() const;
    bool FWin() const;
    bool FSorted(const VecCard& vecIdx) const;
    bool FCanMove() const;
    
    bool IsBenchHaveBlank() const;  
    int  GetCurMoveCardAmount() const;
    void GetLastSortedList(const VecCard& vecCard,  int moveMax, VecCard& vecIdxSorted) const;

protected:
    double UpdateValue();
    void UpdateString();
    
    bool MoveColToBench     (std::vector<CState>& vecState, UINT colIdx);
    bool MoveColToSorted    (std::vector<CState>& vecState, UINT colIdx);
    bool MoveColToCol       (std::vector<CState>& vecState, UINT colIdxSrc, UINT colIdxDest, const VecCard& vecLastSorted);
    bool MoveBenchToSorted  (std::vector<CState>& vecState, UINT benchIdx);
    bool MoveBenchToCol     (std::vector<CState>& vecState, UINT benchIdx, UINT colIdx);

    void SetIdxFather(CState& stSon);

    bool FCanMove(int colIdx) const;
    bool FCanMoveToSorted(const VecCard& vecIdx) const;
    bool FCanPushToEnd(const VecCard& vecIdx, CCard cardIdx) const;

public:    
    UINT                    m_id;
    UINT                    m_idxFather;
    VecInt                  m_idxSon;
    UINT16                  m_value;
    UINT8                   m_step;
    std::string             m_str;

    VecCard                 m_vecIdxSorted;   //4 vec, save show card
    VecCard                 m_vecBench;       //4 card 
    std::vector<VecCard>    m_vecVecIdx;
};

typedef std::vector<CState>     VecState;

