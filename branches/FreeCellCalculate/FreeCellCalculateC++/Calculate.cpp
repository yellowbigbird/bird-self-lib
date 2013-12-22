#include "stdafx.h"
#include "Calculate.h"
#include <algorithm>

using namespace std;
using namespace Card;


//////////////////////////////////////////////////////////////////////////

CCalculate::CCalculate()
{

}

void CCalculate::Run()
{
    m_vecStateAll.reserve(100000);
    m_vecIdxOpen.reserve(100000);
    m_vecIdxClose.reserve(100000);

    //m_stateStart.InitData();
    m_stateStart.InputData();
    m_stateStart.CheckInputDataLegal();
    m_stateStart.m_id = 0;
    m_stateStart.CalculateValue();
    m_vecStateAll.push_back(m_stateStart);

    int lowStateIdx = -1;
    int sonStateIdx = 0;
    UINT idxInOpen = 0;
    UINT idxWin = 0;
    bool fFindLow = false;
    UINT loopCount = 0;

    //push start in open
    m_vecIdxOpen.push_back(0);
    m_valueOpen = m_stateStart.GetValue();
       
    while(m_vecIdxOpen.size() )
    {
        loopCount++;

        //从OPEN表中取估价值f最小的节点n;
        //fFindLow = FindLowestValueState(lowStateIdx, idxInOpen);
        //if(!fFindLow)
            //break;
        lowStateIdx = m_vecIdxOpen[0];
        CState& stN = m_vecStateAll[lowStateIdx];
        if(stN.FWin() ){
            idxWin = stN.m_id;
            break;
        }
        
        GenerateSonState(stN);

        //foreach cur state every son
        for(int i=0; i < stN.m_idxSon.size(); i++)
        {
            sonStateIdx = stN.m_idxSon[i];
            CState& stX = m_vecStateAll[sonStateIdx];
            double valueSon = stX.GetValue();

            //if X in Open 
            VecInt::iterator itResult = find( m_vecIdxOpen.begin(), m_vecIdxOpen.end(), sonStateIdx ); //查找3
            //int findPos = m_vecIdxOpen.find(sonStateIdx);
            if(itResult != m_vecIdxOpen.end() ){  //todo
                if(stX.GetValue() < m_valueOpen)
                {
                    stX.m_idxFather = stN.m_id; //set N is X's father
                    //stN.m_idxSon ; //todo
                    m_valueOpen = stX.GetValue();
                }
            }
            else {
                //if x in close
                itResult = find( m_vecIdxClose.begin(), m_vecIdxClose.end(), sonStateIdx );
                if(itResult != m_vecIdxClose.end() ){
                    continue;
                }
                else{ //X not in both
                    //m_vecIdxOpen.push_back(sonStateIdx);
                    SortInser(m_vecIdxOpen, sonStateIdx);
                }
            }
        }  //for
        
        //erase N in open, put into Close
        m_vecIdxClose.push_back(stN.m_id);
        EraseStateFromOpen(idxInOpen);
        
        //按照估价值将OPEN表中的节点排序; //实际上是比较OPEN表内节点f的大小，从最小路径的节点向下进行。
        //SortOpen();
    }//while

    //bool fWin = FWin();
    //if(fWin)
    //    return;
    //bool fCanMove = FCanMove();
    //if(!fCanMove)
    //    return;
}

 bool CCalculate::FindLowestValueState(int& lowidx, UINT& idxInOpen) const
 {
     int stateIdx = 0;
     //int lowidx = -1;
     double valueLow = 10000, value = 0;
     for(UINT idx=0; idx< m_vecIdxOpen.size(); idx++)
     {
         stateIdx = m_vecIdxOpen[idx];
         if(stateIdx <0 || stateIdx >= m_vecStateAll.size() )
             continue;
         const CState& entry = m_vecStateAll[stateIdx];
         value = entry.GetValue();
         if(value < valueLow){
             valueLow = value;
             lowidx = stateIdx;
             idxInOpen = idx;
         }
     }
     return (lowidx >= 0);
 }

 void CCalculate::GenerateSonState(CState& stFather) 
 {
     VecState vecState;
     stFather.GenerateSonState(vecState);
     UINT idFind = 0;

     if(vecState.size() < 1)
         stFather.m_idxSon.clear();

     for(int i=0; i< vecState.size(); i++)
     {
         CState& entry = vecState[i];

         //check if this state exist
         
         if( FindStInAll(entry, idFind) )
         {
             //stFather.m_idxSon.push_back(idFind);
             SortInser(stFather.m_idxSon, idFind);
         }
         else{
             AddToAll(entry);
             CState& entryPushed = *m_vecStateAll.rbegin();
             //entryPushed.m_id = (UINT)m_vecStateAll.size();
             //stFather.m_idxSon.push_back(entryPushed.m_id);
             SortInser(stFather.m_idxSon, entryPushed.m_id);
         }
     }
    
     //sort the son
     //sort(stFather.m_idxSon.begin(), stFather.m_idxSon.end(), );

 }

 bool CCalculate::FindStInAll(const CState& st, UINT& id) const
 {
     for(UINT idx=0; idx< m_vecStateAll.size(); idx++)
     {
         if(st == m_vecStateAll[idx]){
             id = idx;
             return true;
         }
     }
     return false;
 }


 bool CCalculate::AddToAll(const CState& st)
 {
     //UINT idFind = 0;
     //if(FindStInAll(st, idFind) )  //don't check here
     //    return false;     

     m_vecStateAll.push_back(st);
     m_vecStateAll.rbegin()->m_id = (UINT)m_vecStateAll.size()-1;
     return true;
 }

 void CCalculate::EraseStateFromOpen(UINT idxInOpen)
 {
     
     VecInt::iterator it = m_vecIdxOpen.begin()+ idxInOpen;
     m_vecIdxOpen.erase(it);

     //int idx=0;
     //for(VecInt::iterator it = m_vecIdxOpen.begin();
     //    it != m_vecIdxOpen.end();
     //    it++, idx++)
     //{
     //    if(idx == idxInOpen){
     //        m_vecIdxOpen.erase(it);
     //        return;
     //    }
     //}
 }

 //void CCalculate::SortOpen() //todo
 //{
 //    //sort(m_vecIdxOpen.begin(), m_vecIdxClose.end() );  //idx , can't use this sort
 //}

 void  CCalculate::SortInser(VecInt& vecToInsert, int stateIdx) const
 {
     const CState& staIsert = m_vecStateAll[stateIdx];
     const double valueInsert = staIsert.GetValue();
     double value = 0;

     VecInt::iterator it;
     int idx=0;
     for(it = vecToInsert.begin(); 
         it != vecToInsert.end();
         it++)
     {
         idx = *it;
         const CState& staComp = m_vecStateAll[idx];
         value = staComp.GetValue();
         if(value > valueInsert)
         {
             vecToInsert.insert(it, stateIdx);
             return;
         }
     }
     vecToInsert.push_back(stateIdx);
 }
