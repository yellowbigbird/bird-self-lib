
#include "stdafx.h"

#include "UtilString.h"

#include <tchar.h>
#include <windows.h>

using namespace std;

//////////////////////////////////////////////////////////////////////////
namespace UtilString{

    //must free LPWSTR after call this func
    std::wstring ConvertMultiByteToWideChar(const std::string& strSrc)
    {
        const char* pStr = strSrc.c_str();
        //size_t nLenOld = strSrc.length();
        const int nLen = (int)strSrc.length();

        wstring retStr;
        TCHAR* pwstr = NULL;
        int len=0;

        len=MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,pStr,nLen+1,pwstr,0);
        if(len==0)
            return retStr;

        pwstr=(LPWSTR)malloc(len*sizeof(WCHAR));
        len=MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,pStr,nLen+1,pwstr,len);
        if(len==0)
        {
            free(pwstr);
            return retStr;
        }
        retStr = pwstr;
        free(pwstr);

        return retStr;
    }

    std::string ConvertWideCharToMultiByte(const std::wstring& wstrSrc)                                       
    {
        const TCHAR* pWStr = wstrSrc.c_str();
        const int nLen = (int)wstrSrc.length();
        string ret;
        LPSTR pStr=0;
        int len=WideCharToMultiByte(CP_ACP,0,pWStr,nLen+1,0,0,0,0);
        if(!len)
            return ret;		

        pStr=(LPSTR)malloc(len);

        len=WideCharToMultiByte(CP_ACP,0,pWStr,nLen+1,pStr,len,0,0);
        if(!len)
        {
            free(pStr);
            return ret;		
        }
        ret = pStr;
        free(pStr);

        return ret;
    }

    wstring ConvertMultiByteToWideCharUTF8(const std::string& strSrc)
    {
        const char* pStr = strSrc.c_str();
        const int nLen = (int)strSrc.length();
        LPWSTR pwstr=NULL;
        wstring wstrRet;
        int len=0;

        len=MultiByteToWideChar(CP_UTF8,0,pStr,nLen+1,pwstr,0);
        if(len==0)
            return 0;

        pwstr=(LPWSTR)malloc(len*sizeof(WCHAR));
        len=MultiByteToWideChar(CP_UTF8,0,pStr,nLen+1,pwstr,len);
        if(len==0)
        {
            free(pwstr);
            return wstrRet;
        }
        pwstr[len-1] = 0;//Added by Kevin, 2008/08/19, in order to set pwstr to null terminated string
        wstrRet = pwstr;

        return wstrRet;
    }


    //bird 20080816
    //must free LPWSTR after call this func
    string ConvertWideCharToMultiByteUTF8(const std::wstring& wstrSrc)
    {
        const TCHAR* pWStr = wstrSrc.c_str();
        const int nLen = (int)wstrSrc.length();
        LPSTR pStr=0;
        string strRet;
        int len=0;

        //CP_ACP
        len=WideCharToMultiByte(CP_UTF8,0,pWStr,nLen+1,0,0,0,0);
        if(!len) //????
            return 0;		

        pStr=(LPSTR)malloc(len);

        //CP_ACP
        len=WideCharToMultiByte(CP_UTF8,0,pWStr,nLen+1,pStr,len,0,0);
        if(!len)
        {
            free(pStr);
            return strRet;		
        }
        strRet = pStr;
        return strRet;
    }


    bool  ConvertHexStringToValue(const string& str, int& value) 
    {
        const int strlen0 = (int)str.length();
        char cc = 0;
        bool ifErr = false;
        value = 0;
        int curValue = 0;
        //for(int idx= strlen0-1, pos = 0; idx>-1; idx--, pos++)
        for(int idx = 0; idx< strlen0; idx++)
        {
            cc = str[idx];
            if(cc >= '0' && cc <= '9')
            {
                curValue = cc - '0';
            }
            else if(cc >= 'a' && cc <= 'f')
            {
                curValue = cc - 'a'+ 10;
            }
            else if(cc >= 'A' && cc <= 'F')
            {
                curValue = cc - 'A' + 10;
            }
            else{
                ifErr = true;
                continue;
            }
            value = value* 16 + curValue;
        }
        if(ifErr)
            return false;

        return true;
    }

}; //namespace
