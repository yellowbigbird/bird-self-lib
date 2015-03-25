#pragma once
#include "card.h"

class CStep
{
public:
    CStep();

	enum eType{
		eCol1 =0,
		eCol2,
		eCol3,
		eCol4,
		eCol5,
		eCol6,
		eCol7,
		eCol8,
		eBench1,
		eBench2,
		eBench3,
		eBench4,
		eSorted,

		eMax,
	};

    std::string GetColIdxStr(eType  colIdx);

    //column 8-11 means "bench"
    UINT8   m_colIdxSrc;
    UINT8   m_colIdxDest;
    UINT8   m_cardMoveAmount;
    Card::CCard   m_card;
};
typedef std::vector<CStep>      VecStep;
typedef std::list<CStep>        ListStep;


