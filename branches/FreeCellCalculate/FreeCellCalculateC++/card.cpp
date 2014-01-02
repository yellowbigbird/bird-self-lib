#include "stdafx.h"
#include "card.h"

using namespace std;
using namespace Card;

const UINT8 c_carInvalid = 0xff;

CCard::CCard()
#if CARD_1_BYTE
    :m_num(c_carInvalid)
#else
    :m_type(eX)
    ,m_number(eNumX)
#endif
{
}

CCard::CCard(int cardIdx)
#if CARD_1_BYTE
    :m_num(cardIdx)
#else
    :m_type(eX)
    ,m_number(eNumX)
#endif
{
   //SetIdx(cardIdx);
}

CCard::CCard(eType type, eNumber num)
#if CARD_1_BYTE
{
    m_num = type* c_cardNumberMax + num;
}
#else
    :m_type(type)
    ,m_number(num)
{
}
#endif


bool CCard::operator==(const CCard& other ) const
{
#if CARD_1_BYTE
    const bool ifok = m_num == other.m_num;
#else
    const bool ifok =m_type == other.m_type
        && m_number == other.m_number;
#endif
    return ifok;
}

bool CCard::IsLegal() const
{
#if CARD_1_BYTE
    if(m_num < c_cardAll
#else
    if(
		//m_type >= eHeart
        m_type < eCardMax
        //&& m_number >= eA
        && m_number < eNumberMax
#endif
        )
        return true;
    return false;

}

void  CCard::Disable()
{
#if CARD_1_BYTE
    m_num = (UINT8)-1;
#else
    m_type = (eX);
    m_number = (eNumX);
#endif
}

bool    CCard::SetIdx(int cardIdx)
{
    if(cardIdx < 0
        || cardIdx>= c_cardAll)
        return false;

#if CARD_1_BYTE
    m_num = cardIdx;
#else
    m_number = (eNumber)(cardIdx % c_cardNumberMax);
    m_type =  (eType)(cardIdx /c_cardNumberMax);
#endif
    return true;
}

byte CCard::GetIdx() const
{    
    if(!IsLegal() )
        return -1;
#if CARD_1_BYTE
    return m_num;
#else
    byte retbyte = m_type * c_cardNumberMax + m_number;
    return retbyte;
#endif
}

bool  CCard::SetTypeNumber(eType ty, eNumber num)
{
    if(num >= c_cardNumberMax
        || ty >= eCardMax)
        return false;

#if CARD_1_BYTE
    m_num = ty * c_cardNumberMax + num;
#else
    m_type = ty;
    m_number = num;
#endif
    return true;
}

eType    CCard::GetType() const
{
#if CARD_1_BYTE
    const eType type = (eType)(m_num/ c_cardNumberMax);
    return type;
#else
    return m_type;
#endif
}

eNumber  CCard::GetNumber() const
{
#if CARD_1_BYTE
    const eNumber num =  (eNumber)(m_num % c_cardNumberMax);
    return num;
#else
    return m_number;
#endif
}

bool    CCard::FRed() const
{
    if(eHeart == GetType() 
        || eDiamond == GetType() )
        return true;
    return false;
}
//////////////////////////////////////////////////////////////////////////

//bool    CCard::FRed(int cardIdx)
//{
//    if(cardIdx < 0)
//        return false;
//    eType curtype = GetType(cardIdx);
//
//    if(eHeart == curtype 
//        || eDiamond == curtype )
//        return true;
//    return false;
//}

//eType  CCard::GetType(int cardIdx)
//{
//    if(cardIdx < 0)
//        return eCardMax;
//    eType curtype = (eType)(cardIdx /c_cardNumberMax);
//    return curtype;
//}

//eNumber  CCard::GetNumber(int cardIdx)
//{
//    if(cardIdx <0
//        || cardIdx >= c_cardAll)
//        return eNumberMax;
//
//    const eNumber inum = (eNumber)(cardIdx % c_cardNumberMax);
//
//    return inum;
//}

int  CCard::GetIdx(eType type, eNumber cardNumber)
{
    if(type< 0 
        || type>= eCardMax
        ||cardNumber< 0
        || cardNumber >= c_cardNumberMax)
        return  -1;
    int cardIdx = type*c_cardNumberMax + cardNumber;
    return cardIdx;
}

//must big attach small, K attach Q
bool    CCard::CanAttachInCol(const CCard& other) const
{
    if(FRed() != other.FRed()
        && (GetNumber() - other.GetNumber() ) == 1
        )
        return true;
    return false;
}

bool    CCard::CanAttachSorted(const CCard& other) const
{
    if(GetType() == other.GetType()
        && (GetNumber() - other.GetNumber() ) == 1
        )
        return true;
    return false;
}

string CCard::GetString() const
{
    if(!IsLegal() )
        return "_";

    string strRet;
    string strColor;
    const eType ty = GetType();
    switch(ty){
    case  eClub:    
        strColor = "C";
        break;
    case eDiamond:    
        strColor = "D";
        break;
    case eHeart:
        strColor = "H";
        break;
    case eSpade:
        strColor = "S";
        break;
    }

    const eNumber num =GetNumber();
    char bufNum[4];
    sprintf_s(bufNum, 4, "%d", num+1);
    strRet = strColor + bufNum;
    return strRet;

//    char buf[4];
//#if CARD_1_BYTE
//    sprintf_s(buf, 4, "%2d", m_num);
//#else
//    sprintf_s(buf, 4, "%d%d", m_type, m_number);
//#endif
    //return buf;
}