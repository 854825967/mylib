#include "../libnet/Interface/INet.h"
#include "Tools.h"

INet * g_pNet = NULL;

void Connected(const s32 nConnectID, const void * pContext, const s32 nSize) {
    s32 nPort;
    const char * ip = NULL;
    g_pNet->CRemoteInfo(nConnectID, ip, nPort);

    ECHO_TRACE("连接成功, 上下文 %s, 远端IP %s 端口 %d", (char *)pContext, ip, nPort);
    g_pNet->CSend(nConnectID, pContext, strlen((char *)pContext));

}

void ConnectFail(const s32 nConnectID, const void * pContext, const s32 nSize) {
    ECHO_TRACE("连接失败, 上下文 %s", (char *)pContext);
}

void ConnectionBreak(const s32 nConnectID, const void * pContext, const s32 nSize) {
    s32 nPort;
    const char * ip = NULL;
    g_pNet->CRemoteInfo(nConnectID, ip, nPort);

    ECHO_TRACE("连接断开, 上下文 %s, 远端IP %s 端口 %d", (char *)pContext, ip, nPort);

}

void RemoteConnected(const s32 nConnectID, const void * pContext, const s32 nSize) {
    s32 nPort;
    const char * ip = NULL;
    g_pNet->CRemoteInfo(nConnectID, ip, nPort);

    ECHO_TRACE("新的远端连进来, 上下文 %s, 远端IP %s 端口 %d", (char *)pContext, ip, nPort);
}

void Recv(const s32 nConnectID, const void * pContext, const s32 nSize) {
    //ECHO_TRACE("%s", (char *)pContext);
    g_pNet->CSend(nConnectID, pContext, strlen((char *)pContext));
    CSleep(10);
}

int main() {
#if defined WIN32 || defined WIN64
    WSAData wsd;
    if (WSAStartup(MAKEWORD(2, 2), &wsd)) {
        ECHO_ERROR("init net error");
        return 0;
    }
#endif //#if defined WIN32 || defined WIN64
    char szPath[512] = {0};
    SafeSprintf(szPath, sizeof(szPath), "%s/libnet.dll", ::GetAppPath());

    g_pNet = ::GetNetWorker(szPath, 4);

    g_pNet->CSetCallBackAddress(CALL_CONNECTED, Connected);
    g_pNet->CSetCallBackAddress(CALL_CONNECT_FAILED, ConnectFail);
    g_pNet->CSetCallBackAddress(CALL_REMOTE_CONNECTED, RemoteConnected);
    g_pNet->CSetCallBackAddress(CALL_CONNECTION_BREAK, ConnectionBreak);
    g_pNet->CSetCallBackAddress(CALL_RECV_DATA, Recv);

    g_pNet->CConnectEx("220.181.111.85", 80, "220.181.111.85");
    g_pNet->CListen("127.0.0.1", 10038, "127.0.0.1");
    g_pNet->CConnectEx("127.0.0.1", 10038, "127.0.0.1");
    g_pNet->CLoop(false, 0);

    return 0;
}
