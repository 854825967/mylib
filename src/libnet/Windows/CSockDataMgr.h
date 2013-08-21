#if defined WIN32 || defined WIN64
#ifndef CSOCKPLUSMGR_H
#define CSOCKPLUSMGR_H

#include "CLock.h"
#include "CStream.h"

#include <hash_map>
#include <list>
using namespace std;
using namespace stdext;

#include "CInetAddress.h"
#include "SockData.h"
#include "CLock.h"

#define MAX_SOCKPLUS_COUNT 4096


typedef hash_map<u16, SockData *> ID_SOCKPLUS;
typedef list<u16> LIST_ID;

class CSockDataMgr {
public:
    CSockDataMgr(const u16 size = MAX_SOCKPLUS_COUNT);
    ~CSockDataMgr();
    SockData * CreateSockData();
    SockData * QuerySockData(const u16 id);
    void RecoveSockData(SockData * pSockData);

private:
    //createlock
    LIST_ID m_IdRecycleBin;
    SockData * m_pSockDataArray;
    u16 m_nCurrentMark;
    u16 m_nMaxCount;
    CLockUnit m_lock;
};

#endif    //CSOCKPLUSMGR_H
#endif //#if defined WIN32 || defined WIN64
