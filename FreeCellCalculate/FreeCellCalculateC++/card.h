#pragma once

#include <vector>
#include <list>

#define CARD_1_BYTE  1

namespace Card
{
    const int c_size = 4;
    const int c_colSize = 8;
    const int c_cardNumberMax = 13;     //0-11
    const int c_cardAll = c_cardNumberMax* c_size;
    //-1 means no card

    enum eType
    {
        eClub = 0,     //÷�� 0
        eDiamond,       //���� 1
        eHeart,          // 2  ����
        eSpade,         //���� 3

        eCardMax,
        eX=100,
    };

    enum eNumber
    {
        eA = 0,
        e2,
        e3,
        e4,
        e5,
        e6,
        e7,
        e8,
        e9,
        e10,    //9
        eJ,     //10    
        eQ,     //11
        eK,     //12

        eNumberMax,
        eNumX = 100,
    };

    //ϴ�� shuffle

    class CCard
    {
    public:
        explicit CCard();
        explicit CCard(int cardIdx);
        explicit CCard(eType type, eNumber num);

        bool operator==(const CCard& other ) const;

        eType       GetType() const;
        eNumber     GetNumber() const;

        bool        SetTypeNumber(eType ty, eNumber num);

        byte        GetIdx() const;
        bool        SetIdx(int cardIdx);

        void        Disable();
        bool        IsLegal() const;  //legal leagl

        //static bool FRed(int cardIdx);

        //static eType    GetType(int cardIdx);
        //static eNumber  GetNumber(int cardIdx);
        static int  GetIdx(eType type, eNumber cardNumber);

        bool        FRed() const;
        bool        CanAttachInCol(const CCard& other) const;
        bool        CanAttachSorted(const CCard& other) const;
        

        std::string GetString() const;

    protected:
        
#if CARD_1_BYTE
        UINT8       m_num;
#else
        eType       m_type;
        eNumber     m_number;  //0-9(1-10), 11- J, 11- Q, 12- K
#endif
    };

    typedef std::vector<CCard>   VecCard;
    typedef VecCard::iterator   VecCardIt;
    typedef std::list<CCard>    ListCard;
    typedef ListCard::const_iterator ListCardConstIt;
    typedef ListCard::iterator  ListCardIt;

}; //namespace card
