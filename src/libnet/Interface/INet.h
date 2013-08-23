#ifndef INET_H
#define INET_H
#include "MultiSys.h"

class INet;

typedef INet* (*FUN_GET_NET)(const u8, const u16, const u16);

#define NAME_OF_GET_NET_FUN "GetNet"
#define GET_NET_FUN GetNet

typedef void (*FUN_RETURN_NET)(INet *);

#define NAME_OF_RETURN_NET_FUN "ReturnNet"
#define RETURN_NET_FUN ReturnNet

typedef void (*CALL_FUN)(const u16 conid, const void * buff, const u32 size);

class INet {
public:
    virtual bool CListen(const char * ip, const u16 port, const u16 count = 200)  = 0;
    virtual bool CConnectEx(const char * ip, const u16 port, const void * buff = NULL, const u32 size = 0)  = 0;
    virtual bool CClose(const u16 conid)  = 0;
    virtual bool CSendMsg(const u16 conid, const void * buff, const u16 size)  = 0;
    virtual bool CGetRemoteInfo(const u16 conid, const char * & ip, u16 & port)  = 0;

    virtual bool CSetConnectedCall(const CALL_FUN pcntedfun) = 0;
    virtual bool CSetConnectFailedCall(const CALL_FUN pcntfailedfun) = 0;
    virtual bool CSetConnectionBreakCall(const CALL_FUN pbreakfun) = 0;
    virtual bool CSetRecvCall(const CALL_FUN precvfun) = 0;
    virtual bool CSetNewConCall(const CALL_FUN precvfun) = 0;
    virtual void CStartLoop(u32 waitTime = 5, bool demon = false)  = 0;
};

inline INet * GetNetWorker(const char * dllPath, const u32 threadCount = 1, const u32 conCount = 4096, const u16 waitTime = 10) {
    INet * pNetCore = NULL;
#if defined WIN32 || defined WIN64
    HINSTANCE hinst = ::LoadLibrary(dllPath);
    if (NULL == hinst) {
        ECHO_ERROR("LoadLibrary error, code : %d", GetLastError());
        ASSERT(hinst != NULL);
        return NULL;
    }

    FUN_GET_NET pFunGetNet = (FUN_GET_NET)::GetProcAddress(hinst, NAME_OF_GET_NET_FUN);
    ASSERT(pFunGetNet != NULL);
    if (NULL == pFunGetNet) {
        return NULL;
    }

    pNetCore = pFunGetNet(threadCount, conCount, waitTime);
    ASSERT(pNetCore != NULL);
#endif // WIN32 || WIN64
    return pNetCore;
}

#endif
