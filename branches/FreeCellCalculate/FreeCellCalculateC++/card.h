#pragma once

#include <vector>

enum eType
{
    eHeart = 0,
    eClub,
    eDiamond,
    eSpade,

    eMax,
    eInvalid,
};

//ϴ�� shuffle
//���� heart
//÷�� club
//���� diomand
//���� spade

class CCard
{
public:
    CCard()
        :m_type(eInvalid)
        ,m_number(0)
    {
    }

    eType    m_type;
    UINT     m_number;  //0-8, 9 J, 10 Q, 11 K
};

typedef std::vector<CCard>   VecCard;

