#ifndef CQUEUE_H
#define CQUEUE_H

#include "iocp_event.h"
#include "CLock.h"

class CQueue {
public:
    CQueue() {
        m_nRIndex = 0;
        m_nWIndex = 0;
        memset(m_queue, 0, sizeof(m_queue));
    }

    void add(struct iocp_event * pEvent) {
        CAutoLock lock(&m_wlock);
        while (m_queue[m_nWIndex] != NULL) {
            CSleep(10);
        }

        m_queue[m_nWIndex++] = pEvent;
        if (m_nWIndex >= BUFF_SIZE) {
            m_nWIndex = 0;
        }
    }

    struct iocp_event * read() {//�����ڶ���߳���ʹ�� лл
        CAutoLock lock(&m_rlock);
        while (m_queue[m_nRIndex] == NULL) {
            CSleep(1);
            return NULL;
        }

        struct iocp_event * pEvent = m_queue[m_nRIndex];
        m_queue[m_nRIndex++] = NULL;

        if (m_nRIndex >= BUFF_SIZE) {
            m_nRIndex = 0;
        }
        return pEvent;
    }

private:
    CLockUnit m_wlock;
    CLockUnit m_rlock;
    struct iocp_event * m_queue[BUFF_SIZE];
    u32 m_nRIndex;
    u32 m_nWIndex;
};

#endif //CQUEUE_H
