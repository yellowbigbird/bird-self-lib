#pragma once

#include "card.h"

class CCalculate
{
public:
    CCalculate();

    void InitData();
    void Run();

public:
    typedef std::vector<int>    VecIdx;

    std::vector<VecIdx>         m_vecVecIdx;
    std::vector<VecIdx>         m_vecVecIdxSorted;   //4 vec
    VecIdx                      m_vecBench;       //4 card  

    VecCard             m_vecCardAll;   //4*13
};