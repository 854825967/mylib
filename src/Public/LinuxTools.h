#ifdef linux
#ifndef LINUXTOOLS_H
#define LINUXTOOLS_H
#include "MultiSys.h"
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

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

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //LINUXTOOLS_H
#endif //#ifdef linux