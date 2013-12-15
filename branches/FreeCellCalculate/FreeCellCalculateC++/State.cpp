#include "stdafx.h"
#include "State.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////
CState::CState()
{
    InitData();
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
    for(int idx=0; idx <m_vecVecIdx.size(); idx++ )
    {
        m_vecVecIdx[idx].resize(0);
        m_vecVecIdx[idx].reserve(10);
    }
}


//all card sorted
bool CState::FWin() const
{
    //1st bench empty
    //for(int idx=0; idx< m_vecBench.size(); idx++){
    //    if(m_vecBench[idx]>=0 )
    //        return false;
    //}

    //check 8 column, all sorted
    for(int colIdx=0; colIdx< m_vecVecIdx.size(); colIdx++)
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

    for(int idx=1; idx< vecIdx.size(); idx++)
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
    for(int colIdx =0; colIdx < m_vecVecIdx.size(); colIdx++)
    {
        if(FCanMove(colIdx) )
            return true; 
    }
    return false;
}

bool CState::FCanMove(int curColIdx) const
{   
    if(curColIdx <0
        || curColIdx >= m_vecVecIdx.size() )
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
        for(int colIdx = 0; colIdx < m_vecVecIdx.size(); colIdx++)
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
    for(int colIdx = 0; colIdx < m_vecVecIdx.size(); colIdx++)
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
    for(int idx=0; idx< m_vecIdxSorted.size(); idx++)
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

bool CState::CheckInputDataLegal() const
{
    //todo
    return true;
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
