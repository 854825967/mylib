#if defined WIN32 || defined WIN64

#ifndef CNET_H
#define CNET_H

#include "INet.h"
#include "TQueue.h"
#include "TPool.h"
#include "CConnection.h"

class CNet : public INet {
public:
    CNet(const s8 nThreadCount, const s32 nLinkConut, const s32 nWaitMs);
    ~CNet();

    /** 异步侦听,上下文会在CALL_REMOTE_CONNECTED事件中回带给使用者,注意只是回带一个指针地址 */
    virtual bool CListen(const char * pStrIP, const s32 nPort, const void * pContext, const s32 nBacklog);

    /** 异步连接,上下文会在CALL_CONNECTED事件中回带给使用者,注意只是回带一个指针 */
    virtual bool CConnectEx(const char * pStrIP, const s32 nPort, const void * pContext);

    /** 发送数据 */
    virtual void CSend(const s32 nConnectID, const void * pData, const s32 nSize);

    /** 获取连接远端信息 */
    virtual void CRemoteInfo(const s32 nConnectID, const char * & ip, s32 & nPort);

    /** 关闭连接,安全关闭,在所有数据尽可能发送完之后关闭连接 (该操作会促发CALL_CONNECTION_BREAK事件 */
    virtual bool CClose(const s32 nConnectID);

    /** 设置回调地址,回调类型见CALLBACK_TYPE */
    virtual void CSetCallBackAddress(const CALLBACK_TYPE eType, const CALL_FUN address);    
    
    /** 启动事件扑捉 demon是否为精灵模式, 弱不为精灵模式, nFramems为单帧loop事件, 当nFramems为0时CLoop一直阻塞 */
    virtual void CLoop(bool demon, s32 nFramems);

private:
    void DealConnectEvent(struct iocp_event * pEvent);
    void DealAcceptEvent(struct iocp_event * pEvent);
    void DealRecvEvent(struct iocp_event * pEvent);
    void DealSendEvent(struct iocp_event * pEvent);

    void SafeShutdwon(struct iocp_event * pEvent, s32 sdtags);

private:
    static THREAD_FUN DemonLoop(LPVOID p);
    static THREAD_FUN NetLoop(LPVOID p);
    
private:
    s64 m_nWaitMs;
    s64 m_nFrameMs;
    s32 m_nThreadCount;
    CALL_FUN m_szCallAddress[CALL_TYPE_COUNT];
    tlib::TPOOL<CConnection, true, 8192> m_ConnectPool;
    tlib::TQueue<struct iocp_event *, true, 8192> m_queue;
    bool m_demo;
};

#endif
#endif //#if defined WIN32 || defined WIN64
