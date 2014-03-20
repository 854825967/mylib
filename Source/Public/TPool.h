#ifndef POOL_H
#define POOL_H

#include "CLock.h"
#include <list>
#include <algorithm>
using namespace std;

#define POOL_OPT_LOCK(b, lock) \
    if (b) { \
        lock->Lock(); \
    }
#define POOL_OPT_FREELOCK(b, lock) \
    if (b) { \
        lock->UnLock(); \
    }
namespace tlib {
    template <typename type, bool lock = false, s32 size = 4096>
    class TPOOL {
    public:
        typedef list<type *> UNIT_POOL;
        TPOOL() {
            for (s32 i=0; i<size; i++) {
                m_list.push_back(m_szUnit+i);
            }
            if (lock) {
                m_pLock = NEW CLockUnit;
                ASSERT(m_pLock);
            }
        }

        ~TPOOL() {
            if (lock) {
                delete m_pLock;
            }
        }

        type * Create() {
            type * p = NULL;
            POOL_OPT_LOCK(lock, m_pLock);
    #ifdef _DEBUG
            static s32 test = 0;
            test ++;
    #endif //_DEBUG
            UNIT_POOL::iterator itor = m_list.begin();
            if (itor != m_list.end()) {
                p = *itor;
                m_list.erase(itor);
            }
    #ifdef _DEBUG
            test --;
            ASSERT(test == 0);
    #endif //_DEBUG
            POOL_OPT_FREELOCK(lock, m_pLock);
            return p;
        }

        s32 QueryID(type * pUnit) {
            if ( (char *)pUnit < (char *)m_szUnit 
                || (char *)pUnit > (char *)(m_szUnit + size - 1)
                || 0 != ((char *)pUnit - (char *)m_szUnit)%sizeof(type) ) {
                    ASSERT(false);
                    return false;
            }

            return (s32)(pUnit - m_szUnit);
        }

        s32 CreateID() {
            type * p = NULL;
            POOL_OPT_LOCK(lock, m_pLock);
    #ifdef _DEBUG
            static s32 test = 0;
            test ++;
    #endif //_DEBUG
            UNIT_POOL::iterator itor = m_list.begin();
            if (itor != m_list.end()) {
                p = *itor;
                m_list.erase(itor);
            } else {
                ASSERT(false);
                return -1;
            }
    #ifdef _DEBUG
            test --;
            ASSERT(test == 0);
    #endif //_DEBUG
            POOL_OPT_FREELOCK(lock, m_pLock);
            return (s32)(p - m_szUnit);
        }

        type * operator[] (s32 nIndex) {
            if (nIndex >= size) {
                ASSERT(false);
                return NULL;
            }

            return &m_szUnit[nIndex];
        }

        bool Recover(type * pUnit) {
            if ( (char *)pUnit < (char *)m_szUnit 
                || (char *)pUnit > (char *)(m_szUnit + size - 1)
                || 0 != ((char *)pUnit - (char *)m_szUnit)%sizeof(type) ) {
                    ASSERT(false);
                    return false;
            }
            POOL_OPT_LOCK(lock, m_pLock);
    #ifdef _DEBUG
            static s32 test = 0;
            test ++;
    #endif //_DEBUG
            UNIT_POOL::iterator ibegin = m_list.begin();
            UNIT_POOL::iterator iend = m_list.end();
            UNIT_POOL::iterator itor = ::find(ibegin, iend, pUnit);
            if (itor != iend) {
    #ifdef _DEBUG
                test --;
                ASSERT(test == 0);
    #endif //_DEBUG
                POOL_OPT_FREELOCK(lock, m_pLock);
                return false;
            }
            m_list.push_back(pUnit);
    #ifdef _DEBUG
            test --;
            ASSERT(test == 0);
    #endif //_DEBUG
            POOL_OPT_FREELOCK(lock, m_pLock);
            return true;
        }

    private:
        UNIT_POOL m_list;
        type m_szUnit[size];
        CLockUnit * m_pLock;
    };
}

#endif //POOL_H
