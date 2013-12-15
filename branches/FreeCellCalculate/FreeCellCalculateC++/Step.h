#pragma once

class CStep
{
public:
    CStep();

    //column 8-11 means "bench"
    UINT8   m_colIdxSrc;
    UINT8   m_colIdxDest;
    UINT8   m_cardMoveAmount;
};
