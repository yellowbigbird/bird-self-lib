#include "stdafx.h"
#include "Calculate.h"

using namespace std;

const int c_size = 4;
const int c_colSize = 8;
const int c_cardNumberMax = 12;     //0-11
//-1 means no card

CCalculate::CCalculate()
{
}

void CCalculate::InitData()
{
    m_vecCardAll.resize(c_cardNumberMax * c_size);
    for(int type =0, idx=0; type< eMax; type++ )
    {
        for(int number = 0; number< c_cardNumberMax; number++)
        {
            CCard& card = m_vecCardAll[idx];
            card.m_type = (eType)type;
            card.m_number = number;
            idx++;
        }
    }

    //clear sorted idx
    m_vecVecIdxSorted.resize(c_size);   //4
    for(int idx=0; idx< (int)m_vecVecIdxSorted.size(); idx++){
        m_vecVecIdxSorted[idx].clear();
    }

    m_vecBench.resize(c_size);  //4

    m_vecVecIdx.resize(c_colSize);  //8
    m_vecVecIdx[0];
}

void CCalculate::Run()
{

}



