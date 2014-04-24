#ifndef CCONNECTION_H
#define CCONNECTION_H

#include "MultiSys.h"
#include "TStream.h"
#include "CLockUnit.h"
using namespace tlib;

#define CSD_RECV 0x01
#define CSD_SEND 0x02
#define CSD_BOTH 0x03

class CConnection {
public:
    char szIP[32];
    s32 nPort;
    void * pContext;
    bool bShutdown;
    SOCKET s;
    s32 nSDTags;
    CLockUnit sdlock;
    TStream<4096, true> stream;

    CConnection() {
        Clear();
    }

    void Clear() {
        memset(szIP, 0, sizeof(szIP));
        nPort = 0;
        pContext = NULL;
        s = INVALID_SOCKET;
        bShutdown = false;
        nSDTags = 0;
    }
};

#endif //CCONNECTION_H
