#ifdef linux
#ifndef LINUXNETCORE_H
#define LINUXNETCORE_H
#include "INet.h"

class CNet : public INet {
public:
    CNet(const u8 threadcount, const u16 linkcount, const u16 iowaittime);
    ~CNet();

    /** 异步侦听,上下文会在CALL_REMOTE_CONNECTED和CALL_CONNECTION_BREAK事件中回带给使用者,注意只是回带一个指针地址 */
    virtual bool CListen(const char * pStrIP, const s32 nPort, const void * pContext, const s32 nBacklog);

    /** 异步连接,上下文会在CALL_CONNECTED和CALL_CONNECTION_BREAK事件中回带给使用者,注意只是回带一个指针 */
    virtual bool CConnectEx(const char * pStrIP, const s32 nPort, const void * pContext = NULL);

    /** 关闭连接,安全关闭,在所有数据尽可能发送完之后关闭连接 (该操作会促发CALL_CONNECTION_BREAK事件 */
    virtual bool CClose(const s32 nConnectID);

    /** 发送数据 */
    virtual void CSend(const s32 nConnectID, const void * pData, const s32 nSize);

    /** 获取连接远端信息 */
    virtual void CRemoteInfo(const s32 nConnectID, const char * & ip, s32 & nPort);

    /** 设置回调地址,回调类型见CALLBACK_TYPE */
    virtual void CSetCallBackAddress(const CALLBACK_TYPE eType, const CALL_FUN address);

    /** 启动事件扑捉 demon是否为精灵模式, 弱不为精灵模式, nFramems为单帧loop事件, 当nFramems为0时CLoop一直阻塞 */
    virtual void CLoop(bool demon, s32 nFramems);
private:

private:
};

#endif //LINUXNETCORE_H
#endif //linux
