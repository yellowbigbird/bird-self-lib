#pragma once

#include "card.h"
using namespace Card;

#include "State.h"

class CCalculate
{
    //typedef std::vector<int>    VecIdx;

public:
    CCalculate();
    
    void Run();
 

    CState  m_stateStart;
};

