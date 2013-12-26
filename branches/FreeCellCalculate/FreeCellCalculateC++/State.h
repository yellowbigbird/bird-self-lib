#pragma once

#include "card.h"
using namespace Card;

typedef std::vector<int>       VecInt;
typedef VecInt::iterator       VecIntIt;

class CState
{
public:
    CState();
    //CState(const CState& other);
    
    void InitData();
    void InputData();

    UINT16 GetValue() const;
    
    void Update();

    //CState& operater= (const CState& othe );
    bool operator == (const CState& other) const;
    bool operator < (const CState& other) const;

    void GenerateSonState(std::list<CState>& vecState) ;
    bool GenerateCards(UINT boardNum); 
    
    //bool CheckInputDataLegal() const;
    bool FWin() const;
    bool FSorted(const ListCard& vecIdx) const;
    bool FCanMove() const;
    	
    void SetIdxFather(CState& stSon);

    //bool IsBenchHaveBlank() const;  
    int  GetCurMoveCardAmount() const;
    void GetLastSortedList(const ListCard& vecCard,  int moveMax, ListCard& vecIdxSorted) const;

protected:
    double UpdateValue();
    void UpdateString();
    
    bool MoveColToBench     (std::list<CState>& vecState, UINT colIdx) ;
    //bool MoveColToSorted    (std::list<CState>& vecState, UINT colIdx) ;
    bool MoveColToCol       (std::list<CState>& vecState, UINT colIdxSrc, UINT colIdxDest, const ListCard& vecLastSorted) ;
    //bool MoveBenchToSorted  (std::list<CState>& vecState, UINT benchIdx) ;
    bool MoveBenchToCol     (std::list<CState>& vecState, UINT benchIdx, UINT colIdx) ;

    bool MoveColToSorted    (UINT colIdx);
    bool MoveBenchToSorted  (UINT benchIdx);

    bool UpdateCardToSorted();


    bool FCanMove           (int colIdx) const;
    bool FCanMoveToSorted   (const ListCard& vecIdx) const;
    bool FCanMoveToBench    (UINT& benchIdx) const ;
    bool FCanPushToEnd      (const ListCard& vecCards, const CCard& cardIdx) const;

    UINT GetBenchEmptyCount () const;

public:    
    UINT                    m_id;
    UINT                    m_idxFather;
    VecInt                  m_idxSon;
    UINT16                  m_value;
    UINT8                   m_step;
    std::string             m_str;

    VecCard                 m_vecIdxSorted;   //4 vec, save show card
    VecCard                 m_vecBench;       //4 card 
    std::vector<ListCard>   m_vecVecIdx;
};

//typedef std::vector<CState>     VecState;
typedef std::list<CState>     ListState;
