#if defined WIN32 || defined WIN64

#include "LogSystem.h"
#include "Tools.h"

LogSystem::LogSystem(const char * filename, const u32 duration/* = 3600*/, const u32 maxsize/* = 128 * 1024*/) {
    m_strFileName = filename;
    m_EchoSwitch = true;
    m_Dead = false;
    m_nDuration = duration;
    m_nMaxFileSize = maxsize;
    m_nLastTick = 0;
    m_nStartTime = ::GetCurrentTimeTick();
    m_nWriteThreadID = -1;
    m_nCurFilesize = 0;
    char path[256] = {0};
    SafeSprintf(path, sizeof(path), "%s/log", ::GetAppPath());
    ::mkdir(path);
}

LogSystem::~LogSystem() {

}

bool LogSystem::EchoSwitch(bool echo) {
    m_EchoSwitch = echo;
    return true;
}

bool LogSystem::Trace(const char * pStrLog) {
    char charyLog[4096] = {0};
    SafeSprintf(charyLog, sizeof(charyLog), "[trace][%s]:%s", GetCurrentTimeString().c_str(), pStrLog);
    LogInQueue(LOG_WRITE_TRACE, ::GetCurrentTimeTick(), pStrLog);
    return true;
}

bool LogSystem::Debug(const char * pStrLog) {
    char charyLog[4096] = {0};
    SafeSprintf(charyLog, sizeof(charyLog), "[debug][%s]:%s", GetCurrentTimeString().c_str(), pStrLog);
    LogInQueue(LOG_WRITE_DEBUG, ::GetCurrentTimeTick(), pStrLog);
    return true;
}

bool LogSystem::Error(const char * pStrLog) {
    char charyLog[4096] = {0};
    SafeSprintf(charyLog, sizeof(charyLog), "[error][%s]:%s", GetCurrentTimeString().c_str(), pStrLog);
    LogInQueue(LOG_WRITE_ERROR, ::GetCurrentTimeTick(), pStrLog);
    return true;
}

bool LogSystem::EchoTrace(const char * pStrLog) {
    char charyLog[4096] = {0};
    SafeSprintf(charyLog, sizeof(charyLog), "[trace][%s]:%s", GetCurrentTimeString().c_str(), pStrLog);
    LogInQueue(LOG_ECHO_TRACE, ::GetCurrentTimeTick(), pStrLog);
    return true;
}

bool LogSystem::EchoError(const char * pStrLog) {
    char charyLog[4096] = {0};
    SafeSprintf(charyLog, sizeof(charyLog), "[error][%s]:%s", GetCurrentTimeString().c_str(), pStrLog);
    LogInQueue(LOG_ECHO_ERROR, ::GetCurrentTimeTick(), pStrLog);
    return true;
}

bool LogSystem::EchoDebug(const char * pStrLog) {
    char charyLog[4096] = {0};
    SafeSprintf(charyLog, sizeof(charyLog), "[debug][%s]:%s", GetCurrentTimeString().c_str(), pStrLog);
    LogInQueue(LOG_ECHO_DEBUG, ::GetCurrentTimeTick(), charyLog);
    return true;
}


bool LogSystem::LogInQueue(const u8 type, const u64 timetick, const char * log) {
    ThreadID id = ::GetCurrentThreadID();
    LOGQUEUE_MAP::iterator itor = m_logQueueMap.find(id);
    if (itor == m_logQueueMap.end()) {
        ILogQueue * pLogQueue = NEW LogQueue<4096>;
        m_logQueueMap.insert(make_pair(id, pLogQueue));
        itor = m_logQueueMap.find(id);
    }
    itor->second->In(type, timetick, log);
    return true;
}

THREAD_FUN LogSystem::WriteThread(LPVOID p) {
    LogSystem * pThis = (LogSystem *)p;
    Log log;
    while (true) {
        LOGQUEUE_MAP::iterator itor = pThis->m_logQueueMap.begin();
        LOGQUEUE_MAP::iterator iend = pThis->m_logQueueMap.end();
        while (itor != iend) {
            ILogQueue * pLogQueue = itor->second;
            ASSERT(pLogQueue != NULL);
            if (NULL == pLogQueue) {
                //这里要输出标准错误
            }
            while (pLogQueue->Out(log)) {
                pThis->WriteLog(log);
            }
            itor++;
        }
        if(pThis->m_Dead) {
            return NULL;
        }
        CSleep(1);
    }

    return true;
}

void LogSystem::StartLoop(bool demon) {
    if (demon) {
        HANDLE hThread = ::CreateThread(NULL, 0, LogSystem::WriteThread, (LPVOID)this, 0, NULL);
        CloseHandle(hThread);
    } else {
        WriteThread(this);
    }
}

const string LogSystem::GetLogFilePath(const u64 tick) {
    char path[256] = {0};
    string strtime = ::GetTimeString(tick);
    strtime = ::StringReplace(strtime.c_str(), ":", "_");
    SafeSprintf(path, sizeof(path), "%s/Log/%s_%s.log", GetAppPath(), m_strFileName.c_str(), strtime.c_str());
    return path;
}

bool LogSystem::GetLogFileHandle(const u64 nCurrentTick) {
    if (0 == m_nLastTick || !m_logfile.is_open()) {
        m_nLastTick = nCurrentTick;
        string path = GetLogFilePath(m_nLastTick);
        m_logfile.open(path.c_str(), std::ios::app);
        if (!m_logfile.is_open()) {
            ASSERT(false);
            return false;
        }
    } else if (nCurrentTick - m_nLastTick > m_nDuration * 1000) {
        m_nLastTick = nCurrentTick;
        if (m_logfile.is_open()) {
            m_logfile.close();
        }
        string path = GetLogFilePath(m_nLastTick);
        m_logfile.open(path.c_str(), std::ios::app);
        if (!m_logfile.is_open()) {
            ASSERT(false);
            return false;
        }
    } else if (m_nCurFilesize > m_nMaxFileSize * 1024 * 1024) {
        m_nLastTick = nCurrentTick;
        m_nCurFilesize = 0;
        if (m_logfile.is_open()) {
            m_logfile.close();
        }
        string path = GetLogFilePath(m_nLastTick);
        m_logfile.open(path.c_str(), std::ios::app);
        if (!m_logfile.is_open()) {
            ASSERT(false);
            return false;
        }
    }

    return true;
}

void LogSystem::WriteLog(Log & log) {
    if (!GetLogFileHandle(log.m_nTick)) {
        ASSERT(false);
        return;
    }
    if (log.m_nType == NOT_IN_USE) {
        //这里输出标准错误
        ASSERT(false);
        return;
    }
    switch (log.m_nType) {
    case LOG_ECHO_TRACE:
        ECHO_TRACE(log.m_strLog);
        break;
    case LOG_ECHO_DEBUG:
        ECHO_WARN(log.m_strLog);
        break;
    case LOG_ECHO_ERROR:
        ECHO_ERROR(log.m_strLog);
        break;
    case LOG_WRITE_TRACE:
        m_nCurFilesize += strlen(log.m_strLog);
        m_logfile << log.m_strLog << std::flush;
        break;
    case LOG_WRITE_DEBUG:
        m_nCurFilesize += strlen(log.m_strLog);
        m_logfile << log.m_strLog << std::flush;
        break;
    case LOG_WRITE_ERROR:
        m_nCurFilesize += strlen(log.m_strLog);
        m_logfile << log.m_strLog << std::flush;
        break;
    }


}

#endif //#if defined WIN32 || defined WIN64
