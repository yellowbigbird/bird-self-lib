#include "stdafx.h"
#include "State.h"
#include <assert.h>

using namespace std;

const UINT c_id = 0xffffffff;
const double c_valueInit = 100.0;

//////////////////////////////////////////////////////////////////////////
CState::CState()
    :m_id(0xffffffff)
    ,m_value(c_valueInit)
{
    InitData();
}

#define COPY_MEMBER(var)    var(other.var)
CState::CState(const CState& other)
    :COPY_MEMBER(m_id)
    ,COPY_MEMBER(m_idxFather)
    ,COPY_MEMBER(m_idxSon)
    ,COPY_MEMBER(m_value)
    ,COPY_MEMBER(m_vecIdxSorted)
    ,COPY_MEMBER(m_vecBench)
    ,COPY_MEMBER(m_vecVecIdx)
{

}

bool CState::operator == (const CState& other) const
{
    const bool fEqual =
        //m_id == other.m_id
        m_vecVecIdx == other.m_vecVecIdx
        && m_vecIdxSorted == other.m_vecIdxSorted
        && m_vecBench == other.m_vecBench
        ;
    return fEqual;
}

//////////////////////////////////////////////////////////////////////////

void CState::InitData()
{
    CCard cardInvalid;

    //clear sorted idx
    m_vecIdxSorted.resize(c_size, cardInvalid);   //4
    //for(int idx=0; idx< (int)m_vecIdxSorted.size(); idx++){
    //    m_vecIdxSorted[idx] = -1;
    //}

    //bench
    m_vecBench.clear();  //4

    m_vecVecIdx.resize(c_colSize);  //8
    for(int idx=0; idx < (int)m_vecVecIdx.size(); idx++ )
    {
        m_vecVecIdx[idx].resize(0);
        m_vecVecIdx[idx].reserve(10);
    }
}


//all card sorted
bool CState::FWin() const
{
    //1st bench empty
    if(m_vecBench.size() >0)
        return false;
    //for(int idx=0; idx< m_vecBench.size(); idx++){
    //    if(m_vecBench[idx]>=0 )
    //        return false;
    //}

    //check 8 column, all sorted
    for(int colIdx=0; colIdx< (int)m_vecVecIdx.size(); colIdx++)
    {
        if(!FSorted(m_vecVecIdx[colIdx]) )
            return false;
    }

    return true;
}

bool CState::FSorted(const VecCard& vecIdx) const
{
    //int cardIdx = -1;
    bool fRedPre = false, fRed = false;
    int numberPre = -1, number = 0;

    if(vecIdx.size() < 1)
        return true;

    CCard cardPre(vecIdx[0]);
    CCard card;

    for(int idx=1; idx< (int)vecIdx.size(); idx++)
    {
        card = (vecIdx[idx]);  //setidx
        if(cardPre.CanAttach(card)            ){
            cardPre = card;
            continue;
        }
        return false;
    }
    return true;
}

bool CState::FCanMove() const
{
    //check bench
    if(IsBenchHaveBlank() )
        return true;

    //check each column
    for(int colIdx =0; colIdx < (int)m_vecVecIdx.size(); colIdx++)
    {
        if(FCanMove(colIdx) )
            return true; 
    }
    return false;
}

bool CState::FCanMove(int curColIdx) const
{   
    if(curColIdx <0
        || curColIdx >= (int)m_vecVecIdx.size() )
        return false;

    const VecCard& curVecIdx = m_vecVecIdx[curColIdx];

    //check move to sorted
    if(FCanMoveToSorted(curVecIdx) )
        return true;

    //check move to other column
    const int curMoveMax = GetCurMoveCardAmount();
    if(curMoveMax < 1){
        assert(false);
        return false;
    }

    //get a max list of sorted card of this column
    VecCard vecSorted ;
    GetLastSortedList(curVecIdx, curMoveMax, vecSorted );

    //move max, to move 1 card
    CCard cardIdx ;
    const int sortSize = (int)vecSorted.size();
    for(int idx = 0; idx < sortSize; idx++)
    {
        cardIdx = vecSorted[idx];
        for(int colIdx = 0; colIdx < (int)m_vecVecIdx.size(); colIdx++)
        {
            //pass self
            if(colIdx == curColIdx)
                continue;

            if(FCanPushToEnd(m_vecVecIdx[colIdx], cardIdx) )
                return true;
        }
    }
    return false;
}

void CState::GenerateSonState(VecState& vecState) 
{
    vecState.clear();

    if(!FCanMove() 
        || m_idxSon.size() ){
            return;
    }

    //CState dummy = *this;

    for(UINT colIdx=0; colIdx < m_vecVecIdx.size(); colIdx++)
    {
        //move to sorted
        MoveColToSorted(vecState, colIdx);
        //move to bench
        MoveColToBench(vecState, colIdx);

        for(UINT colIdxDest =0; colIdxDest < m_vecVecIdx.size(); colIdxDest++)
        {
            MoveColToCol(vecState, colIdx, colIdxDest);
        }
    }
    

    //bench
    for(UINT benchIdx=0; benchIdx< m_vecBench.size(); benchIdx++)
    {
        MoveBenchToSorted(vecState, benchIdx);
        for(UINT colIdxDest =0; colIdxDest < m_vecVecIdx.size(); colIdxDest++)
        {
            MoveBenchToCol(vecState, benchIdx, colIdxDest);
        }
    }
        
}

bool CState::MoveColToBench(VecState& vecState, UINT colIdx)
{
    if(colIdx>= m_vecVecIdx.size() 
        || m_vecVecIdx[colIdx].size() < 1
        || m_vecBench.size() >= c_size)
        return false;
    
    vecState.push_back(*this);
    CState& staNew = *vecState.rbegin();

    staNew.m_idxFather = m_id;  //set father

    VecCard& vecCardNew = staNew.m_vecVecIdx[colIdx];
    VecCard::iterator it = vecCardNew.end()-1;
    const CCard& card = *it;

    staNew.m_vecBench.push_back(card);     
    vecCardNew.erase(it);

    staNew.CalculateValue();
    return true;
}

bool CState::MoveColToSorted(VecState& vecState, UINT colIdx)
{
    if(colIdx >= m_vecVecIdx.size() )
        return false;
    const VecCard& vecCard = m_vecVecIdx[colIdx];
    if(vecCard.size() < 1)
        return false;
    const CCard& card = vecCard[vecCard.size()- 1];
    const eType type = card.GetType();

    //check sort
    const CCard& cardSortLast = m_vecIdxSorted[type];
    if(  e1 == card.GetNumber() //must be A
        || (cardSortLast.IsLegal()
        && card.CanAttach(cardSortLast) )  
        )            
    {      
        vecState.push_back(*this);
        CState& staNew = *vecState.rbegin();

        staNew.m_idxFather = m_id;  //set father

        staNew.m_vecIdxSorted[type] = card; //move to sorted
        VecCard& vecCardNew = staNew.m_vecVecIdx[colIdx];
        vecCardNew.erase(vecCardNew.end()-1);

        staNew.CalculateValue();
        return true;
    }
    return false;
}

bool CState::MoveColToCol(VecState& vecState, UINT colIdxSrc, UINT colIdxDest)  //todo, move more cards
{
    if(colIdxSrc == colIdxDest
        || colIdxSrc >= m_vecVecIdx.size()
        || colIdxDest >= m_vecVecIdx.size() 
        || m_vecVecIdx[colIdxSrc].size() < 1)
        return false;

    const VecCard& vecCardSrc = m_vecVecIdx[colIdxSrc];
    const VecCard& vecCardDest = m_vecVecIdx[colIdxDest];
    const CCard& cardSrc = *vecCardSrc.rbegin();
    if(vecCardDest.size() ){
        const CCard& cardDest = *vecCardDest.rbegin();
        if(!cardDest.CanAttach(cardSrc) )
            return false;
    }
    else{
        //move to blank. 
    }

    vecState.push_back(*this);
    CState& staNew = *vecState.rbegin();

    staNew.m_idxFather = m_id;

    staNew.m_vecVecIdx[colIdxDest].push_back(cardSrc);
    VecCardIt it = staNew.m_vecVecIdx[colIdxSrc].end()-1;
    staNew.m_vecVecIdx[colIdxSrc].erase(it );

    staNew.CalculateValue();
    //vecState.push_back(staNew);
    return false;
}

bool CState::MoveBenchToSorted(VecState& vecState, UINT benchIdx)
{
    if(benchIdx >= m_vecBench.size() )
        return false;

    const CCard& card = m_vecBench[benchIdx];
    const eType type = card.GetType();
    const CCard& cardSortLast = m_vecIdxSorted[type];
    if(!card.CanAttach(cardSortLast) )
        return false;
    
    VecCardIt it = m_vecBench.begin();
    for(int idx=0; idx< m_vecBench.size(); idx++, it++)
    {
        if(idx != benchIdx)
            continue;
                
        vecState.push_back(*this);
        CState& staNew = *vecState.rbegin();
        staNew.m_idxFather = m_id;

        staNew.m_vecIdxSorted[type] = card;
        staNew.m_vecBench.erase(staNew.m_vecBench.begin() + idx);

        staNew.CalculateValue();
        //vecState.push_back(staNew);
        break;
    }
    return true;
}

bool CState::MoveBenchToCol(VecState& vecState, UINT benchIdx, UINT colIdx)
{
    if(benchIdx >= m_vecBench.size() 
        || colIdx>= m_vecVecIdx.size() )
        return false;

    const CCard& card = m_vecBench[benchIdx];
    const eType type = card.GetType();
    const VecCard& vecCardDest = m_vecVecIdx[colIdx];
    if(vecCardDest.size() > 0 ){
        const CCard& cardCol = *vecCardDest.rbegin(); 
        if(!cardCol.CanAttach(card) )
            return false;
    }

    vecState.push_back(*this);
    CState& staNew = *vecState.rbegin();

    staNew.m_idxFather = m_id;

    VecCard vecNew = staNew.m_vecVecIdx[colIdx];
    vecNew.push_back(card);

    VecCardIt it = staNew.m_vecBench.begin() + benchIdx;
    staNew.m_vecBench.erase(it);

    staNew.CalculateValue();
    //vecState.push_back(staNew);

    return true;
}
//////////////////////////////////////////////////////////////////////////

void CState::GetLastSortedList(const VecCard& vecIdx
                                   , int moveMax
                                   , VecCard& vecIdxSorted) const
{
    vecIdxSorted.clear();

    const int vecIdxSize = (int)vecIdx.size();
    if(vecIdxSize < 1)
        return;
    if(vecIdxSize < moveMax )
        moveMax = vecIdxSize;

    //VecIdx idxBackSort;
    //int cardIdx = -1;
    CCard cardPre;
    CCard card;
    for(int moveCount=0; moveCount < moveMax; moveCount++)
    {        
        card = vecIdx[vecIdxSize - moveCount-1];
        //card.SetIdx(cardIdx);
        if(0 != moveCount ){
            //check if can push
            if(!card.CanAttach(cardPre) )
                break;
        }
        vecIdxSorted.insert(vecIdxSorted.begin(), card );

        cardPre = card;
    }
}

int  CState::GetCurMoveCardAmount() const
{
    int amount = 1;

    //get empty bench
    amount += c_size - (int)m_vecBench.size();
    //for(int idx=0; idx< m_vecBench.size(); idx++)
    //{
    //    if(!m_vecBench[idx].IsLegal())  //-1 ==
    //        amount++;
    //}

    //get empty column
    for(int colIdx = 0; colIdx < (int)m_vecVecIdx.size(); colIdx++)
    {
        if(m_vecVecIdx[colIdx].size() < 1)
            amount++;
    }
    return amount;
}

//move to right up corner
bool  CState::FCanMoveToSorted(const VecCard& vecIdx) const
{
    if(vecIdx.size() < 1)
        return false;

    CCard lastCard(*vecIdx.rbegin());

    CCard tempCard;
    //int cardIdx = -1;
    for(int idx=0; idx< (int)m_vecIdxSorted.size(); idx++)
    {
        tempCard = m_vecIdxSorted[idx];
        //tempCard.SetIdx(cardIdx);
        //blank
        if(!tempCard.IsLegal() ){  //-1 == cardIdx
            if(eK == lastCard.GetNumber() )
                return true;
            else
                continue;
        }
        if(tempCard.GetType() == lastCard.GetType() 
            && abs(tempCard.GetNumber() -lastCard.GetNumber() )==1
            )
            return true;
    }
    return false;
}

bool CState::FCanPushToEnd(const VecCard& vecIdx, CCard card) const
{
    //blank column
    if(vecIdx.size() < 1)
        return true;

    if(!card.IsLegal() )
        return false;

    const CCard lastCard(*vecIdx.rbegin() );
    //const CCard card(cardIdx);

    if(lastCard.CanAttach(card) )
        return true;

    return false;
}

//can move 1 card, return true
bool CState::IsBenchHaveBlank() const  
{
    if(m_vecBench.size() < c_size)
        return true;
    //for(int idx=0; idx< m_vecBench.size(); idx++)
    //{
    //    if(m_vecBench[idx] < 0)
    //        return true;
    //}
    return false;
}

bool CState::CheckInputDataLegal() const //todo
{   
    return true;
}

double CState::GetValue() const
{
    return m_value;
}

double CState::CalculateValue()
{
    const double c_valueBench = 2.0;
    const double c_valueColumn = 3.0;
    const double c_valueSortedCard = 1.0;

    if(FWin() ){
        m_value = 0;
        return m_value;
    }
    double valueGood = 0;

    //bench
    const int benchBlank = c_size - (int)m_vecBench.size();
    valueGood += benchBlank* c_valueBench;

    //sorted
    int idx =0;
    for(int i=0; i< m_vecIdxSorted.size(); i++){
        const CCard& card = m_vecIdxSorted[i];
        if(!card.IsLegal())
            continue;
        valueGood += card.GetNumber() * c_valueSortedCard;
    }

    // column
    for(int colIdx =0; colIdx < m_vecVecIdx.size(); colIdx++)
    {
        const VecCard& vecCard = m_vecVecIdx[colIdx];
        //blank
        if(m_vecVecIdx[colIdx].empty() ){
            valueGood += c_valueColumn ;
            continue;
        }

        //sorted chain
        for(UINT cardIdx=0; cardIdx < vecCard.size()-1; cardIdx++)
        {
            const CCard& cardPre = vecCard[cardIdx];
            const CCard& cardNext = vecCard[cardIdx+1];
            if(cardPre.CanAttach(cardNext) ){
                valueGood += c_valueSortedCard;
            }
        }
    }

    //column sorted cards  
    for(int colIdx =0; colIdx < m_vecVecIdx.size(); colIdx++)
    {
        
    }
    
    m_value = c_valueInit - valueGood;
    
    return m_value;
}
//////////////////////////////////////////////////////////////////////////

#define ADD_CARD(color, number)       m_vecVecIdx[curCol].push_back(CCard(color, number) );

void CState::InputData()
{
    int curCol = 0;
    //m_vecVecIdx[curCol].push_back(GetIdx(, ) );

    //0
    ADD_CARD(eDiamond, e6);
    ADD_CARD(eSpade, e9);
    ADD_CARD(eHeart, e10);
    ADD_CARD(eDiamond, eK);
    ADD_CARD(eDiamond, e7);
    ADD_CARD(eDiamond, eJ);
    ADD_CARD(eHeart, e9);

    curCol++; //1
    ADD_CARD(eDiamond, eQ);
    ADD_CARD(eClub, e2);
    ADD_CARD(eHeart, e2);
    ADD_CARD(eClub, eJ);
    ADD_CARD(eHeart, e3);
    ADD_CARD(eClub, e3);
    ADD_CARD(eDiamond, e5);

    curCol++;  //2
    ADD_CARD(eHeart, eJ);
    ADD_CARD(eClub, e1);
    ADD_CARD(eHeart, eQ);
    ADD_CARD(eSpade, e1);
    ADD_CARD(eSpade, e4);
    ADD_CARD(eDiamond, e4);
    ADD_CARD(eSpade, e8);

    curCol++; //3
    ADD_CARD(eClub, e7);
    ADD_CARD(eHeart, e7);
    ADD_CARD(eDiamond, e3);
    ADD_CARD(eHeart, e8);
    ADD_CARD(eSpade, e6);
    ADD_CARD(eSpade, eQ);
    ADD_CARD(eClub, e8);

    curCol++; //4
    ADD_CARD(eSpade, e2);
    ADD_CARD(eDiamond, e8);
    ADD_CARD(eClub, e5);
    ADD_CARD(eSpade, e5);
    ADD_CARD(eHeart, eK);
    ADD_CARD(eClub, e6);
    //ADD_CARD(e, e);

    curCol++; //5
    ADD_CARD(eSpade, eJ);
    ADD_CARD(eHeart, e1);
    ADD_CARD(eClub, e9);
    ADD_CARD(eClub, eK);
    ADD_CARD(eClub, eQ);
    ADD_CARD(eDiamond, e2);
    //ADD_CARD(e, e);

    curCol++;  //6
    ADD_CARD(eSpade, e3);
    ADD_CARD(eHeart, e5);
    ADD_CARD(eClub, e10);
    ADD_CARD(eDiamond, e10);
    ADD_CARD(eSpade, e7);
    ADD_CARD(eSpade, eK);

    curCol++;  //6
    ADD_CARD(eHeart, e4);
    ADD_CARD(eSpade, e10);
    ADD_CARD(eDiamond, e9);
    ADD_CARD(eDiamond, e1);
    ADD_CARD(eClub, e4);
    ADD_CARD(eHeart, e6);
}