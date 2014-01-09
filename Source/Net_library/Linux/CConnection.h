#ifndef CCONNECTION_H
#define CCONNECTION_H

#include "MultiSys.h"
#include "CStream.h"

class CConnection {
public:
    char szIP[32];
    s32 nPort;
    void * pContext;
    bool bShutdown;
    SOCKET s;
    CStream stream;
    CConnection() {
        memset(szIP, 0, sizeof(szIP));
        nPort = 0;
        pContext = NULL;
        s = INVALID_SOCKET;
        bShutdown = false;
    }
};

#endif //CCONNECTION_H
