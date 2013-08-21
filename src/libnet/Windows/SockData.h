#ifndef SOCKDATA_H
#define SOCKDATA_H

#include "MultiSys.h"
#include "CInetAddress.h"
#include "CStream.h"
#include "SockPlus.h"
#include "Pool.h"

class SockData : public pool_unit {
public:
    CInetAddress m_address;
    u16 m_id;
    SOCKET m_sock;            //当前连接
    bool m_isclose;

    bool Write(const void * buff, u32 size) {
        return m_sendbuff.In(buff, size);
    }

    char * Read(u32 & size) {
        size = m_sendbuff.GetLength();
        return m_sendbuff.GetBuff();
    }

    bool StreamIsEmpty() {
        return (0 == m_sendbuff.GetLength());
    }

    void Out(u32 nSize) {
        m_sendbuff.Out(nSize);
    }

    virtual void Clear() {
        m_id = 0;
        m_sock = INVALID_SOCKET;
        m_isclose = false;
        m_sendbuff.Clear();
        m_address.Initialize();
        for (u8 i=0; i<EVENT_TYPE_COUT; i++) {
            m_sockPlus[i].Initialize();
            m_sockPlus[i].SetEventType(i);
        }
    }

    SockData() {
        Clear();
    }

    SockPlus * GetSockPlus(const u8 type) {
        ASSERT(type < EVENT_TYPE_COUT);
        return &m_sockPlus[type];
    }

private :
    CStream m_sendbuff;
    SockPlus m_sockPlus[EVENT_TYPE_COUT];
};

#endif //SOCKDATA_H
