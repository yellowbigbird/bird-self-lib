#include "stdafx.h"
#include "card.h"

using namespace std;
using namespace Card;

const UINT8 c_carInvalid = 0xff;

CCard::CCard()
    :m_numberType(c_carInvalid)
    //:m_type(eInvalid)
    //,m_number(eNumberInvalid)
{
}

CCard::CCard(int cardIdx)
    :m_numberType(cardIdx)
    //:m_type(eInvalid)
    //,m_number(eNumberInvalid)
{
   //SetIdx(cardIdx);
}

CCard::CCard(eType type, eNumber num)
    //:m_type(type)
    //,m_number(num)
{
    m_numberType = type* c_cardNumberMax + num;
}

bool CCard::operator==(const CCard& other ) const
{
    const bool ifok = m_numberType == other.m_numberType
        //m_type == other.m_type
        //&& m_number == other.m_number
        ;
    return ifok;
}

bool CCard::IsLegal() const
{
    if(m_numberType< c_cardNumberMax
        //m_type >= eHeart
        //&& m_type < eCardMax
        //&& m_number >= e1
        //&& m_number < eNumberMax
        )
        return true;
    return false;

}

void  CCard::Disalbe()
{
    //m_type = (eInvalid);
    //m_number = (eNumberInvalid);
}

bool    CCard::SetIdx(int cardIdx)
{
    if(cardIdx < 0
        || cardIdx>= c_cardAll)
        return false;

    //m_number = (eNumber)(cardIdx % c_cardNumberMax);
    //m_type =  (eType)(cardIdx /c_cardNumberMax);
    m_numberType = cardIdx;
    return true;
}

eType    CCard::GetType() const
{
    //return m_type;
    const eType type = (eType)(m_numberType/ c_cardNumberMax);
    return type;
}
eNumber  CCard::GetNumber() const
{
    //return m_number;
    const eNumber num =  (eNumber)(m_numberType % c_cardNumberMax);
    return num;
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
bool    CCard::CanAttach(const CCard& other) const
{
    if(FRed() != other.FRed()
        && (GetNumber() - other.GetNumber() ) == 1
        )
        return true;
    return false;
}