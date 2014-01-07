#include "stdafx.h"
#include "Calculate.h"
#include "Step.h"

#include <util/DebugFile.h>
#include <algorithm>

using namespace std;
using namespace Card;

const bool C_DEL_DEAD = true;
const bool C_USE_A_STAR = true;

//////////////////////////////////////////////////////////////////////////

CCalculate::CCalculate()
    :m_fThreadRunning(false)
    ,m_fEnd(false)
{
}

void CCalculate::StartCalc(UINT gameNum)
{
    m_gameNum = gameNum;

    DWORD dwThreadId = 0;
    HANDLE hThread = ::CreateThread(NULL, 0, CCalculate::ThreadFunc, (LPVOID*)this, 0, &dwThreadId);
    if (!hThread)
    {
        m_fThreadRunning = false;
        return ;
    }    
    m_fThreadRunning = true;
    CloseHandle(hThread);	
}

void CCalculate::Stop()
{
    m_fThreadRunning = false;
}

DWORD CCalculate::ThreadFunc(void* pServer)
{
    CCalculate* pthis = (CCalculate*)(pServer);
    if(!pthis)
        return 0;
    pthis->Run();
    //while(pthis->m_fThreadRunning)
    //{      
    //    Sleep(1);
    //}
    return 0;
}

void CCalculate::Run()
{
    Init();
    
    bool fWin = false;

    if(C_USE_A_STAR)
        fWin = SolutionAstar();
    else
        fWin = SolutionDeep1st();
        
	//output
    OutputResult();
    m_fEnd = true;
}

void  CCalculate::Init()
{
    m_vecIdxOpen.clear();
    m_vecIdxClose.clear();
    m_mapDeadId.clear();

    m_vecStrStep.clear();
    m_vecStateAll.clear();
    m_mapStateId.clear();
    m_vecStrStep.clear();

    m_stateStart.InitData();
    m_stateStart.GenerateCards(m_gameNum);

    m_stateStart.CheckDataLegal();

    m_fEnd = false;
    m_stateStart.m_id = 0;
    m_stateStart.Update();
    m_nextGenId = 0;

    AddToAll(m_stateStart);
}

bool CCalculate::SolutionAstar()
{
    //int lowStateIdx = -1;
    m_curStateIdx = 0;
    int sonStateIdx = 0;
    //UINT idxInOpen = 0;
    UINT idxWin = 0;
    bool fFindLow = false;
    bool game_done = false;
    UINT loopCount = 0;
	UINT sonSum = 0;
    double   valueOpen = 0;    //save lowest value of open

    //push start in open
    m_vecIdxOpen.push_back(0);
    valueOpen = m_stateStart.GetValue();

    while(m_vecIdxOpen.size() 
        && !game_done
        && m_fThreadRunning)
    {
        loopCount++;

        //从OPEN表中取估价值f最小的节点n;
        //fFindLow = FindLowestValueState(lowStateIdx, idxInOpen);
        //if(!fFindLow)
        //break;
        m_curStateIdx = *m_vecIdxOpen.begin();
        CState& stN = m_vecStateAll[m_curStateIdx];

        char	szMessage[1024];
        int ret = sprintf_s(szMessage, 1024, "step=%d, value=%d", stN.m_step, stN.m_value);
        //AddDebug(szMessage );

        game_done = stN.FWin();
        //if( game_done){
        //    idxWin = stN.m_id;
        //    AddDebug("win.");
        //    break;
        //}

        //get son sums
        if(stN.m_hasGenSon){
            sonSum = (UINT)stN.m_idxSon.size();
        }
        else{
            sonSum = stN.GenerateSonState(this);
        }

        //foreach cur state every son
        for(ListInt::iterator intIt = stN.m_idxSon.begin();
            intIt != stN.m_idxSon.end() ;
            intIt++)
        {
            sonStateIdx = *intIt;
            CState& stX = m_vecStateAll[sonStateIdx];
            double valueSon = stX.GetValue();

            //if X in Open 
            ListInt::iterator itResult = find( m_vecIdxOpen.begin(), m_vecIdxOpen.end(), sonStateIdx ); 
            if(itResult != m_vecIdxOpen.end() ){  //todo
                if(stX.GetValue() < valueOpen)
                {
                    stX.m_idxFather = stN.m_id; //set N is X's father
                    //stN.m_idxSon ; //todo
                    valueOpen = stX.GetValue();
                }
            }
            else {
                //if x in close
                SetId::const_iterator setIdConIt = m_vecIdxClose.find(sonStateIdx);
                if(setIdConIt != m_vecIdxClose.end() ){
                    continue;
                }
                else{ //X not in both
                    //m_vecIdxOpen.push_back(sonStateIdx);
                    SortInsert(m_vecIdxOpen, sonStateIdx);
                }
            }
        }  //for

        //erase N in open, put into Close
        m_vecIdxClose.insert(stN.m_id);
        EraseStateFromOpen(m_curStateIdx);

        //按照估价值将OPEN表中的节点排序; //实际上是比较OPEN表内节点f的大小，从最小路径的节点向下进行。
        //SortOpen();
    }//while

    m_fThreadRunning = false;
    AddDebug("win = %d", game_done);
	return true;
}

bool CCalculate::SolutionDeep1st()
{
    UINT loopCount = 0;
    UINT sonSum = 0;
    int lastStateIdx = 0;
    UINT sonIdx = 0;
	m_curStateIdx = 0;

    bool game_done = false;
    bool fFindSon = true;

    AddDebug();

    while (!game_done && m_fThreadRunning) 
    {        
        loopCount++;

        //get current state
        MapIdState::iterator it = m_vecStateAll.find(m_curStateIdx);
        if(it == m_vecStateAll.end() ){
            //TRACE("\nfail.\n");
            ASSERT("invalid id" && false);
            AddDebug("fail.");
            break;
        }

        CState& curSt =  it->second;
        //TRACE("loopcnt=%d, id=%d,step=%d, \n", loopCount, curSt.m_id, curSt.m_step ); //value=%d  curSt.m_value
        
        game_done = curSt.FWin();
        //if(game_done)
        //    break;

        //get son sums
        if(curSt.m_hasGenSon){
            sonSum = (UINT)curSt.m_idxSon.size();
        }
        else{
            sonSum = curSt.GenerateSonState(this);
        }

        fFindSon = false;
        sonIdx = m_curStateIdx;
        ListInt::const_iterator intIt;
        if (sonSum) {
            // try to test the first sub step , lowest value
            while(curSt.m_idxSon.size() 
                && !fFindSon)
            {
                intIt = curSt.m_idxSon.begin();

                sonIdx = *intIt;                
                curSt.m_idxSon.erase(intIt);

                if(FindStInDead(sonIdx) )
                    continue;  //for

                fFindSon = true;

                lastStateIdx = m_curStateIdx;
                m_curStateIdx = sonIdx;
                break;
            } //while

            ////have deleted all son
            //if( curSt.m_idxSon.size() < 1)
            //{
            //    //clear all
            //    //curSt.m_vecIdxSorted.clear();   //4 vec, save show card
            //    //curSt.m_vecBench.clear();       //4 card 
            //    //curSt.m_vecVecIdx.clear();
            //    curSt.m_dead = true; //todo
            //}
        } //if sum
              
        if(!fFindSon)
        {
            //no son , backward, find father.
            //TRACE("no son, delete id =%d,goto father=%d \n", curSt.m_id, curSt.m_idxFather);
            char	szMessage[1024];
            int ret = sprintf_s(szMessage, 1024, "no son, go back to step =%d", curSt.m_step-1);
            //AddDebug("no son, go back to step =%d", curSt.m_step-1);
            AddDebug(szMessage);

            lastStateIdx = m_curStateIdx;
            m_curStateIdx = curSt.m_idxFather; 
            if(0 == m_curStateIdx ){
                //go back to 0
                AddDebug("go back to root.\n");
            }
            else{
            }

            //erase this state
            if(C_DEL_DEAD){
                m_mapDeadId.insert(curSt.m_id);
                m_vecStateAll.erase(it);
            }
        }
    } //while 

    m_fThreadRunning = false;
    AddDebug("win = %d", game_done);
	return game_done;
}

void CCalculate::OutputResult()
{
	VecState listSt;
	UINT curStIdx = m_curStateIdx;

    //get all state
	while(true)
	{
		MapIdState::iterator it = m_vecStateAll.find(curStIdx);
		if(it == m_vecStateAll.end() ){
			TRACE("\nfail.\n");
			break;
		}
		const CState& curSt = it->second;
		listSt.insert(listSt.begin(), curSt);
		curStIdx = curSt.m_idxFather;
	}
	if(listSt.size() < 1)
		return;

    //get all steps
    VecStep vecStep;
	string str;	
    CStep step;
    bool fok = false;
    const int c_bufSize = 5;
    char buf[c_bufSize];
    string strNumber;

	for(VecState::const_iterator it = listSt.begin();
        it!= listSt.end()-1;
        it++)
	{
		const CState& stCur = *it;
        const CState& stNext = *(it+1);

        str = "step ";
        _itoa_s(stCur.m_step, buf, 10);
        strNumber = buf;
        str += strNumber;
        str += ": ";
        str +="\n";

        fok = stCur.GenerateStep(&step, str, stNext);
        ASSERT(fok);
		//TRACE(curSt.m_str.c_str() );
		//TRACE("\n" );
		//str += curSt.m_str;
		//str +="\n";
        m_vecStrStep.push_back(str);
	}
	//TRACE(str.c_str() );
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
    //const string& str = st.m_str;

    MapHashId::const_iterator it = m_mapStateId.find(st.m_hash);
    if(it == m_mapStateId.end() )
        return false;

    return true;
}

bool  CCalculate::FindStInDead(UINT id) const
{
    if(C_DEL_DEAD){
        SetId::const_iterator it = m_mapDeadId.find(id);
        if(it == m_mapDeadId.end() )
            return false;

        return true;
    }
    else{
        //default return false
        return false;
    }
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
    //if(st.m_str.empty() ){
    if(0 == st.m_hash){
        ASSERT(false);
    }
    else{
        m_mapStateId[st.m_hash] = id;  //m_str
    }
    return true;
}

void CCalculate::EraseStateFromOpen(UINT id)
{     
    ListInt::iterator it = find( m_vecIdxOpen.begin(), m_vecIdxOpen.end(), id ); 
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

bool  CCalculate::SortInsert(ListInt& vecToInsert, int stateIdx) const
{
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
