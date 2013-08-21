#if defined WIN32 || defined WIN64

#ifndef CNET_H
#define CNET_H

#include "INet.h"
#include "CStream.h"
#include "CLock.h"
#include "Pool.h"
#include "SockData.h"
#include "CQueue.h"
#include "../liblog/LogSystem.h"

#define NET_TRACE(format, ...) {\
    char _log[4096] = {0}; \
    SafeSprintf(_log, 4096, format, ##__VA_ARGS__); \
    char __log[4096] = {0}; \
    SafeSprintf(__log, sizeof(__log), "[info] %s|%d|%s | %s\n", __FILE__, __LINE__, __FUNCTION__, _log); \
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN); \
    printf(__log); \
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_INTENSITY); \
    }

#define NET_WARN(format, ...) {\
    char _log[4096] = {0}; \
    SafeSprintf(_log, 4096, format, ##__VA_ARGS__); \
    char __log[4096] = {0}; \
    SafeSprintf(__log, sizeof(__log), "[info] %s|%d|%s | %s\n", __FILE__, __LINE__, __FUNCTION__, _log); \
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN); \
    printf(__log); \
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_INTENSITY); \
    }

#define NET_ERROR(format, ...) {\
    char _log[4096] = {0}; \
    SafeSprintf(_log, 4096, format, ##__VA_ARGS__); \
    char __log[4096] = {0}; \
    SafeSprintf(__log, sizeof(__log), "[info] %s|%d|%s | %s\n", __FILE__, __LINE__, __FUNCTION__, _log); \
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED); \
    printf(__log); \
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_INTENSITY); \
    }

class CNet : public INet {
public:
    virtual bool CListen(const char * ip, const u16 port, const u16 count, const void * buff = NULL, const u32 size = 0);
    virtual bool CConnectEx(const char * ip, const u16 port, const void * buff, const u32 size);
    virtual bool CSendMsg(const u16 conid, const void * buff, const u16 size);
    virtual bool CClose(const u16 conid);
    virtual void CStartLoop(bool demon);

    virtual bool CGetRemoteInfo(const u16 conid, const char * & ip, u16 & port);

    virtual void CSetNetThreadCount(const u8 count);
    virtual bool CSetConnectedCall(const CALL_FUN pcntedfun);
    virtual bool CSetConnectFailedCall(const CALL_FUN pcntfailedfun);
    virtual bool CSetConnectionBreakCall(const CALL_FUN pbreakfun);
    virtual bool CSetNewConCall(const CALL_FUN pnewconcall);
    virtual bool CSetRecvCall(const CALL_FUN precvfun);

    CNet(const u8 threadcount, const u16 linkcount, const u16 iowaittime);
    ~CNet();
private:
    static THREAD_FUN CLoop(LPVOID p);
    void CAccept(SockData * pSockData, SockPlus * pSockPlus, const u32 nSize, u32 nErrorCode);
    void CRecv(SockData * pSockData, SockPlus * pSockPlus, const u32 nSize, u32 nErrorCode);
    void CSend(SockData * pSockData, SockPlus * pSockPlus, const u32 nSize, u32 nErrorCode);
    void CConnect(SockData * pSockData, SockPlus * pSockPlus, const u32 nSize, u32 nErrorCode);
    bool CClose(SockData * pSockData);

private:
    static THREAD_FUN CEventLoop(LPVOID p);

private:
    LPFN_ACCEPTEX m_pFunAcceptEx;
    LPFN_CONNECTEX m_pFunConnectEx;
    CALL_FUN m_pRecvFun;
    CALL_FUN m_pConnectedFun;
    CALL_FUN m_pCntFailedFun;
    CALL_FUN m_pCntBreakFun;
    CALL_FUN m_pNewConFun;
    u8 m_nNetThreadCount;
    POOL<SockData, true, 10240> m_SockDataPool;
    HANDLE m_hCompletionPort;
    BOOL m_endIocp;
    u16 m_nIOWaitTime;
	CQueue m_CQueue;
};

#endif
#endif //#if defined WIN32 || defined WIN64
