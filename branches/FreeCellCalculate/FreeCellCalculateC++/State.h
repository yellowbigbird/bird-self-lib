#pragma once

#include "card.h"
using namespace Card;

#include <list>

typedef std::vector<byte>       VecByte;
typedef std::vector<int>       VecInt;
typedef std::list<int>         ListInt;
typedef VecInt::iterator       VecIntIt;

class CCalculate;
class CStep;

class CState
{
public:
    CState();
    
    void InitData();
    //void InputData();

    UINT16 GetValue() const;
    
    void Update();
    void CopyFromFather(const CState& other);

    //CState& operater= (const CState& othe );
    bool operator == (const CState& other) const;
    bool operator < (const CState& other) const;

    UINT GenerateSonState(CCalculate* ) ; //std::list<CState>& vecState
    bool GenerateCards(UINT boardNum); 
    bool GenerateStep(CStep* step, std::string& str, const CState& stNext) const;
    
    bool CheckDataLegal() const;

    bool FWin() const;
    bool FSorted(const ListCard& vecIdx) const;
    bool FCanMove() const;
    	
    void SetIdxFather(CState& stSon);

    //bool IsBenchHaveBlank() const;  
    int  GetCurMoveCardAmount(int colDestIdx = c_colSize) const;
    void GetLastSortedList(const ListCard& vecCard,  int moveMax, ListCard& vecIdxSorted) const;


protected:
    double UpdateValue();
    std::string UpdateString() const;
    void UpdateVectorInt(VecByte& vecData) const;
    void UpdateHash();
    
    bool MoveColToBench     (CCalculate*, UINT colIdx) ;
    bool MoveColToCol       (CCalculate*, UINT colIdxSrc, UINT colIdxDest, const ListCard& vecLastSorted) ;    
    bool MoveBenchToCol     (CCalculate*, UINT benchIdx, UINT colIdx) ;

    bool MoveBenchToSorted  (CCalculate*, UINT benchIdx) ;
    bool MoveColToSorted    (CCalculate*, UINT colIdx) ;

    bool MoveColToSorted    (UINT colIdx);
    bool MoveBenchToSorted  (UINT benchIdx);

    bool UpdateCardToSorted();


    bool FCanMove           (UINT colIdx) const;
    bool FCanMoveToSorted   (const ListCard& vecIdx) const;
    bool FCanMoveToBench    (UINT& benchIdx) const ;
    bool FCanPushToEnd      (const ListCard& vecCards, const CCard& cardIdx) const;

    UINT GetBenchEmptyCount () const;

public:    
    UINT                    m_id;
    UINT                    m_idxFather;
    bool                    m_hasGenSon;
    //bool                    m_dead;
    ListInt                 m_idxSon;
    UINT16                  m_value;
    UINT8                   m_step;
    UINT                    m_hash;
    //std::string             m_str;

    VecCard                 m_vecIdxSorted;   //4 vec, save show card
    VecCard                 m_vecBench;       //4 card 
	typedef std::vector<ListCard>  VecListCard;
    VecListCard				m_vecVecIdx;
};

typedef std::vector<CState>     VecState;
typedef std::list<CState>     ListState;
typedef std::map<UINT, UINT>  MapHashId;
typedef std::map<UINT, CState>  MapIdState;
