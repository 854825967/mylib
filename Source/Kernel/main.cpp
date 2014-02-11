#include "MultiSys.h"
#include "CDumper.h"
#include "Tools.h"
#include "CHashMap.h"
#include "CLock.h"
#include <vector>
#include <iostream>

#ifdef WIN32
class InitDumper {
public:
    InitDumper() {
        CDumper::GetInstance().SetDumpFileName("Kernel_dump");
    }
};
#endif //WIN32

#include "../Net_library/Interface/INet.h"

#define MSGID(main,sub) (((main) << 16 ) + (sub))
INet * pNet = NULL;
bool DnsParse(const char* domain, char * pStrIP, size_t size) {
    struct hostent * p;
#ifdef WIN32
   if ((p = gethostbyname(domain))==NULL) {
       return false;
   }

   SafeSprintf(pStrIP, size, size, "%u.%u.%u.%u", (unsigned char)p->h_addr_list[0][0],
       (unsigned char)p->h_addr_list[0][1],
       (unsigned char)p->h_addr_list[0][2],
       (unsigned char)p->h_addr_list[0][3]);
#endif //WIN32
    return true;
}

struct connectinfo {
    s64 lTick;
};

typedef CHashMap<s32, connectinfo *> CONNECT_INFO_MAP;
s32 s_optCount = 0;
CONNECT_INFO_MAP s_map;
CLockUnit * plock;
void Connected(const s32 nConnectID, const void * pContext, const s32 nSize) {
    s64 lTick = ::GetCurrentTimeTick() - ((const connectinfo *)pContext)->lTick;
    if (lTick > 1000) {
        //ECHO_ERROR("连接耗时:%ld", ::GetCurrentTimeTick() - ((const connectinfo *)pContext)->lTick);
    }

    char buf[512];
    memset(buf, 0, sizeof(buf));

    int msgId = MSGID(0x1000,0x43);
    memcpy(buf+sizeof(unsigned short),(const void*)&msgId,sizeof(int));
    unsigned long long acid = 0;
    memcpy(buf+sizeof(unsigned short) + sizeof(unsigned int),&acid,sizeof(unsigned long long));
    unsigned short len = sizeof(unsigned int) + sizeof(unsigned long long);
    memcpy(buf,&len,sizeof(unsigned short));

    int count = sizeof(unsigned short) + sizeof(unsigned int) + sizeof(unsigned long long);
    pNet->CSend(nConnectID, buf, count);

    CAutoLock lock(plock);
    s_map.insert(make_pair(nConnectID, (connectinfo *)pContext));

}

void Recv(const s32 nConnectID, const void * pContext, const s32 nSize) {
    CAutoLock lock(plock);
    CONNECT_INFO_MAP::iterator itor = s_map.find(nConnectID);
    ASSERT(itor != s_map.end());
    s64 lTick = ::GetCurrentTimeTick() - itor->second->lTick;
    if (lTick > 3000) {
        static s32 i = 1;
        ECHO_ERROR("当前总操作数 %d, 总超时次数%d, 从解析域名到查询服务器成功一共耗时 %ld ms", s_optCount, i++, lTick);
    } else {
        //ECHO_TRACE("从解析域名到查询服务器成功一共耗时 %ld ms", lTick);
    }
}

bool Connect(connectinfo * pInfo) {
    s_optCount++;
    char szIP[128] = {0};
    memset(szIP, 0, sizeof(szIP));
    pInfo->lTick = ::GetCurrentTimeTick();
    if (!DnsParse("pay.zstb.android.haohaowan.com", szIP, sizeof(szIP))) {
        s64 lTick = ::GetCurrentTimeTick() - pInfo->lTick;
        static s32 i = 1;
        ECHO_ERROR("当前操作总数 %d, 解析域名失败总次数%d, 耗时 %ld", s_optCount, i++, lTick);
        return false;
    }

    s64 lTick = ::GetCurrentTimeTick() - pInfo->lTick;
    if (lTick > 1000) {
        static s32 i = 1;
        ECHO_ERROR("当前操作总数 %d, 解析域名失败总次数 %d, 耗时 %ld", s_optCount, i++, lTick);
    }
    return pNet->CConnectEx(szIP, 10038, pInfo);
}

void ConnectBreak(const s32 nConnectID, const void * pContext, const s32 nSize) {
    //ECHO_TRACE("连接 %d 断开", nConnectID);
    CAutoLock lock(plock);
    CONNECT_INFO_MAP::iterator ifind = s_map.find(nConnectID);
    if (ifind != s_map.end()) {
        ASSERT(ifind->second == pContext);
        Connect((connectinfo *) pContext);
        s_map.erase(ifind);
    }
}

void ConnectedFailed(const s32 nConnectID, const void * pContext, const s32 nSize) {
    s_optCount++;
    static s32 i = 1;
    s64 lTick = ::GetCurrentTimeTick() - ((const connectinfo *)pContext)->lTick;
    ECHO_ERROR("当前总操作数 %d, 总失败次数%d, 连接失败,耗时:%ld", s_optCount, i++, lTick);

    //ASSERT(ifind->second == pContext);
    bool b = Connect((connectinfo *) pContext);
    while (!b) {
        b = Connect((connectinfo *) pContext);
    }
}


THREAD_FUN CLoop(LPVOID p) {
    pNet->CLoop(false, 0);
    return 0;
}

s32 main(int argc, char * args[]) {
//     if (argc < 2) {
//         ECHO_TRACE("请输入域名先....");
//         getchar();
//         return 0;
//     }

    plock = NEW CLockUnit();

#if defined WIN32 || defined WIN64
    WSADATA wsd;
    ASSERT(WSAStartup(MAKEWORD(2,2), &wsd) == 0);
#endif //defined WIN32 || defined WIN64
    char szPath[512] = {0};
#ifdef WIN32
    SafeSprintf(szPath, sizeof(szPath), "%s/%s", ::GetAppPath(), "libnet.dll");
#elif defined linux
    SafeSprintf(szPath, sizeof(szPath), "%s/%s", ::GetAppPath(), "libnet.so");
#endif //WIN32
    ECHO_TRACE("dll path : %s", szPath);

    pNet = GetNetWorker(szPath, 4);
    ASSERT(pNet);
    pNet->CSetCallBackAddress(CALL_CONNECTED, Connected);
    pNet->CSetCallBackAddress(CALL_CONNECT_FAILED, ConnectedFailed);
    pNet->CSetCallBackAddress(CALL_RECV_DATA, Recv);
    pNet->CSetCallBackAddress(CALL_CONNECTION_BREAK, ConnectBreak);
    for (s32 i=0; i<2048; i++) {
        connectinfo * pInfo = NEW connectinfo;
        bool b = Connect(pInfo);
        while (!b) {
            b = Connect(pInfo);
        }
    }

    ::CreateThread(NULL, 0, CLoop, (LPVOID)NULL, 0, NULL);
    ::CreateThread(NULL, 0, CLoop, (LPVOID)NULL, 0, NULL);
    ::CreateThread(NULL, 0, CLoop, (LPVOID)NULL, 0, NULL);
    ::CreateThread(NULL, 0, CLoop, (LPVOID)NULL, 0, NULL);
    ::CreateThread(NULL, 0, CLoop, (LPVOID)NULL, 0, NULL);
    ::CreateThread(NULL, 0, CLoop, (LPVOID)NULL, 0, NULL);
    ::CreateThread(NULL, 0, CLoop, (LPVOID)NULL, 0, NULL);

    pNet->CLoop(false, 0);
	return 0;
}

