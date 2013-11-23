#ifndef INET_H
#define INET_H
#include "MultiSys.h"

typedef enum call_type {
    CALL_CONNECTED = 0, /** 连接远端成功 */
    CALL_CONNECT_FAILED, /** 连接远端失败 */
    CALL_REMOTE_CONNECTED, /** 新的远端连接进来 */
    CALL_CONNECTION_BREAK, /** 连接断开 */
    CALL_RECV_DATA,  /** 新的数据到达 */

    /** add call type before this */
    CALL_TYPE_COUNT
}CALLBACK_TYPE;

typedef void (*CALL_FUN)(const s32 nConnectID, const void * pContext, const s32 nSize);

class INet {
public:
    /** 异步侦听,上下文会在CALL_REMOTE_CONNECTED和CALL_CONNECTION_BREAK事件中回带给使用者,注意只是回带一个指针地址 */
    virtual bool CListen(const char * pStrIP, const s32 nPort, const void * pContext, const s32 nBacklog = 2048)  = 0;

    /** 异步连接,上下文会在CALL_CONNECTED和CALL_CONNECTION_BREAK事件中回带给使用者,注意只是回带一个指针 */
    virtual bool CConnectEx(const char * pStrIP, const s32 nPort, const void * pContext = NULL)  = 0;

    /** 关闭连接,安全关闭,在所有数据尽可能发送完之后关闭连接 (该操作会促发CALL_CONNECTION_BREAK事件 */
    virtual bool CClose(const s32 nConnectID)  = 0;

    /** 发送数据 */
    virtual void CSend(const s32 nConnectID, const void * pData, const s32 nSize)  = 0;

    /** 获取连接远端信息 */
    virtual void CRemoteInfo(const s32 nConnectID, const char * & ip, s32 & nPort)  = 0;

    /** 设置回调地址,回调类型见CALLBACK_TYPE */
    virtual void CSetCallBackAddress(const CALLBACK_TYPE eType, const CALL_FUN address) = 0;

    /** 启动事件扑捉 demon是否为精灵模式, 弱不为精灵模式, nFramems为单帧loop事件, 当nFramems为0时CLoop一直阻塞 */
    virtual void CLoop(bool demon, s32 nFramems) = 0;
};


#define NAME_OF_GET_NET_FUN "GetNet"
#define GET_NET_FUN GetNet

#define NAME_OF_RETURN_NET_FUN "ReturnNet"
#define RETURN_NET_FUN ReturnNet

typedef INet* (*FUN_GET_NET)(const u8, const u16, const u16);
typedef void (*FUN_RETURN_NET)(INet *);

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
