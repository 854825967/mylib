#include "Message.h"
#include "IKernel.h"

Message * Message::s_pSelf = NULL;
CONNECTID_STREAM_MAP Message::s_ConnectID_Stream_Map;
TPOOL<TStream<4096, false>, false, 8092> Message::s_Stream_Pool;
CALL_POOL_MAP Message::s_CallPool_Map;

bool Message::Initialize(IKernel * pKernel) {
    s_pKernel = pKernel;
    RGS_EVENT_CALL(EVENT_TYPE_CORE, CORE_EVENT_RECV_DATA, Message::OnRecv);
    RGS_EVENT_CALL(EVENT_TYPE_CORE, CORE_EVENT_CONNECTED, Message::OnConnected);
    RGS_EVENT_CALL(EVENT_TYPE_CORE, CORE_EVENT_REMOTE_CONNECTED, Message::OnRemoteConnect);
    RGS_EVENT_CALL(EVENT_TYPE_CORE, CORE_EVENT_CONNECTION_BREAK, Message::OnConnectBreak);
    RGS_EVENT_CALL(EVENT_TYPE_CORE, CORE_EVENT_CONNECT_FAILED, Message::OnConnectFailed);
    s_pSelf = this;
    s_ConnectID_Stream_Map.clear();
    return true;
}

bool Message::DelayInitialize(IKernel * pKernel) {

    return true;
}

bool Message::Destroy(IKernel * pKernel) {

    return true;
}

bool Message::RgsMsgCall(const char * type, const s32 nMsgID, const MSG_CALL_BACK pCallBack, const char * pCallName) {
    return s_CallPool_Map[type].RegisterCall(nMsgID, pCallBack, pCallName);
}

s32 Message::OnRecv(const s8 nEventType, const s32 nConnectID, const void * pContext, const s32 nSize) {
    //CONNECTID_STREAM_MAP::iterator itor = s_ConnectID_Stream_Map.find(nConnectID);
    ASSERT(s_ConnectID_Stream_Map[nConnectID]);
    TStream<4096, false> * pStream = s_ConnectID_Stream_Map[nConnectID];
    pStream->in(pContext, nSize);
    s32 nLen = *(s32 *)pStream->buff();
    s32 nBuffSize = pStream->size();
    while (nLen <= nBuffSize && nBuffSize != 0) {
        s32 nMsgID = *(s32 *)(pStream->buff() + sizeof(s32));
        void * pData = (void *)(pStream->buff() + sizeof(s32) + sizeof(s32));
        const char * pType = (const char *)s_pSelf->s_pKernel->CContext(nConnectID);
        s_CallPool_Map[pType].Call(nMsgID, pType, nConnectID, pData, nLen - sizeof(nLen) - sizeof(nLen));
        pStream->out(nLen);
        nLen = *(s32 *)pStream->buff();
        nBuffSize = pStream->size();
    }

    return 0;
}

s32 Message::OnConnected(const s8 nEventType, const s32 nConnectID, const void * pContext, const s32 nSize) {
    s_ConnectID_Stream_Map[nConnectID] = s_Stream_Pool.Create();
    ASSERT(s_ConnectID_Stream_Map[nConnectID]);
    return 0;
}

s32 Message::OnRemoteConnect(const s8 nEventType, const s32 nConnectID, const void * pContext, const s32 nSize) {
    s_ConnectID_Stream_Map[nConnectID] = s_Stream_Pool.Create();
    ASSERT(s_ConnectID_Stream_Map[nConnectID]);

    const char * pStrIP = NULL;
    s32 nPort = -1;
    s_pKernel->RemoteInfo(nConnectID, pStrIP, nPort);
    ECHO("Remote Connected, Remote info %s:%d", pStrIP, nPort);

    return 0;
}

s32 Message::OnConnectBreak(const s8 nEventType, const s32 nConnectID, const void * pContext, const s32 nSize) {
    CONNECTID_STREAM_MAP::iterator itor = s_ConnectID_Stream_Map.find(nConnectID);
    ASSERT(itor != s_ConnectID_Stream_Map.end());
    s_Stream_Pool.Recover(itor->second);
    s_ConnectID_Stream_Map.erase(itor);
    return 0;
}

s32 Message::OnConnectFailed(const s8 nEventType, const s32 nConnectID, const void * pContext, const s32 nSize) {

    return 0;
}


