#include "stdafx.h"
#include "State.h"
#include <assert.h>

using namespace std;

const UINT c_id = 0xffffffff;
const UINT16 c_valueInit = 1000;

const UINT16 c_valueBench = 1;
const UINT16 c_valueColumn = 10;
const UINT16 c_valueSortedCard = 5;
const UINT16 c_valueSortedInColumn = 1;
const UINT16 c_valueSortedChain = 1;

const int c_maxStep = 60+ c_cardAll;

//////////////////////////////////////////////////////////////////////////
CState::CState()
    :m_id(0xffffffff)
    ,m_value(c_valueInit)
    ,m_step(0)
{
    InitData();
}

//caution
//if add member , must add to this func

//#define COPY_MEMBER(var)    var(other.var)
//CState::CState(const CState& other)
//    :COPY_MEMBER(m_id)
//    ,COPY_MEMBER(m_idxFather)
//    ,COPY_MEMBER(m_idxSon)
//    ,COPY_MEMBER(m_value)
//    ,COPY_MEMBER(m_vecIdxSorted)
//    ,COPY_MEMBER(m_vecBench)
//    ,COPY_MEMBER(m_vecVecIdx)
//    ,COPY_MEMBER(m_str)
//    ,COPY_MEMBER(m_step)
//{
//
//}

//CState& operater= (const CState& othe )
//{
//
//}

bool CState::operator == (const CState& other) const
{
    const bool fEqual =
        m_value == other.m_value
        && m_str == other.m_str
        //m_id == other.m_id
        //m_vecVecIdx == other.m_vecVecIdx
        //&& m_vecIdxSorted == other.m_vecIdxSorted
        //&& m_vecBench == other.m_vecBench
        ;
    return fEqual;
}

bool CState::operator < (const CState& other) const
{
    if(
        //m_value < other.m_value         
        m_str < other.m_str
        )
        return true;
    return false;
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
    //m_vecBench.clear();  //4
    m_vecBench.resize(c_size, cardInvalid);

    m_vecVecIdx.resize(c_colSize);  //8
    //for(int idx=0; idx < (int)m_vecVecIdx.size(); idx++ )
    //{
    //    ListCard& listCard = m_vecVecIdx[idx];
    //    //listCard.resize(0);
    //    //listCard.reserve(10);
    //}
}


//all card sorted
bool CState::FWin() const
{
    //1st bench empty
    /*if(m_vecBench.size() >0)
    return false;*/
    for(int idx=0; idx< m_vecBench.size(); idx++){
        if(m_vecBench[idx].IsLegal() )
            return false;
    }

    //check 8 column, all sorted
    for(int colIdx=0; colIdx< (int)m_vecVecIdx.size(); colIdx++)
    {
        if(!FSorted(m_vecVecIdx[colIdx]) )
            return false;
    }

    return true;
}

bool CState::FSorted(const ListCard& cardArray) const
{
    //int cardIdx = -1;
    bool fRedPre = false, fRed = false;
    int numberPre = -1, number = 0;

    if(cardArray.size() < 1)
        return true;


    ListCardConstIt it  = cardArray.begin();
    CCard cardPre = *it;
    //CCard card;

    for(it++;
        it != cardArray.end();
        it++)
    {
        const CCard& curCard = *it;
        //card = (cardArray[idx]);  //setidx
        if(cardPre.CanAttach(curCard)  ){
            cardPre = curCard;
            continue;
        }
        return false;
    }
    return true;
}

bool CState::FCanMove() const
{
    //check bench
    //UINT benchIdx = 0;
    if(GetBenchEmptyCount() >0 )
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

    const ListCard& curVecIdx = m_vecVecIdx[curColIdx];

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
    ListCard vecSorted ;
    GetLastSortedList(curVecIdx, curMoveMax, vecSorted );

    //move max, to move 1 card
    //CCard cardIdx ;
    const int sortSize = (int)vecSorted.size();
    for(ListCardConstIt it = vecSorted.begin(); 
        it != vecSorted.end(); 
        it++)
    {
        const CCard&  card = *it;
        for(int colIdx = 0; colIdx < (int)m_vecVecIdx.size(); colIdx++)
        {
            //pass self
            if(colIdx == curColIdx)
                continue;

            if(FCanPushToEnd(m_vecVecIdx[colIdx], card) )
                return true;
        }
    }
    return false;
}

void CState::GenerateSonState(ListState& vecState) 
{
    vecState.clear();

    if(!FCanMove() 
        || m_idxSon.size() 
        || m_step > c_maxStep){
            return;
    }

    //CState dummy = *this;
    ListCard vecLastSorted ;

    for(UINT colIdx=0; colIdx < m_vecVecIdx.size(); colIdx++)
    {
        ListCard& vecCard = m_vecVecIdx[colIdx];

        //get a max list of sorted card of this column     
        GetLastSortedList(vecCard, c_cardNumberMax, vecLastSorted ); //todo ,max 13 cards

        //move to sorted
        MoveColToSorted(vecState, colIdx);
        //move to bench
        MoveColToBench(vecState, colIdx);

        for(UINT colIdxDest =0; colIdxDest < m_vecVecIdx.size(); colIdxDest++)
        {       
            MoveColToCol(vecState, colIdx, colIdxDest, vecLastSorted );
        }
    }
    
    //bench
    for(UINT benchIdx=0; benchIdx< m_vecBench.size(); benchIdx++)
    {
        const CCard& card = m_vecBench[benchIdx];
        if(!card.IsLegal() )
            continue;

        MoveBenchToSorted(vecState, benchIdx);

        for(UINT colIdxDest =0; colIdxDest < m_vecVecIdx.size(); colIdxDest++)
        {
            MoveBenchToCol(vecState, benchIdx, colIdxDest);
        }
    }
        
}

bool CState::MoveColToBench(ListState& vecState, UINT colIdx)
{
    if(colIdx>= m_vecVecIdx.size() 
        || m_vecVecIdx[colIdx].size() < 1
        //|| m_vecBench.size() >= c_size        
        )
        return false;
    UINT benchIdx = 0;
    if(!FCanMoveToBench(benchIdx) )
         return false;
    
    vecState.push_back(*this);
    CState& staNew = *vecState.rbegin();

    SetIdxFather(staNew);  //set father

    ListCard& vecCardNew = staNew.m_vecVecIdx[colIdx];
    ListCard::iterator it = vecCardNew.end();
    it--;
    const CCard& card = *it;

    staNew.m_vecBench[benchIdx] = (card);     
    vecCardNew.erase(it);

    staNew.Update();
    return true;
}

bool CState::MoveColToSorted(ListState& vecState, UINT colIdx)
{
    if(colIdx >= m_vecVecIdx.size() )
        return false;
    const ListCard& vecCard = m_vecVecIdx[colIdx];
    if(vecCard.size() < 1)
        return false;
    const CCard& card = *vecCard.rbegin();  //last one
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

        SetIdxFather(staNew);  //set father

        staNew.m_vecIdxSorted[type] = card; //move to sorted
        ListCard& vecCardNew = staNew.m_vecVecIdx[colIdx];
        ListCardIt it = vecCardNew.end();
        it--;
        vecCardNew.erase(it);
        //vecCardNew.erase(vecCardNew.rbegin() );

        staNew.Update();
        return true;
    }
    return false;
}

//move more cards
bool CState::MoveColToCol(ListState& vecState
                          , UINT colIdxSrc
                          , UINT colIdxDest
                          , const ListCard& vecLastSorted
                          )  
{
    if(colIdxSrc == colIdxDest
        || colIdxSrc >= m_vecVecIdx.size()
        || colIdxDest >= m_vecVecIdx.size() 
        || m_vecVecIdx[colIdxSrc].size() < 1)
        return false;

    int moveAmount = 0;

    UINT idx = 0;
    for(ListCardConstIt it = vecLastSorted.begin();
        it != vecLastSorted.end();
        idx++, it++)
    {
        //const VecCard& vecCardSrc = m_vecVecIdx[colIdxSrc];
        const ListCard& vecCardDest = m_vecVecIdx[colIdxDest];

        const CCard& cardSrc = *it;
        if(vecCardDest.size() ){
            const CCard& cardDest = *vecCardDest.rbegin();
            if(!cardDest.CanAttach(cardSrc) )
                continue;
        }
        else{
            //move to blank. 
        }

        //get move amount
        moveAmount = (UINT)vecLastSorted.size() - idx;
        if(moveAmount< 1){
            ASSERT(false);
            break;
        }            

        //create new state
        vecState.push_back(*this);
        CState& staNew = *vecState.rbegin();

        SetIdxFather(staNew);

        for(ListCardConstIt itAdd = it;
            itAdd != vecLastSorted.end();
            itAdd++){
            staNew.m_vecVecIdx[colIdxDest].push_back(*itAdd );
        }

        ListCard& vecSrcNew = staNew.m_vecVecIdx[colIdxSrc];
        ListCardIt itStart = vecSrcNew.begin(); // - moveAmount;
        advance(itStart, vecSrcNew.size() - moveAmount);
        ListCardIt itEnd = vecSrcNew.end(); //todo, if need -1
        vecSrcNew.erase(itStart, itEnd );

        staNew.Update();
    }
    
    //vecState.push_back(staNew);
    return false;
}

bool CState::MoveBenchToSorted(ListState& vecState, UINT benchIdx)
{
    if(benchIdx >= m_vecBench.size() )
        return false;

    const CCard& card = m_vecBench[benchIdx];
    if(!card.IsLegal() )
        return false;

    const eType type = card.GetType();
    const CCard& cardSortLast = m_vecIdxSorted[type];
    if(!card.CanAttach(cardSortLast) )
        return false;
    
    VecCardIt it = m_vecBench.begin();
    for(int idxSorted = 0; idxSorted< m_vecIdxSorted.size(); idxSorted++, it++)
    {
        //if(idxSorted != benchIdx)
        //    continue;
                
        vecState.push_back(*this);
        CState& staNew = *vecState.rbegin();
        SetIdxFather(staNew);

        staNew.m_vecIdxSorted[type] = card;
        //staNew.m_vecBench.erase(staNew.m_vecBench.begin() + idx);
        staNew.m_vecBench[benchIdx].Disable();

        staNew.Update();
        //vecState.push_back(staNew);
        break;
    }
    return true;
}

bool CState::MoveBenchToCol(ListState& vecState, UINT benchIdx, UINT colIdx)
{
    if(benchIdx >= m_vecBench.size() 
        || colIdx>= m_vecVecIdx.size() )
        return false;

    const CCard& cardSrc = m_vecBench[benchIdx];
    if(!cardSrc.IsLegal() )
        return false;

    const eType type = cardSrc.GetType();
    const ListCard& vecCardDest = m_vecVecIdx[colIdx];
    if(vecCardDest.size() > 0 ){
        const CCard& cardCol = *vecCardDest.rbegin(); 
        if(!cardCol.CanAttach(cardSrc) )
            return false;
    }

    vecState.push_back(*this);
    CState& staNew = *vecState.rbegin();

    SetIdxFather(staNew);

    ListCard& vecNew = staNew.m_vecVecIdx[colIdx];
    vecNew.push_back(cardSrc);

    //VecCardIt it = staNew.m_vecBench.begin() + benchIdx;
    //staNew.m_vecBench.erase(it);
    staNew.m_vecBench[benchIdx].Disable();

    staNew.Update();
    //vecState.push_back(staNew);

    return true;
}

void CState::SetIdxFather(CState& stSon)
{
    stSon.m_idxFather = m_id;
    stSon.m_step = m_step+1;
}
//////////////////////////////////////////////////////////////////////////

void CState::GetLastSortedList(const ListCard& vecCard
                               , int moveMax
                               , ListCard& vecIdxSorted) const
{
    vecIdxSorted.clear();

    const int vecIdxSize = (int)vecCard.size();
    if(vecIdxSize < 1)
        return;
    if(vecIdxSize < moveMax )
        moveMax = vecIdxSize;

    //VecIdx idxBackSort;
    //int cardIdx = -1;
    CCard cardPre;
    //CCard card;

    ListCard::const_reverse_iterator it = vecCard.rbegin();
    for(int moveCount=0;
        it != vecCard.rend() && moveCount < moveMax;
        it++, moveCount++
        )
    {        
        //card = vecCard[vecIdxSize - moveCount-1];
        //card.SetIdx(cardIdx);
        const CCard& card = *it;
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
    //amount += c_size - (int)m_vecBench.size();
    for(int idx=0; idx< m_vecBench.size(); idx++)
    {
        if(!m_vecBench[idx].IsLegal())  //-1 ==
            amount++;
    }

    //get empty column
    for(int colIdx = 0; colIdx < (int)m_vecVecIdx.size(); colIdx++)
    {
        if(m_vecVecIdx[colIdx].size() < 1)
            amount++;
    }
    return amount;
}

bool CState::FCanMoveToBench(UINT& benchIdx) const 
{
    VecCard::const_iterator it ;
    benchIdx = 0;
    for(it= m_vecBench.begin();
        it != m_vecBench.end();
        it++, benchIdx++)
    {
        const CCard& card = *it;
        if(!card.IsLegal() ){
            return true;
        }
    }
    return false;
}

UINT CState::GetBenchEmptyCount () const
{
    VecCard::const_iterator it ;
    UINT benchCount = 0;
    for(it= m_vecBench.begin();
        it != m_vecBench.end();
        it++)
    {
        const CCard& card = *it;
        if(card.IsLegal() ){
            continue;
        }
        benchCount++;
    }
    return benchCount;
}

//move to right up corner
bool  CState::FCanMoveToSorted(const ListCard& vecIdx) const
{
    if(vecIdx.size() < 1)
        return false;

    const CCard& lastCard = (*vecIdx.rbegin());

    //CCard tempCard;
    //int cardIdx = -1;
    for(int idx=0; idx< (int)m_vecIdxSorted.size(); idx++)
    {
        const CCard& tempCard = m_vecIdxSorted[idx];
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

bool CState::FCanPushToEnd(const ListCard& vecIdx, const CCard& card) const
{
    //blank column
    if(vecIdx.size() < 1)
        return true;

    if(!card.IsLegal() )
        return false;

    const CCard& lastCard = (*vecIdx.rbegin() );
    //const CCard card(cardIdx);

    if(lastCard.CanAttach(card) )
        return true;

    return false;
}

//can move 1 card, return true
//bool CState::IsBenchHaveBlank() const  

//bool CState::CheckInputDataLegal() const //todo
//{   
//    return true;
//}

UINT16 CState::GetValue() const
{
    return m_value;
}

double CState::UpdateValue()
{
    if(FWin() ){
        m_value = 0;
        return m_value;
    }
    UINT valueGood = 0;

    //bench
    const int benchBlank = GetBenchEmptyCount();
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
    int curChainLen = 0, i=0;
    ListCardConstIt it  ;
    CCard cardPre;

    //column sorted cards  
    for(int colIdx =0; colIdx < m_vecVecIdx.size(); colIdx++)
    {
        const ListCard& vecCard = m_vecVecIdx[colIdx];

        //blank
        if(vecCard.empty() ){
            valueGood += c_valueColumn ;
            continue;
        }

        //sorted chain
        //itEnd = vecCard.end();
        it = vecCard.begin();
        cardPre = *it;
        i=0;
        for(it++ ;
            it != vecCard.end();
            it++, i++)
        {
            //const CCard& cardPre = *it;
            const CCard& card = *(it);
            if(cardPre.CanAttach(card) ){
                valueGood += c_valueSortedInColumn + curChainLen * c_valueSortedChain;
                curChainLen++;
            }
            else{
                curChainLen = 0;
            }
            cardPre = card;
        }
    }

    //get minus value
    m_value = c_valueInit - valueGood;
    
    return m_value;
}

void CState::UpdateString()
{
    string& str = m_str;
    str = "";

    //bench
    for(UINT idx=0; idx< m_vecBench.size(); idx++ )    {
        const CCard& card = m_vecBench[idx];
        str += card.GetString();        
    }
    str+=",";

    //sorted
    for(UINT idx=0; idx< m_vecIdxSorted.size(); idx++)
    {
        str+= m_vecIdxSorted[idx].GetString();
    }
    str+=",";

    //column
    UINT colIdx=0, cardIdx = 0;
    ListCardConstIt it ;
    for(colIdx=0; colIdx< m_vecVecIdx.size(); colIdx++)
    {
        const ListCard& cards = m_vecVecIdx[colIdx];
        for(it = cards.begin(); 
            it != cards.end(); 
            it++)
        {
            str+= (*it).GetString();
        }
        str+=",";
    }
}

void  CState::Update()
{
    UpdateValue();
    UpdateString();
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
