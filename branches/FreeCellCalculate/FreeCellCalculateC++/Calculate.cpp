#include "stdafx.h"
#include "Calculate.h"
#include <algorithm>

using namespace std;
using namespace Card;

const UINT c_maxVecState = 200000;
//////////////////////////////////////////////////////////////////////////

CCalculate::CCalculate()
{

}

void CCalculate::Run()
{
    //m_vecStateAll.reserve(c_maxVecState);
    //m_vecIdxOpen.reserve(c_maxVecState);
    //m_vecIdxClose.reserve(c_maxVecState);
    m_mapDeadId.clear();
    m_stateStart.GenerateCards(3);

    //m_stateStart.CheckInputDataLegal();
    m_stateStart.m_id = 0;
    m_stateStart.Update();
    m_nextGenId = 0;
    AddToAll(m_stateStart);

    //SolutionAstar();
    SolutionDeep1st();

    int i = 0;
    i++;

}

//void CCalculate::SolutionAstar()
//{
//    int lowStateIdx = -1;
//    int sonStateIdx = 0;
//    UINT idxInOpen = 0;
//    UINT idxWin = 0;
//    bool fFindLow = false;
//    UINT loopCount = 0;
//    double      valueOpen = 0;    //save lowest value of open
//
//    //push start in open
//    m_vecIdxOpen.push_back(0);
//    valueOpen = m_stateStart.GetValue();
//
//    while(m_vecIdxOpen.size() )
//    {
//        loopCount++;
//
//        //从OPEN表中取估价值f最小的节点n;
//        //fFindLow = FindLowestValueState(lowStateIdx, idxInOpen);
//        //if(!fFindLow)
//        //break;
//        lowStateIdx = *m_vecIdxOpen.begin();
//        CState& stN = m_vecStateAll[lowStateIdx];
//        TRACE("step=%d, value=%d", stN.m_step, stN.m_value);
//
//        if(stN.FWin() ){
//            idxWin = stN.m_id;
//            TRACE("win.\n");
//            break;
//        }
//
//        GenerateSonState(stN);
//
//        //foreach cur state every son
//        for(ListInt::iterator intIt = stN.m_idxSon.begin();
//            intIt != stN.m_idxSon.end() ;
//            intIt++)
//        {
//            sonStateIdx = *intIt;
//            CState& stX = m_vecStateAll[sonStateIdx];
//            double valueSon = stX.GetValue();
//
//            //if X in Open 
//            VecInt::iterator itResult = find( m_vecIdxOpen.begin(), m_vecIdxOpen.end(), sonStateIdx ); 
//            if(itResult != m_vecIdxOpen.end() ){  //todo
//                if(stX.GetValue() < valueOpen)
//                {
//                    stX.m_idxFather = stN.m_id; //set N is X's father
//                    //stN.m_idxSon ; //todo
//                    valueOpen = stX.GetValue();
//                }
//            }
//            else {
//                //if x in close
//                itResult = find( m_vecIdxClose.begin(), m_vecIdxClose.end(), sonStateIdx );
//                if(itResult != m_vecIdxClose.end() ){
//                    continue;
//                }
//                else{ //X not in both
//                    //m_vecIdxOpen.push_back(sonStateIdx);
//                    SortInsert(m_vecIdxOpen, sonStateIdx);
//                }
//            }
//        }  //for
//
//        //erase N in open, put into Close
//        m_vecIdxClose.push_back(stN.m_id);
//        EraseStateFromOpen(idxInOpen);
//
//        //按照估价值将OPEN表中的节点排序; //实际上是比较OPEN表内节点f的大小，从最小路径的节点向下进行。
//        //SortOpen();
//    }//while
//}

void CCalculate::SolutionDeep1st()
{
    UINT loopCount = 0;
    UINT sonSum = 0;
    int curStateIdx = 0;
    UINT sonIdx = 0;

    bool game_done = false;
    bool fFindSon = true;
    
    while (!game_done) 
    {        
        //if(curStateIdx >= m_vecStateAll.size())
        //    break;

        //TRACE("\n");
        loopCount++;

        //get current state
        MapIdState::iterator it = m_vecStateAll.find(curStateIdx);
        if(it == m_vecStateAll.end() ){
            TRACE("\nfail.\n");
            ASSERT("invalid id" && false);
            break;
        }

        CState& curSt =  it->second;
        //TRACE("loopcnt=%d, step=%d, value=%d", loopCount, curSt.m_step, curSt.m_value);
        //printf("DEEP: %d  STEP: %d\n", s->level, s->id);
        game_done = curSt.FWin();
        if(game_done)
            break;

        //get son sums
        if(curSt.m_hasGeneratedSon){
            sonSum = (UINT)curSt.m_idxSon.size();
        }
        else{
            sonSum = curSt.GenerateSonState(this);
        }

        fFindSon = false;
        sonIdx = curStateIdx;
        if (sonSum) {
            // try to test the first sub step , lowest value
            while(curSt.m_idxSon.size() 
                && !fFindSon)
            {
                ListInt::const_iterator intIt = curSt.m_idxSon.begin();

                sonIdx = *intIt;                
                curSt.m_idxSon.erase(intIt);

                if(FindStInDead(sonIdx) )
                    continue;  //for

                fFindSon = true;

                curStateIdx = sonIdx;
                break;
            } //while

            //have deleted all son
            if(
                /*curSt.m_hasGeneratedSon
                &&*/
                curSt.m_idxSon.size() < 1)
            {
                //clear all
                curSt.m_vecIdxSorted.clear();   //4 vec, save show card
                curSt.m_vecBench.clear();       //4 card 
                curSt.m_vecVecIdx.clear();
                curSt.m_dead = true; //todo
            }
        } //if sum
              
        if(!fFindSon)
        {
            //no son , backward, find father.
            curStateIdx = curSt.m_idxFather;       

            //erase this state
            m_mapDeadId.insert(curSt.m_id);
            m_vecStateAll.erase(it);
        }
    }

    int i =0;
    i++;
    TRACE("win = %d", game_done);
}

bool CCalculate::FindLowestValueState(int& lowidx, UINT& idxInOpen) const
{
    int stateIdx = 0;
    //int lowidx = -1;
    double valueLow = 10000, value = 0;
    MapIdState::const_iterator itMap;

    UINT idx=0;
    for(ListInt::const_iterator it = m_vecIdxOpen.begin();
        it != m_vecIdxOpen.end();
        idx++, it++)
    {
        stateIdx = *it;
        if(stateIdx <0 || stateIdx >= m_vecStateAll.size() )
            continue;

        //const CState& entry = m_vecStateAll[stateIdx];
        itMap = m_vecStateAll.find(stateIdx);
        if(itMap == m_vecStateAll.end() )
            return false;
        const CState& entry = itMap->second;

        value = entry.GetValue();
        if(value < valueLow){
            valueLow = value;
            lowidx = stateIdx;
            idxInOpen = idx;
        }
    }
    return (lowidx >= 0);
}

//return son's amount
//UINT CCalculate::GenerateSonState(CState& stFather) 
//{
//    ListState vecState;
//    stFather.GenerateSonState(vecState);
//
//    UINT idFind = 0;
//    UINT sonAmount = 0;
//
//    if(vecState.size() < 1)
//        stFather.m_idxSon.clear();
//
//    for(ListState::iterator it = vecState.begin();
//        it != vecState.end();
//        it++)
//    {
//        CState& entry = *it;
//
//        //check if this state exist
//
//        if( FindStInAll(entry, idFind) )
//        {
//            //check step
//            CState& stOther = m_vecStateAll[idFind];			 
//            if(entry.m_step > stOther.m_step)
//                continue;
//            SortInsert(stFather.m_idxSon, idFind);
//            stFather.SetIdxFather(stOther);  //switch to short one's son
//            sonAmount++;
//        }
//        else{
//            AddToAll(entry);
//            //CState& entryPushed = *m_vecStateAll.rbegin();
//            //entryPushed.m_id = (UINT)m_vecStateAll.size();
//            //stFather.m_idxSon.push_back(entryPushed.m_id);
//            SortInsert(stFather.m_idxSon, entry.m_id);
//            sonAmount++;
//        }
//    }
//
//    //sort the son
//    //sort(stFather.m_idxSon.begin(), stFather.m_idxSon.end(), );
//    return sonAmount;
//}

//true, is need this sta
//false , drop this one
bool  CCalculate::CheckAndInsertState(CState& stInsert, CState& stFather)
{
    UINT idFind = 0;
    if( FindStInAll(stInsert, idFind) )
    {
        //id in dead
        if(FindStInDead(idFind) )
            return false;

        //check step
        CState& stOther = m_vecStateAll[idFind];			 
        if(stInsert.m_step > stOther.m_step)
            return false;
        SortInsert(stFather.m_idxSon, idFind);
        stFather.SetIdxFather(stOther);  //switch to short one's son
    }
    else{
        AddToAll(stInsert);
        SortInsert(stFather.m_idxSon, stInsert.m_id);
        
    }
    return true;
}

bool CCalculate::FindStInAll(const CState& st, UINT& id) const
{
    //for(UINT idx=0; idx< m_vecStateAll.size(); idx++)
    //{
    //    if(st == m_vecStateAll[idx]){
    //        id = idx;
    //        return true;
    //    }
    //}
    const string& str = st.m_str;
    MapStrId::const_iterator it = m_mapStateId.find(str);
    if(it == m_mapStateId.end() )
        return false;

    return true;
}

bool  CCalculate::FindStInDead(UINT id) const
{
    SetId::const_iterator it = m_mapDeadId.find(id);
    if(it == m_mapDeadId.end() )
        return false;

    return true;
}


bool CCalculate::AddToAll(CState& st)
{
    //UINT idFind = 0;
    //if(FindStInAll(st, idFind) )  //don't check here
    //    return false;     

    UINT id = m_nextGenId;
    st.m_id = id;
    m_nextGenId++;

    //m_vecStateAll.push_back(st);
    m_vecStateAll[id] = st;

    //m_vecStateAll.rbegin()->m_id = (UINT)id;

    //update map
    if(st.m_str.empty() ){
        ASSERT(false);
    }
    else{
        m_mapStateId[st.m_str] = id;
    }
    return true;
}

//void CCalculate::EraseStateFromOpen(UINT idxInOpen)
//{     
//    VecInt::iterator it = m_vecIdxOpen.begin()+ idxInOpen;
//    m_vecIdxOpen.erase(it);

//    //int idx=0;
//    //for(VecInt::iterator it = m_vecIdxOpen.begin();
//    //    it != m_vecIdxOpen.end();
//    //    it++, idx++)
//    //{
//    //    if(idx == idxInOpen){
//    //        m_vecIdxOpen.erase(it);
//    //        return;
//    //    }
//    //}
//}

//void CCalculate::SortOpen() //todo
//{
//    //sort(m_vecIdxOpen.begin(), m_vecIdxClose.end() );  //idx , can't use this sort
//}

bool  CCalculate::SortInsert(ListInt& vecToInsert, int stateIdx) const
{
    //const CState& staIsert = m_vecStateAll[stateIdx];
    MapIdState::const_iterator itMap = m_vecStateAll.find(stateIdx);
    if(itMap == m_vecStateAll.end() )
        return false;
    const CState& staIsert = itMap->second;

    const double valueInsert = staIsert.GetValue();
    double value = 0;

    ListInt::iterator it;
    int idx=0;
    for(it = vecToInsert.begin(); 
        it != vecToInsert.end();
        it++)
    {
        idx = *it;
        //const CState& staComp = m_vecStateAll[idx];
        itMap = m_vecStateAll.find(idx);
        if(itMap == m_vecStateAll.end() )
            continue;
        const CState& staComp = itMap->second;

        value = staComp.GetValue();
        if(value > valueInsert)
        {
            vecToInsert.insert(it, stateIdx);
            return true;
        }
    }
    vecToInsert.push_back(stateIdx);
    return true;
}
