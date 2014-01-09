#include "MultiSys.h"
#include "../Public/CDumper.h"
#include "../Public/Tools.h"
#include "../Public/CHashMap.h"
#include <vector>
#include <iostream>

class InitDumper {
public:
    InitDumper() {
        CDumper::GetInstance().SetDumpFileName("Kernel_dump");
    }
};

#include "../Net_library/Interface/INet.h"

#define MSGID(main,sub) (((main) << 16 ) + (sub))
INet * pNet = NULL;
bool DnsParse(const char* domain, char * pStrIP, size_t size) {
    struct hostent * p;
    if ((p = gethostbyname(domain))==NULL) {
        return false;
    }

    SafeSprintf(pStrIP, size, size, "%u.%u.%u.%u", (unsigned char)p->h_addr_list[0][0],
        (unsigned char)p->h_addr_list[0][1],
        (unsigned char)p->h_addr_list[0][2],
        (unsigned char)p->h_addr_list[0][3]);
    return true;
}

struct connectinfo {
    s64 lTick;
};

typedef CHashMap<s32, connectinfo *> CONNECT_INFO_MAP;
s32 s_optCount = 0;
CONNECT_INFO_MAP s_map;
void Connected(const s32 nConnectID, const void * pContext, const s32 nSize) {
    s64 lTick = ::GetCurrentTimeTick() - ((const connectinfo *)pContext)->lTick;
    if (lTick > 1000) {
        //ECHO_ERROR("���Ӻ�ʱ:%ld", ::GetCurrentTimeTick() - ((const connectinfo *)pContext)->lTick);
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
    s_map.insert(make_pair(nConnectID, (connectinfo *)pContext));

}

void Recv(const s32 nConnectID, const void * pContext, const s32 nSize) {
    CONNECT_INFO_MAP::iterator itor = s_map.find(nConnectID);
    ASSERT(itor != s_map.end());
    s64 lTick = ::GetCurrentTimeTick() - itor->second->lTick;
    if (lTick > 3000) {
        static s32 i = 1;
        ECHO_ERROR("��ǰ�ܲ����� %d, �ܳ�ʱ����%d, �ӽ�����������ѯ�������ɹ�һ����ʱ %ld ms", s_optCount, i++, lTick);
    } else {
        //ECHO_TRACE("�ӽ�����������ѯ�������ɹ�һ����ʱ %ld ms", lTick);
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
        ECHO_ERROR("��ǰ�������� %d, ��������ʧ���ܴ���%d, ��ʱ %ld", s_optCount, i++, lTick);
        return false;
    }

    s64 lTick = ::GetCurrentTimeTick() - pInfo->lTick;
    if (lTick > 1000) {
        static s32 i = 1;
        ECHO_ERROR("��ǰ�������� %d, ��������ʧ���ܴ��� %d, ��ʱ %ld", s_optCount, i++, lTick);
    }
    return pNet->CConnectEx(szIP, 10038, pInfo);
}

void ConnectBreak(const s32 nConnectID, const void * pContext, const s32 nSize) {
    //ECHO_TRACE("���� %d �Ͽ�", nConnectID);
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
    ECHO_ERROR("��ǰ�ܲ����� %d, ��ʧ�ܴ���%d, ����ʧ��,��ʱ:%ld", s_optCount, i++, lTick);

    //ASSERT(ifind->second == pContext);
    bool b = Connect((connectinfo *) pContext);
    while (!b) {
        b = Connect((connectinfo *) pContext);
    }
}

s32 main(int argc, char * args[]) {
//     if (argc < 2) {
//         ECHO_TRACE("������������....");
//         getchar();
//         return 0;
//     } 
    WSADATA wsd;   
    ASSERT(WSAStartup(MAKEWORD(2,2), &wsd) == 0);

    char szPath[512] = {0};
    SafeSprintf(szPath, sizeof(szPath), "%s/%s", ::GetAppPath(), "libnet.dll");
    ECHO_TRACE("dll path : %s", szPath);

    pNet = GetNetWorker(szPath, 4);
    pNet->CSetCallBackAddress(CALL_CONNECTED, Connected);
    pNet->CSetCallBackAddress(CALL_CONNECT_FAILED, ConnectedFailed);
    pNet->CSetCallBackAddress(CALL_RECV_DATA, Recv);
    pNet->CSetCallBackAddress(CALL_CONNECTION_BREAK, ConnectBreak);
    for (s32 i=0; i<4; i++) {
        connectinfo * pInfo = NEW connectinfo;
        bool b = Connect(pInfo);
        while (!b) {
            b = Connect(pInfo);
        }
    }

    pNet->CLoop(false, 0);
	return 0;
}
