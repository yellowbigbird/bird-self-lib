#pragma once

#include <vector>

namespace Card
{

    const int c_size = 4;
    const int c_colSize = 8;
    const int c_cardNumberMax = 13;     //0-11
    const int c_cardAll = c_cardNumberMax* c_size;
    //-1 means no card

    enum eType
    {
        eHeart = 0,     //���� 0
        eDiamond,       //���� 1
        eClub,          //÷�� 2
        eSpade,         //���� 3

        eCardMax,
        eInvalid=100,
    };

    enum eNumber
    {
        e1 = 0,
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
        eNumberInvalid = 100,
    };

    //ϴ�� shuffle

    class CCard
    {
    public:
        explicit CCard();
        explicit CCard(int cardIdx);
        explicit CCard(eType type, eNumber num);

        bool operator==(const CCard& other ) const;

        bool        SetIdx(int cardIdx);
        void        Disalbe();
        bool        IsLegal() const;  //legal leagl

        //static bool FRed(int cardIdx);

        //static eType    GetType(int cardIdx);
        //static eNumber  GetNumber(int cardIdx);
        static int  GetIdx(eType type, eNumber cardNumber);

        bool        FRed() const;
        bool        CanAttach(const CCard& other) const;
        eType       GetType() const;
        eNumber     GetNumber() const;

    protected:
        //eType       m_type;
        //eNumber     m_number;  //0-9(1-10), 11- J, 11- Q, 12- K
        UINT8       m_numberType;
    };

    typedef std::vector<CCard>   VecCard;
    typedef VecCard::iterator   VecCardIt;

}; //namespace card
