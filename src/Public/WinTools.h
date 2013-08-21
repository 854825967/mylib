#if defined WIN32 || defined WIN64

#ifndef WINTOOLS_H
#define WINTOOLS_H
#include "MultiSys.h"
#include <string>
using namespace std;

inline ThreadID GetCurrentThreadID() {
    return ::GetCurrentThreadId();
}

inline u64 GetCurrentTimeTick() {
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    struct tm tTm;
    tTm.tm_year     = wtm.wYear - 1900;
    tTm.tm_mon      = wtm.wMonth - 1;
    tTm.tm_mday     = wtm.wDay;
    tTm.tm_hour     = wtm.wHour;
    tTm.tm_min      = wtm.wMinute;
    tTm.tm_sec      = wtm.wSecond;
    tTm.tm_isdst    = -1;
    return (u64)mktime(&tTm) * 1000 + (u64)wtm.wMilliseconds;
}

inline string GetCurrentTimeString() {
    char strtime[64] = {0};
    SYSTEMTIME time;
    GetLocalTime(&time);
    SafeSprintf(strtime, sizeof(strtime), "%4d-%d-%d %d:%d:%d", 
        time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
    return strtime;
}

inline string GetTimeString(const u64 ntick) {
    struct tm _tm;
    time_t lt;
    char strtime[64];
    lt = ntick/1000;
    errno_t nerror = localtime_s(&_tm, &lt); //UTC
    printf("%d\n", nerror);
    SafeSprintf(strtime, sizeof(strtime), "%4d-%d-%d %d:%d:%d", 
        _tm.tm_year + 1900, _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec);
    return strtime;
}

inline bool GetAcceptExFun(LPFN_ACCEPTEX & acceptfun) {
    GUID GuidAcceptEx = WSAID_ACCEPTEX;
    DWORD dwBytes = 0;
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx, 
        sizeof(GuidAcceptEx), &acceptfun, sizeof(acceptfun), 
        &dwBytes, NULL, NULL);

    if (NULL == acceptfun) {
        ECHO_ERROR("Get AcceptEx fun error, error code : %d", ::WSAGetLastError());
        return false;
    }

    return true;
}

inline bool GetConnectExFun(LPFN_CONNECTEX & connectfun) {
    GUID GuidConnectEx = WSAID_CONNECTEX;
    DWORD dwBytes = 0;
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidConnectEx, 
        sizeof(GuidConnectEx), &connectfun, sizeof(connectfun), 
        &dwBytes, NULL, NULL);

    if (NULL == connectfun) {
        ECHO_ERROR("Get ConnectEx fun error, error code : %d", ::WSAGetLastError());
        return false;
    }

    return true;
}

inline const char * GetAppPath()
{
    static char * pStr = NULL;

    if (NULL == pStr)
    {
        pStr = NEW char[256];
        GetModuleFileName(NULL, pStr, 256);
        PathRemoveFileSpec(pStr);
    }

    return pStr;
}

inline void CopyString(char * buf, size_t len, const char * str)
{
    const size_t SIZE1 = ::strlen(str) + 1;

    if (SIZE1 <= len)
    {
        ::memcpy(buf, str, SIZE1);
    }
    else
    {
        ::memcpy(buf, str, len - 1);
        buf[len - 1] = 0;
    }
}

inline u32 StringAsInt(const char * value) {
    ASSERT(value);
    return atoi(value);
}

inline string IntAsString(const u32 value) {
    char str[32];
    memset(str, 0, sizeof(str));
    SafeSprintf(str, sizeof(str), "%d", value);
    return string(str);
}

inline string StringReplace(const char * pString, const char * pSrc, const char * pReplace) {
    if (NULL == pString || NULL == pSrc || NULL == pReplace) {
        ASSERT(false);
        return NULL;
    }

    string strSrc = pString;
    char * pChSrc = (char *)strSrc.c_str();
    int nOldLen = strlen(pChSrc);
    int nSrcLen = strlen(pSrc);
    if (0 == nSrcLen) {
        ASSERT(false);
        return NULL;
    }

    int nReplaceLen = strlen(pReplace);
    char * pStart = pChSrc;
    char * pEnd = pStart + nOldLen;
    char * pTarget = NULL;

    while (pStart < pEnd) {
        while ((pTarget = (char *)_mbsstr((const unsigned char *)pStart, (const unsigned char *)pSrc)) != NULL) {
            int nBalance = nOldLen - (pTarget - pChSrc + nSrcLen);
            memmove(pTarget + nReplaceLen, pTarget + nSrcLen, nBalance * sizeof(char));
            memcpy(pTarget, pReplace, nReplaceLen * sizeof(char));
            pStart = pTarget + nReplaceLen;
            pStart[nBalance] = '\0';
            nOldLen += (nReplaceLen - nSrcLen);
        }
        pStart += strlen(pStart) + 1;
    }

    return pChSrc;
}

#endif //WINTOOLS_H
#endif //#if defined WIN32 || defined WIN64