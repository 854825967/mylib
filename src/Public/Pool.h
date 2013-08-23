#ifndef POOL_H
#define POOL_H

#include "CLock.h"
#include <list>
#include <algorithm>
using namespace std;

#define TYPE_ID u32
class pool_unit {
public:
    virtual void Clear() = 0;
public:
    pool_unit() {
        m_nID = (u32)-1;
    }
    virtual void SetID(const u32 id) {
        m_nID = id;
    }

    virtual u32 GetID() {
        return m_nID;
    }

private:
    u32 m_nID;
};

#define POOL_OPT_LOCK(b, lock) if (b) { lock.Lock(); }
#define POOL_OPT_FREELOCK(b, lock) if (b) { lock.UnLock(); }

//注意 type 必须 继承上面的类
template <typename type, bool islock = false, int size = 4096>
class POOL 
{
public:
typedef list<type *> UNIT_POOL;

    POOL() {
        //memset(m_unit, 0, sizeof(m_unit));
        for (u32 i=0; i<size; i++) {
            m_unit[i].SetID(i);
            m_pool.push_back(m_unit+i);
        }
    }

    ~POOL() {

    }

    type * Create() {
        type * p = NULL;
        static u32 test = 0;
        POOL_OPT_LOCK(islock, m_lock);
        test ++;
        UNIT_POOL::iterator itor = m_pool.begin();
        if (itor != m_pool.end()) {
            p = *itor;
            m_pool.erase(itor);
        }
        test --;
        ASSERT(test == 0);
        POOL_OPT_FREELOCK(islock, m_lock);
        return p;
    }

    type * Query(const u32 id) {
        if (id >= size) {
            ASSERT(false);
            return NULL;
        }

        static u32 test = 0;
        POOL_OPT_LOCK(islock, m_lock);
        test ++;
        type * punit = &m_unit[id];
        UNIT_POOL::iterator ibegin = m_pool.begin();
        UNIT_POOL::iterator iend = m_pool.end();
        UNIT_POOL::iterator itor = ::find(ibegin, iend, punit);
        if (itor != iend) {
            //ASSERT(false);
            test --;
            ASSERT(test == 0);
            POOL_OPT_FREELOCK(islock, m_lock);
            return NULL;
        }
        test --;
        ASSERT(test == 0);
        POOL_OPT_FREELOCK(islock, m_lock);
        return punit;
    }

    bool Recover(type * punit) {
        if ( (char *)punit < (char *)m_unit 
            || (char *)punit > (char *)(m_unit + size - 1)
            || 0 != ((char *)punit - (char *)m_unit)%sizeof(type) ) {
                ASSERT(false);
                return false;
        }
        static u32 test = 0;
        POOL_OPT_LOCK(islock, m_lock);
        test ++;
        UNIT_POOL::iterator ibegin = m_pool.begin();
        UNIT_POOL::iterator iend = m_pool.end();
        UNIT_POOL::iterator itor = ::find(ibegin, iend, punit);
        if (itor != iend) {
            test --;
            //ASSERT(test == 0);
            POOL_OPT_FREELOCK(islock, m_lock);
            return false;
        }
        m_pool.push_back(punit);
        punit->Clear();
        test --;
        //ASSERT(test == 0);
        POOL_OPT_FREELOCK(islock, m_lock);
        return true;
    }
    
private:
    UNIT_POOL m_pool;
    type m_unit[size];
    CLockUnit m_lock;
};

#endif //POOL_H
