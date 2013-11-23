/* 
 * File:   LogQueue.h
 * Author: traveler
 *
 * Created on January 10, 2013, 3:03 PM
 */

#ifndef LOGQUEUE_H
#define	LOGQUEUE_H

#include "ILogQueue.h"

template<u32 SIZE = 4096 >
        class LogQueue : public ILogQueue{
public:
    LogQueue() {
        m_nCrtOut = 0;
        m_nCrtIn = 0;
		memset(m_logpool, 0, sizeof(m_logpool));
    }

    virtual bool Out(OUT Log & log) {
        if (m_nCrtOut >= SIZE) {
            m_nCrtOut = 0;
        }
        if (NOT_IN_USE == m_logpool[m_nCrtOut].m_nType ) {
            CSleep(1);
            if (NOT_IN_USE == m_logpool[m_nCrtOut].m_nType ) {
                return false;
            }
        }

        log = m_logpool[m_nCrtOut];
        m_logpool[m_nCrtOut].m_nType = NOT_IN_USE;
        m_nCrtOut++;
        return true;
    }

    virtual bool In(const u8 type, const u64 timetick, const char * log) {
        if(type == NOT_IN_USE) {
            ASSERT(false);
            return false;
        }
        if (m_nCrtIn >= SIZE) {
            m_nCrtIn = 0;
        }
        if (NOT_IN_USE != m_logpool[m_nCrtIn].m_nType) {
            CSleep(1);
            if (NOT_IN_USE != m_logpool[m_nCrtIn].m_nType) {
                return false;
            }
        }
        
        m_logpool[m_nCrtIn].Initialize(type, timetick, log);
        m_nCrtIn++;
        return true;
    }

    virtual void Clear() {

    }

    ~LogQueue() {

    }
private:
    Log m_logpool[SIZE];
    u32 m_nCrtOut; //position about current out
    u32 m_nCrtIn; //position about current in
};

#endif	/* LOGQUEUE_H */

