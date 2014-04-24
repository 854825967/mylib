#if defined WIN32 || defined WIN64

#ifndef WINTOOLS_H
#define WINTOOLS_H
#include "MultiSys.h"
#include <string>

inline ThreadID GetCurrentThreadID() {
    return ::GetCurrentThreadId();
}

inline std::string GetHostIp(const char * pUrl){
    struct hostent *hp;
    struct in_addr in;
    hp = gethostbyname(pUrl);
    memcpy (&in.s_addr,hp->h_addr,4);
    return inet_ntoa(in);
}

inline s64 GetCurrentTimeTick() {
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    struct tm tTm;
    tTm.tm_year     = wtm.wYear - 1900;
    tTm.tm_mon      = wtm.wMonth;
    tTm.tm_mday     = wtm.wDay;
    tTm.tm_hour     = wtm.wHour;
    tTm.tm_min      = wtm.wMinute;
    tTm.tm_sec      = wtm.wSecond;
    tTm.tm_isdst    = -1;
    return (s64)mktime(&tTm) * 1000 + (s64)wtm.wMilliseconds;
}

inline s64 GetTimeTickOfDayBeginning() {
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    struct tm tTm;
    tTm.tm_year     = wtm.wYear - 1900;
    tTm.tm_mon      = wtm.wMonth;
    tTm.tm_mday     = wtm.wDay;
    tTm.tm_hour     = 0;
    tTm.tm_min      = 0;
    tTm.tm_sec      = 0;
    tTm.tm_isdst    = -1;
    return (s64)mktime(&tTm) * 1000 + (s64)wtm.wMilliseconds;
}

inline std::string GetCurrentTimeString(const char * format = "%4d-%02d-%02d %02d:%02d:%02d") {
    char strtime[64] = {0};
    SYSTEMTIME time;
    GetLocalTime(&time);
    SafeSprintf(strtime, sizeof(strtime), format,
        time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
    return strtime;
}

inline std::string GetTimeString(const s64 ntick, const char * format = "%4d-%02d-%02d %02d:%02d:%02d") {
    struct tm _tm;
    time_t lt;
    char strtime[64];
    lt = ntick/1000;
    errno_t nerror = localtime_s(&_tm, &lt); //UTC
    ASSERT(0 == nerror);
    SafeSprintf(strtime, sizeof(strtime), format,
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

inline const char * GetAppPath() {
    static char * pStr = NULL;

    if (NULL == pStr)
    {
        pStr = NEW char[256];
        GetModuleFileName(NULL, pStr, 256);
        PathRemoveFileSpec(pStr);
    }

    return pStr;
}

inline void CopyString(char * buf, size_t len, const char * str) {
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

inline std::string IntAsString(const u32 value) {
    char str[32];
    memset(str, 0, sizeof(str));
    SafeSprintf(str, sizeof(str), "%d", value);
    return str;
}

inline std::string StringReplace(const char * pString, const char * pSrc, const char * pReplace) {
    if (NULL == pString || NULL == pSrc || NULL == pReplace) {
        ASSERT(false);
        return NULL;
    }

    std::string strSrc = pString;
    char * pChSrc = (char *)strSrc.c_str();
    s32 nOldLen = strlen(pChSrc);
    s32 nSrcLen = strlen(pSrc);
    if (0 == nSrcLen) {
        ASSERT(false);
        return NULL;
    }

    s32 nReplaceLen = strlen(pReplace);
    char * pStart = pChSrc;
    char * pEnd = pStart + nOldLen;
    char * pTarget = NULL;

    while (pStart < pEnd) {
        while ((pTarget = (char *)_mbsstr((const unsigned char *)pStart, (const unsigned char *)pSrc)) != NULL) {
            s32 nBalance = nOldLen - (pTarget - pChSrc + nSrcLen);
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

inline std::string GBKToUTF8(const char * pStrGBK) {
    std::string strOutUTF8 = "";
    wchar_t * pStrWChar = NULL;
    int n = MultiByteToWideChar(CP_ACP, 0, pStrGBK, -1, NULL, 0);
    pStrWChar = new wchar_t[n];
    MultiByteToWideChar(CP_ACP, 0, pStrGBK, -1, pStrWChar, n);
    n = WideCharToMultiByte(CP_UTF8, 0, pStrWChar, -1, NULL, 0, NULL, NULL);
    char * pStrChar = new char[n];
    WideCharToMultiByte(CP_UTF8, 0, pStrWChar, -1, pStrChar, n, NULL, NULL);
    strOutUTF8 = pStrChar;
    delete[] pStrChar;
    delete[] pStrWChar;
    return strOutUTF8;
}

inline std::string UTF8ToGBK(const char * pStrUtf8) {
    int len=MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)pStrUtf8, -1, NULL,0);
    wchar_t * wszGBK = NEW wchar_t[len];
    memset(wszGBK, 0, len);
    MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)pStrUtf8, -1, (LPWSTR)wszGBK, len);

    len = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
    char *szGBK=NEW char[len + 1];
    memset(szGBK, 0, len + 1);
    WideCharToMultiByte (CP_ACP, 0, (LPWSTR)wszGBK, -1, szGBK, len, NULL,NULL);
    std::string temp(szGBK);
    delete[] szGBK;
    delete[] wszGBK;
    return temp;
}

extern "C" {
//     const char * GetIPByHost(const char * pStrHost) {
//         return NULL;
//     }
};

#endif //WINTOOLS_H
#endif //#if defined WIN32 || defined WIN64
