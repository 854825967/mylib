#ifndef CCONNECTION_H
#define CCONNECTION_H

#include "MultiSys.h"
#include "CStream.h"
#include "CLockUnit.h";
using namespace tlib;

class CConnection {
public:
    char szIP[32];
    s32 nPort;
    void * pContext;
    bool bShutdown;
    SOCKET s;
    s32 nSDTags;
    CLockUnit sdlock;
    CStream stream;
    CConnection() {
        memset(szIP, 0, sizeof(szIP));
        nPort = 0;
        pContext = NULL;
        s = INVALID_SOCKET;
        bShutdown = false;
        nSDTags = 0;
    }
};

#endif //CCONNECTION_H
