/* 
 * File:   ILogQueue.h
 * Author: traveler
 *
 * Created on January 10, 2013, 3:34 PM
 */

#ifndef ILOGQUEUE_H
#define	ILOGQUEUE_H

#include "MultiSys.h"

#define LOG_MAX_SIZE 4096
enum {
    NOT_IN_USE = 0,
    LOG_WRITE_TRACE,
    LOG_WRITE_DEBUG,
    LOG_WRITE_ERROR,
    LOG_ECHO_TRACE,
    LOG_ECHO_DEBUG,
    LOG_ECHO_ERROR,

    //==============
    LOG_TYPE_COUNT
};

class Log {
public:
    u8 m_nType;
    u64 m_nTick;
    char m_strLog[LOG_MAX_SIZE];

    Log() {
        m_nType = NOT_IN_USE;
        m_nTick = 0;
        m_strLog[LOG_MAX_SIZE-1] = 0;
    }

    bool Initialize(const u8 type, const u64 timetick, const char * log) {
        ASSERT(type < LOG_TYPE_COUNT);
        SafeSprintf(m_strLog, sizeof(m_strLog)-1, "%s", log);
        m_nTick = timetick;
        m_nType = type;
        return true;
    }

    inline Log & operator=(const Log & target) {
        m_nTick = target.m_nTick;
        SafeSprintf(m_strLog, sizeof(m_strLog)-1, "%s", target.m_strLog);
        m_nType = target.m_nType;
        return *this;
    }

};

class ILogQueue {
public:
    virtual bool Out(Log &) = 0; //get log from queue
    virtual bool In(const u8 type, const u64 timetick, const char * log) = 0; //log in queue
    virtual void Clear() = 0;
};

#endif	/* ILOGQUEUE_H */

