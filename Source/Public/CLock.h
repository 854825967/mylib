#ifndef CAUTOLOCK_H
#define CAUTOLOCK_H

#include "MultiSys.h"
#include "CLockUnit.h"

namespace tlib {
    class CAutoLock {
    public:
        CAutoLock(CLockUnit * pLock) {
            ASSERT(pLock != NULL);
            m_pLock = pLock;
            m_pLock->Lock();
        }

        void Free() {
            ASSERT(m_pLock != NULL);
            m_pLock->UnLock();
        }

        ~CAutoLock() {
            m_pLock->UnLock();
        }

    private:
        CLockUnit * m_pLock;
    };
}

#endif