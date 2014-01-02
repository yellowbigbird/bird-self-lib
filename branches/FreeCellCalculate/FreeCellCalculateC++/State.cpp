#include "stdafx.h"
#include "State.h"
#include <assert.h>
//#include <ASSERT>

#include "Calculate.h"
#include "jhash.h"
#include "step.h"

using namespace std;

const UINT c_id = 0xffffffff;
const UINT16 c_valueInit = 1000;

const UINT16 c_valueBench = 1;
const UINT16 c_valueColumn = 10;
const UINT16 c_valueSortedCard = 5;
const UINT16 c_valueSortedInColumn = 1;
const UINT16 c_valueSortedChain = 1;

const int c_maxStep = 70 ; //c_cardAll

//////////////////////////////////////////////////////////////////////////
CState::CState()
    :m_id(0xffffffff)
    ,m_value(c_valueInit)
    ,m_step(0)
    ,m_hasGenSon(false)
    //,m_dead(false)
{
    InitData();
}

bool CState::operator == (const CState& other) const
{
    const bool fEqual =
        m_value == other.m_value
        && m_hash == other.m_hash
        //&& m_str == other.m_str
        //m_id == other.m_id
        //m_vecVecIdx == other.m_vecVecIdx
        //&& m_vecIdxSorted == other.m_vecIdxSorted
        //&& m_vecBench == other.m_vecBench
        ;
    return fEqual;
}

bool CState::operator < (const CState& other) const
{
    if( m_hash< other.m_hash
        //m_value < other.m_value         
            //m_str < other.m_str
                )
                return true;
    return false;
}

void CState::CopyFromFather(const CState& stFather)
{
    m_id = -1;
    m_idxFather = stFather.m_id;
    m_hasGenSon = false;
    m_idxSon.clear();
    m_value = c_valueInit;
    m_step = stFather.m_step +1;
    //m_str;

    m_vecIdxSorted = stFather.m_vecIdxSorted;  
    m_vecBench = stFather.m_vecBench   ;      
    m_vecVecIdx = stFather.m_vecVecIdx;
}

//////////////////////////////////////////////////////////////////////////

void CState::InitData()
{
    CCard cardInvalid;

    //clear sorted idx
    m_vecIdxSorted.resize(c_size, cardInvalid);   //4
  
    //bench   
    m_vecBench.resize(c_size, cardInvalid);

    m_vecVecIdx.resize(c_colSize);  //8   
}


//all card sorted
bool CState::FWin() const
{
    //if(m_dead)
    //    return false;

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
        if(cardPre.CanAttachInCol(curCard)  ){
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

bool CState::FCanMove(UINT curColIdx) const
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

UINT CState::GenerateSonState(CCalculate* pCal)  
{
    //vecState.clear();

    if( m_hasGenSon
        || m_idxSon.size() 
        || m_step > c_maxStep
        ||!pCal
        ||!FCanMove() 
        ){
            m_hasGenSon= true;
            return (UINT) m_idxSon.size();
    }
    
    CCalculate& rcal = * pCal;

    ListCard vecLastSorted ;

    for(UINT colIdx=0; colIdx < m_vecVecIdx.size(); colIdx++)
    {
        const ListCard& vecCard = m_vecVecIdx[colIdx];

        //get a max list of sorted card of this column     
        GetLastSortedList(vecCard, c_cardNumberMax, vecLastSorted ); //todo ,max 13 cards

        //move to sorted
        //MoveColToSorted(pCal, colIdx);

        //move to bench
        MoveColToBench(pCal, colIdx);

        for(UINT colIdxDest =0; colIdxDest < m_vecVecIdx.size(); colIdxDest++)
        {       
            MoveColToCol(pCal, colIdx, colIdxDest, vecLastSorted );
        }
    }
    
    //bench
    for(UINT benchIdx=0; benchIdx< m_vecBench.size(); benchIdx++)
    {
        const CCard& card = m_vecBench[benchIdx];
        if(!card.IsLegal() )
            continue;
        
        //MoveBenchToSorted(pCal, benchIdx);

        for(UINT colIdxDest =0; colIdxDest < m_vecVecIdx.size(); colIdxDest++)
        {
            MoveBenchToCol(pCal, benchIdx, colIdxDest);
        }
    }
        
    //must be at last, the son copy father ,also copy all member
    m_hasGenSon = true;
    return (UINT) m_idxSon.size();
}

bool CState::MoveColToBench(CCalculate* pCal, UINT colIdx)
{
    UINT benchIdx = 0;
    if(!pCal
        || colIdx>= m_vecVecIdx.size() 
        || m_vecVecIdx[colIdx].size() < 1
        //|| m_vecBench.size() >= c_size     
        || !FCanMoveToBench(benchIdx) 
        )
        return false;
    CCalculate& rcal = *pCal;
        
    CState staNew ;
    staNew.CopyFromFather(*this);

    ListCard& vecCardNew = staNew.m_vecVecIdx[colIdx];
    ListCard::iterator it = vecCardNew.end();
    it--;
    const CCard& card = *it;

    staNew.m_vecBench[benchIdx] = (card);     
    vecCardNew.erase(it);

    staNew.Update();

    rcal.CheckAndInsertState(staNew, *this);
    return true;
}

bool CState::MoveColToSorted(CCalculate* pCal, UINT colIdx)
{
    if(!pCal
        ||colIdx >= m_vecVecIdx.size() )
        return false;

    CCalculate& rcal = *pCal;
    const ListCard& vecCard = m_vecVecIdx[colIdx];
    if(vecCard.size() < 1)
        return false;

    const CCard& card = *vecCard.rbegin();  //last one
    const eType type = card.GetType();

    //check sort
    const CCard& cardSortLast = m_vecIdxSorted[type];
    if(  eA == card.GetNumber() //must be A
        || (cardSortLast.IsLegal()
        && card.CanAttachSorted(cardSortLast) )  
        )            
    {      
        CState staNew;
        staNew.CopyFromFather(*this);
        
        staNew.m_vecIdxSorted[type] = card; //move to sorted
        ListCard& vecCardNew = staNew.m_vecVecIdx[colIdx];
        ListCardIt it = vecCardNew.end();
        it--;
        vecCardNew.erase(it);

        staNew.Update();

        rcal.CheckAndInsertState(staNew, *this);
        return true;
    }
    return false;
}

bool CState::MoveColToSorted(UINT colIdx)
{
    if(colIdx >= m_vecVecIdx.size() )
        return false;
    ListCard& vecCard = m_vecVecIdx[colIdx];
    if(vecCard.size() < 1)
        return false;

    const CCard& card = *vecCard.rbegin();  //last one
    const eType type = card.GetType();
    const eNumber num = card.GetNumber();

    const CCard& cardSortLast = m_vecIdxSorted[type];
    if(  eA == num //must be A
        || (cardSortLast.IsLegal()
        && card.CanAttachSorted(cardSortLast) )  
        )            
    {      
        m_vecIdxSorted[type] = card; //move to sorted        
        ListCardIt it = vecCard.end();
        it--;
        vecCard.erase(it);
        return true;
    }
    return false;
}

//move more cards
bool CState::MoveColToCol(CCalculate* pCal
                          , UINT colIdxSrc
                          , UINT colIdxDest
                          , const ListCard& vecLastSorted
                          )  
{
    if(!pCal
        ||colIdxSrc == colIdxDest
        || colIdxSrc >= m_vecVecIdx.size()
        || colIdxDest >= m_vecVecIdx.size() 
        || m_vecVecIdx[colIdxSrc].size() < 1)
        return false;

    CCalculate& rcal = *pCal;
    int moveAmount = 0;

    UINT idx = 0;
    for(ListCardConstIt it = vecLastSorted.begin();
        it != vecLastSorted.end();
        idx++, it++)
    {
        const ListCard& vecCardDest = m_vecVecIdx[colIdxDest];

        const CCard& cardSrc = *it;
        if(vecCardDest.size() ){
            const CCard& cardDest = *vecCardDest.rbegin();
            if(!cardDest.CanAttachInCol(cardSrc) )
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
        CState staNew;
        staNew.CopyFromFather(*this);

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

        rcal.CheckAndInsertState(staNew, *this);
    }
    
    //vecState.push_back(staNew);
    return false;
}

bool CState::MoveBenchToSorted(CCalculate* pcal, UINT benchIdx)
{
    if(benchIdx >= m_vecBench.size() 
        || !pcal)
        return false;

    CCalculate& rcal = *pcal;
    const CCard& card = m_vecBench[benchIdx];
    if(!card.IsLegal() )
        return false;

    const eType type = card.GetType();
    const CCard& cardSortLast = m_vecIdxSorted[type];
    if(!card.CanAttachSorted(cardSortLast) )
        return false;
    
    VecCardIt it = m_vecBench.begin();
                   
    CState staNew ;
    staNew.CopyFromFather(*this);

    staNew.m_vecIdxSorted[type] = card;
    staNew.m_vecBench[benchIdx].Disable();

    staNew.Update();

    rcal.CheckAndInsertState(staNew, *this);

    return true;
}

bool CState::MoveBenchToSorted(UINT benchIdx)
{
    if(benchIdx >= m_vecBench.size() )
        return false;

    const CCard& card = m_vecBench[benchIdx];
    if(!card.IsLegal() )
        return false;

    const eType type = card.GetType();
    const CCard& cardSortLast = m_vecIdxSorted[type];
    if(!card.CanAttachSorted(cardSortLast) )
        return false;

    m_vecIdxSorted[type] = card;
    m_vecBench[benchIdx].Disable();
    return true;
}


bool CState::MoveBenchToCol(CCalculate* pCal, UINT benchIdx, UINT colIdx)
{
    if(!pCal
        ||benchIdx >= m_vecBench.size() 
        || colIdx>= m_vecVecIdx.size() )
        return false;
    CCalculate& rcal = *pCal;

    const CCard& cardSrc = m_vecBench[benchIdx];
    if(!cardSrc.IsLegal() )
        return false;

    const eType type = cardSrc.GetType();
    const ListCard& vecCardDest = m_vecVecIdx[colIdx];
    if(vecCardDest.size() > 0 ){
        const CCard& cardCol = *vecCardDest.rbegin(); 
        if(!cardCol.CanAttachInCol(cardSrc) )
            return false;
    }

    CState staNew ;
    staNew.CopyFromFather(*this);
    
    ListCard& vecNew = staNew.m_vecVecIdx[colIdx];
    vecNew.push_back(cardSrc);

    staNew.m_vecBench[benchIdx].Disable();

    staNew.Update();

    //check if staNew new
    rcal.CheckAndInsertState(staNew, *this);

    return true;
}

//////////////////////////////////////////////////////////////////////////

bool CState::UpdateCardToSorted()
{
    bool fCanMove = true;

    while(fCanMove){
        fCanMove = false;

        //bench
        for(UINT benchIdx=0; benchIdx< m_vecBench.size(); benchIdx++)
        {
            const CCard& card = m_vecBench[benchIdx];
            if(!card.IsLegal() )
                continue;
            fCanMove = MoveBenchToSorted(benchIdx)
                || fCanMove ;               
        }

        for(UINT colIdx=0; colIdx < m_vecVecIdx.size(); colIdx++)
        {
            ListCard& vecCard = m_vecVecIdx[colIdx];

            //get a max list of sorted card of this column     
            //GetLastSortedList(vecCard, c_cardNumberMax, vecLastSorted ); //todo ,max 13 cards
            fCanMove = MoveColToSorted(colIdx) || fCanMove ;            
        }
    }
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
            if(!card.CanAttachInCol(cardPre) )
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

    if(lastCard.CanAttachInCol(card) )
        return true;

    return false;
}

//can move 1 card, return true
//bool CState::IsBenchHaveBlank() const  

bool CState::CheckDataLegal() const //todo
{   
	//check cards number
	vector<int> vecCardAmount;
	vecCardAmount.resize(c_size);
	eType et ;
	eNumber en;

	//foreach col
	for(VecListCard::const_iterator itVecList = m_vecVecIdx.begin();
		itVecList != m_vecVecIdx.end();
		itVecList ++)
	{
		const ListCard& listCard = *itVecList;
		for(ListCard::const_iterator itList = listCard.begin();
			itList != listCard.end();
			itList++)
		{
			const CCard& card = *itList;
			et = card.GetType();
			en = card.GetNumber();
			vecCardAmount[et]++;
		}
	}

	//bench
	for(int idx=0; idx< m_vecBench.size(); idx++){
		const CCard& cd = m_vecBench[idx];
		if(!cd.IsLegal() )
			continue;
		et = cd.GetType();
		en = cd.GetNumber();
		vecCardAmount[et]++;
	}

	//sorted
	for(int idx=0; idx< m_vecIdxSorted.size(); idx++){
		const CCard& cd = m_vecIdxSorted[idx];
		if(!cd.IsLegal() )
			continue;
		et = cd.GetType();
		en = cd.GetNumber();
		vecCardAmount[et]+= en +1;
	}

	//check number
	for(int idx=0; idx< vecCardAmount.size(); idx++ )
	{
		if(c_cardNumberMax != vecCardAmount[idx] ){
			ASSERT(false);
			TRACE("ST check fail, id=", m_id);
			return false;
		}
	}

    return true;
}

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
            if(cardPre.CanAttachInCol(card) ){
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

string CState::UpdateString() const
{
    string str ;
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
    return str;
}

void  CState::UpdateVectorInt(VecByte& vecData)const
{
    vecData.reserve(100);

    //bench
    for(UINT idx=0; idx< m_vecBench.size(); idx++ )    {
        const CCard& card = m_vecBench[idx];
        vecData.push_back(card.GetIdx() );        
    }
    vecData.push_back(',');

    //sorted
    for(UINT idx=0; idx< m_vecIdxSorted.size(); idx++)
    {
        const CCard& card = m_vecIdxSorted[idx];
        vecData.push_back(card.GetIdx() );  
    }
    vecData.push_back(',');

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
            const CCard& card = *it;
            vecData.push_back(card.GetIdx() );
        }
        vecData.push_back(',');
    }
}

void CState::UpdateHash()
{
    VecByte vecData;
    UpdateVectorInt(vecData);

    //u32 jhash(const void *key, u32 length, u32 initval);
    m_hash = jhash(&vecData[0], (UINT)vecData.size() );
}

void  CState::Update()
{
    //must be before update value
    UpdateCardToSorted();

	CheckDataLegal();

    UpdateValue();
    //UpdateString();
    UpdateHash();
}
//////////////////////////////////////////////////////////////////////////

#define ADD_CARD(color, number)       m_vecVecIdx[curCol].push_back(CCard(color, number) );

bool  CState::GenerateCards(UINT gameNum)
{
    //int gameNum = 1;	// 牌局号
    const int c_posAll = 168;
    const int c_cardAll = 53;
    int cards[c_posAll], order[c_cardAll];
    int i,j,k;

    for (i=0; i<168; i++)
        cards[i] = -1;
    for (i=0; i< c_cardAll; i++)
        order[i] = i;

    srand(gameNum);
    for (i=0; i<52; i++)
    {
        j = rand() % order[52];
        k = i/8 + (i & 7) * 21;
        cards[k] = order[j];
        order[j] = order[52-i-1];
        --order[52];
    }

    for (i=0;i<c_posAll;i++)
    {
        if (cards[i] == -1){
            printf("%-10d", -1);
            TRACE("%-10d", -1);
        }
        else
        {
            k = ((cards[i]+1 % 13)+3) / 4;
            k = k?k:13;	// 计算牌面大小
            printf("%-2d(%-2X)    ", k, cards[i]);
            TRACE("%-2d(%-2X)    ", k, cards[i]);
        }
    }

    //convert to my format
    int cardId = 0;
    CCard card;
    UINT colIdx = 0;
    bool fMinus = true;
    eNumber cardNumber = eA;
    eType cardType = eDiamond;

    for (i=0; i<c_posAll; i++)
    {
        cardId = cards[i];
        if (cardId == -1){
            //printf("%-10d", -1);
            TRACE("%-10d", -1);
            if(fMinus){
                colIdx ++;
                fMinus = false;
            }
        }
        else
        {
            k = ((cardId +1 % 13)+3) / 4;
            k = k?k:13;	// 计算牌面大小
            cardNumber = (eNumber)(k-1);
            cardType = eType(cardId %4);
            card.SetTypeNumber( cardType, cardNumber);

            //card.SetIdx(cardId-1);  //1-13
            m_vecVecIdx[colIdx].push_back(card);

            //printf("%-2d(%-2X)    ", k, cardId);
            TRACE("%-2d(%-2X)    ", k, cardId);
            fMinus = true;
        }
    }
    return true;
}

bool CState::GenerateStep(CStep* pstep
                          , string& str
                          ,const CState& stNext) const
{
    if(!pstep
        || *this == stNext)
        return false;
    CStep& step = *pstep;

    const int c_bufSize = 5;
    char buf[c_bufSize];
    //buf[1] = 0;

    //check sorted
    //if enable auto-move, move to sorted, not in count
    for(UINT idx=0; idx< m_vecIdxSorted.size(); idx++)
    {
        const CCard& cardCur = m_vecIdxSorted[idx];
        const CCard& cardNext = stNext.m_vecIdxSorted[idx];
        if(cardCur == cardNext)
            continue;
        str += "sorted change to ";
        str += cardNext.GetString();
        str += ", ";
    }

    //check bench
    for(UINT idx=0; idx< m_vecBench.size(); idx++)
    {
        const CCard& cardCur = m_vecBench[idx];
        const CCard& cardNext = stNext.m_vecBench[idx];
        if(cardCur == cardNext)
            continue;
        str += "bench ";
        //sprintf_s(buf, 2, "%d", idx);
        //str += buf;
        _itoa_s(idx+1, buf, 10);
        str += buf;

        str +=" change from ";
        str += cardCur.GetString();
        str += " to ";
        str += cardNext.GetString();
        str += ", ";
    }

    //check col
    for(UINT colIdx =0; colIdx < m_vecVecIdx.size(); colIdx++)
    {
        const ListCard& listCur = m_vecVecIdx[colIdx];
        const ListCard& listNext = stNext.m_vecVecIdx[colIdx];

        if(listCur == listNext)
            continue;

        //sprintf_s(buf, 2, "%d", colIdx);

        str += "col ";
        //str += buf;
        _itoa_s(colIdx +1, buf, 10);
        str += buf;
        str +=" change from ";
        if(listCur.empty() )
            str += "_";
        else
            str += listCur.rbegin()->GetString();
        
        str += " to ";
        if(listNext.empty() )
            str += "_";
        else 
            str += listNext.rbegin()->GetString();
        
        str += ", ";
    }

    return true;
}

//void CState::InputData()
//{
//    int curCol = 0;
//    //m_vecVecIdx[curCol].push_back(GetIdx(, ) );
//
//    //0
//    ADD_CARD(eDiamond, e6);
//    ADD_CARD(eSpade, e9);
//    ADD_CARD(eHeart, e10);
//    ADD_CARD(eDiamond, eK);
//    ADD_CARD(eDiamond, e7);
//    ADD_CARD(eDiamond, eJ);
//    ADD_CARD(eHeart, e9);
//
//    curCol++; //1
//    ADD_CARD(eDiamond, eQ);
//    ADD_CARD(eClub, e2);
//    ADD_CARD(eHeart, e2);
//    ADD_CARD(eClub, eJ);
//    ADD_CARD(eHeart, e3);
//    ADD_CARD(eClub, e3);
//    ADD_CARD(eDiamond, e5);
//
//    curCol++;  //2
//    ADD_CARD(eHeart, eJ);
//    ADD_CARD(eClub, eA);
//    ADD_CARD(eHeart, eQ);
//    ADD_CARD(eSpade, eA);
//    ADD_CARD(eSpade, e4);
//    ADD_CARD(eDiamond, e4);
//    ADD_CARD(eSpade, e8);
//
//    curCol++; //3
//    ADD_CARD(eClub, e7);
//    ADD_CARD(eHeart, e7);
//    ADD_CARD(eDiamond, e3);
//    ADD_CARD(eHeart, e8);
//    ADD_CARD(eSpade, e6);
//    ADD_CARD(eSpade, eQ);
//    ADD_CARD(eClub, e8);
//
//    curCol++; //4
//    ADD_CARD(eSpade, e2);
//    ADD_CARD(eDiamond, e8);
//    ADD_CARD(eClub, e5);
//    ADD_CARD(eSpade, e5);
//    ADD_CARD(eHeart, eK);
//    ADD_CARD(eClub, e6);
//    //ADD_CARD(e, e);
//
//    curCol++; //5
//    ADD_CARD(eSpade, eJ);
//    ADD_CARD(eHeart, eA);
//    ADD_CARD(eClub, e9);
//    ADD_CARD(eClub, eK);
//    ADD_CARD(eClub, eQ);
//    ADD_CARD(eDiamond, e2);
//    //ADD_CARD(e, e);
//
//    curCol++;  //6
//    ADD_CARD(eSpade, e3);
//    ADD_CARD(eHeart, e5);
//    ADD_CARD(eClub, e10);
//    ADD_CARD(eDiamond, e10);
//    ADD_CARD(eSpade, e7);
//    ADD_CARD(eSpade, eK);
//
//    curCol++;  //6
//    ADD_CARD(eHeart, e4);
//    ADD_CARD(eSpade, e10);
//    ADD_CARD(eDiamond, e9);
//    ADD_CARD(eDiamond, eA);
//    ADD_CARD(eClub, e4);
//    ADD_CARD(eHeart, e6);
//}
