#include "stdafx.h"
#include "Step.h"

using namespace std;

CStep::CStep()
    : m_colIdxSrc(eCol1)
    , m_colIdxDest(eCol1)
    , m_cardMoveAmount(0)
{
}

std::string CStep::GetColIdxStr(eType  colIdx)
{
    string str;

    if(colIdx >= eCol1
        && colIdx <= eCol8){
            str = "Col ";
            str += '0' + colIdx - eCol1;
    }
    else if(colIdx >= eBench1
        && colIdx <= eBench4){
        str = "Bench ";
        str += '0' + colIdx - eBench1; 
    }
    else{
        str = "sorted";
    }
    return str;
}