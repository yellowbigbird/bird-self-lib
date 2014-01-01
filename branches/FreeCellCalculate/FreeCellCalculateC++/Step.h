#pragma once

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
		eBench0,
		eBench,
		eBench,
		eBench,
		eSorted,

		eMax,
	};

    //column 8-11 means "bench"
    eType   m_colIdxSrc;
    eType   m_colIdxDest;
    UINT8   m_cardMoveAmount;
};
