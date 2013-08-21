#ifndef CQUEUE_H
#define CQUEUE_H

#include "CLock.h"

enum {
    CEVENT_CONNECT_SECUESS,
    CEVENT_CONNECT_FAILED,
    CEVENT_CONNECT_BREAK,
    CEVENT_NEW_CONNECT,
    CEVENT_RECV,

    /*********************/
    CENENT_TYPE_COUNT,
};

struct cevent {
    u8 isready; //0,unready  1,ready
    u32 id;
    u8 type;
    char buff[BUFF_SIZE];
    u32 size;

    cevent() {
        memset(this, 0, sizeof(*this));
    }
};

class CQueue {
public:
    CQueue() {
        m_nRIndex = 0;
        m_nWIndex = 0;
    }

    bool AddEvent(const u32 id, const u8 type, const void * buff, const u32 size) {
        ASSERT(type < CENENT_TYPE_COUNT && size <= BUFF_SIZE);
        CAutoLock lock(&m_lock);
        if (m_queue[m_nWIndex].isready) {
            CSleep(100);
            if (m_queue[m_nWIndex].isready) {
                ASSERT(false);
                return false;
            }
        }

        m_queue[m_nWIndex].id = id;
        m_queue[m_nWIndex].type = type;
        memcpy(m_queue[m_nWIndex].buff, buff, size);
        m_queue[m_nWIndex].size = size;
        m_queue[m_nWIndex].isready = 1;

        m_nWIndex++;
        if (m_nWIndex >= BUFF_SIZE) {
            m_nWIndex = 0;
        }
        return true;
    }
    bool ReadEvent(OUT u32 & id, OUT u8 & type, OUT void * & buff, OUT u32 & size) {//不可在多个线程中使用 谢谢
        if (!m_queue[m_nRIndex].isready) {
            CSleep(1);
            if (!m_queue[m_nRIndex].isready) {
                return false;
            }
        }

        id = m_queue[m_nRIndex].id;
        type = m_queue[m_nRIndex].type;
        buff = (void *)m_queue[m_nRIndex].buff;
        size = m_queue[m_nRIndex].size;
        m_queue[m_nRIndex].isready = 0;

        m_nRIndex++;
        if (m_nRIndex >= BUFF_SIZE) {
            m_nRIndex = 0;
        }
        return true;
    }
    //bool RemoveEvent();
private:
    CLockUnit m_lock;
    cevent m_queue[BUFF_SIZE];
    u32 m_nRIndex;
    u32 m_nWIndex;
};
#endif //CQUEUE_H
