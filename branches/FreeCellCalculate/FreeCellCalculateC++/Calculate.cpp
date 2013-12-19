#include "stdafx.h"
#include "Calculate.h"

using namespace std;
using namespace Card;


//////////////////////////////////////////////////////////////////////////

CCalculate::CCalculate()
{

}

void CCalculate::Run()
{
    //m_stateStart.InitData();
    m_stateStart.InputData();
    m_stateStart.CheckInputDataLegal();

    while(true){
        //m_stateStart.doMove();
        //push...
        //check if win
        //save the path
        Sleep(1);
        break;
    }
    //bool fWin = FWin();
    //if(fWin)
    //    return;
    //bool fCanMove = FCanMove();
    //if(!fCanMove)
    //    return;
}

