
#if defined WIN32 || defined WIN64

#ifndef SOCKPLUS_H
#define SOCKPLUS_H

#include "MultiSys.h"

enum {
    EVENT_RECV = 0,
    EVENT_SEND,
    EVENT_ACCEPT,
    EVENT_CONNECT,

    //ADD EVENT TYPE BEFORE HERE
    EVENT_TYPE_COUT,
};

class SockPlus
{
public:
    OVERLAPPED m_overlapped;  //不解释,该成员变量必须放在第一位
    u8 m_events;
    WSABUF m_wBuf;
    DWORD m_dwBytes, m_dwFlags;
    char m_buff[BUFF_SIZE];
    SOCKET m_acceptSock;

    SockPlus() {
        Initialize();
    }

    ~SockPlus(){

    }

    void Initialize() {
        memset(this, 0, sizeof(SockPlus));
        m_wBuf.len = BUFF_SIZE;
        m_wBuf.buf = m_buff;
        //m_dwBytes = 0;
        //m_dwFlags = 0;
    }

    void SetEventType(const u8 type) {
        ASSERT(type < EVENT_TYPE_COUT);
        m_events = type;
    }
};

#endif //SOCKPLUS_H

#endif //#if defined WIN32 || defined WIN64
