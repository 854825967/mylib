/* 
 * File:   LogSystem.h
 * Author: traveler
 *
 * Created on January 5, 2013, 11:03 AM
 */

#ifndef LOGSYSTEM_H
#define LOGSYSTEM_H

#include "MultiSys.h"
#include "ILog.h"
#include "CHashMap.h"
#include "LogQueue.h"
#include <string>
#include <fstream>
using namespace std;

typedef CHashMap<ThreadID, ILogQueue * > LOGQUEUE_MAP;


class LogSystem : public ILog {
public:
    LogSystem(const char * filename, const u32 duration = 3600, const u32 maxsize = 50);
    ~LogSystem();

    virtual bool EchoSwitch(bool echo);
    virtual bool Trace(const char * pStrLog);
    virtual bool Debug(const char * pStrLog);
    virtual bool Error(const char * pStrLog);
    virtual bool EchoTrace(const char * pStrLog);
    virtual bool EchoError(const char * pStrLog);
    virtual bool EchoDebug(const char * pStrLog);
    virtual void StartLoop(bool demon);
private:
    bool LogInQueue(const u8 type, const u64 timetick, const char * log);
    static THREAD_FUN WriteThread(LPVOID p);
    void WriteLog(Log & log);
    bool GetLogFileHandle(const u64 tick);
    const string GetLogFilePath(const u64 nCurrent);
private:
    bool m_EchoSwitch;
    bool m_Dead;
    string m_strFileName;
    u64 m_nLastTick;
    u64 m_nCurFilesize;
    u64 m_nStartTime;
    u32 m_nDuration;
    u32 m_nMaxFileSize;
    ThreadID m_nWriteThreadID;
    LOGQUEUE_MAP m_logQueueMap;
    ofstream m_logfile;
};

#endif	/* LOGSYSTEM_H */
