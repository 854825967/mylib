#ifndef TQUEUE_H
#define TQUEUE_H
#include "CLock.h"

#define QUEUE_OPT_LOCK(b, lock) \
    if (b) { \
    lock->Lock(); \
    }
#define QUEUE_OPT_FREELOCK(b, lock) \
    if (b) { \
    lock->UnLock(); \
    }

enum {
    NOT_EXISTS_DATA = 0,
    EXISTS_DATA = 1
};
namespace tlib {
    typedef void (*default_fun_type)();
    template <typename type, bool lock = false, const s32 size = 4096, const void * pfun = NULL, typename... Args>
    class TQueue {
    public:
        TQueue() {
            m_nRIndex = 0;
            m_nWIndex = 0;
            m_nRCount = 0;
            m_nWCount = 0;
            if (lock) {
                m_pRlock = NEW CLockUnit;
                m_pWlock = NEW CLockUnit;
                ASSERT(m_pRlock && m_pWlock);
            }

            //memset(m_queue, 0, sizeof(m_queue));
            memset(m_sign, 0, sizeof(m_sign));
        }

        ~TQueue() {
            if (lock) {
                delete m_pWlock;
                delete m_pRlock;
            }
        }

        typedef void (*fun_type)(type &, Args...);
        inline void addByArgs(Args... args) {
            QUEUE_OPT_LOCK(lock, m_pWlock);
            while (m_sign[m_nWIndex] != NOT_EXISTS_DATA) {
                CSleep(10);
            }

            if (pfun != NULL) {
                ((fun_type)pfun)(m_queue[m_nWIndex++], args...);
            }
            m_nWCount++;
            m_sign[m_nWIndex-1] = EXISTS_DATA;

            if (m_nWIndex >= size) {
                m_nWIndex = 0;
            }
            QUEUE_OPT_FREELOCK(lock, m_pWlock);
        }

        inline void add(type src) {
            QUEUE_OPT_LOCK(lock, m_pWlock);
            while (m_sign[m_nWIndex] != NOT_EXISTS_DATA) {
                CSleep(10);
            }


            m_queue[m_nWIndex] = src;
            m_sign[m_nWIndex++] = EXISTS_DATA;
            m_nWCount++;
            if (m_nWIndex >= size) {
                m_nWIndex = 0;
            }
            QUEUE_OPT_FREELOCK(lock, m_pWlock);
        }

        inline bool read(type & value) {//不可在多个线程中使用 谢谢
            QUEUE_OPT_LOCK(lock, m_pRlock);
            while (m_sign[m_nRIndex] != EXISTS_DATA) {
                CSleep(10);
                QUEUE_OPT_FREELOCK(lock, m_pRlock);
                return false;
            }

            value = m_queue[m_nRIndex];
            m_sign[m_nRIndex++] = NOT_EXISTS_DATA;
            m_nRCount++;

            if (m_nRIndex >= size) {
                m_nRIndex = 0;
            }
            QUEUE_OPT_FREELOCK(lock, m_pRlock);
            return true;
        }

        inline bool IsEmpty() {
            return (m_nRCount == m_nWCount);
        }

    private:
        CLockUnit * m_pWlock;
        CLockUnit * m_pRlock;
        type m_queue[size];
        s8 m_sign[size];
        u32 m_nRIndex;
        u32 m_nWIndex;
        u32 m_nRCount;
        u32 m_nWCount;
    };
}
#endif //CQUEUE_H
