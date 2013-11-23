/* 
 * File:   ILogSystem.h
 * Author: traveler
 *
 * Created on January 3, 2013, 1:25 PM
 */

#ifndef ILOGSYSTEM_H
#define ILOGSYSTEM_H

class ILog;

typedef ILog* (*FUN_GET_LOG)(const char *, const u32, const u32);

#define NAME_OF_GET_LOG_FUN "GetLog"
#define GET_LOG_FUN GetLog

typedef void (*FUN_RETURN_LOG)(ILog *);

#define NAME_OF_RETURN_LOG_FUN "ReturnLog"
#define RETURN_LOG_FUN ReturnLog

#if defined WIN32 || defined WIN64
#define LOG_TRACE(pLog, format, ...) \
{ \
    char content[3072] = {0}; \
    char log[4096] = {0}; \
    SafeSprintf(content, sizeof(content), format, ##__VA_ARGS__); \
    SafeSprintf(log, sizeof(log), "[trace][%s][%s:%d]>>%s\n", ::GetCurrentTimeString().c_str(), __FILE__, __LINE__, content); \
    pLog->Trace(log); \
}

#define LOG_DEBUG(pLog, format, ...) \
{ \
    char content[3072] = {0}; \
    char log[4096] = {0}; \
    SafeSprintf(content, sizeof(content), format, ##__VA_ARGS__); \
    SafeSprintf(log, sizeof(log), "[debug][%s][%s:%d]>>%s\n", ::GetCurrentTimeString().c_str(), __FILE__, __LINE__, content); \
    pLog->Debug(log); \
}

#define LOG_ERROR(pLog, format, ...) \
{ \
    char content[3072] = {0}; \
    char log[4096] = {0}; \
    SafeSprintf(content, sizeof(content), format, ##__VA_ARGS__); \
    SafeSprintf(log, sizeof(log), "[error][%s][%s:%d]>>%s\n", ::GetCurrentTimeString().c_str(), __FILE__, __LINE__, content); \
    pLog->Error(log); \
}
#endif //defined WIN32 || defined WIN64

class ILog {
public:
    virtual bool EchoSwitch(bool echoswitch = true) = 0;
    virtual bool Trace(const char * log) = 0;
    virtual bool Debug(const char * log) = 0;
    virtual bool Error(const char * log) = 0;
    virtual bool EchoTrace(const char * log) = 0;
    virtual bool EchoError(const char * log) = 0;
    virtual bool EchoDebug(const char * log) = 0;
    virtual void StartLoop(bool demon = false) = 0;
};

inline ILog * GetLogWorker(const char * dllPath, const char * filename, const u32 duration = 3600, const u32 maxsize = 50) {
    ILog * pLogCore = NULL;
#if defined WIN32 || defined WIN64
    HINSTANCE hinst = ::LoadLibrary(dllPath);
    if (NULL == hinst) {
        ECHO_ERROR("LoadLibrary error, code : %d", GetLastError());
        ASSERT(hinst != NULL);
        return NULL;
    }

    FUN_GET_LOG pFunGetLog = (FUN_GET_LOG)::GetProcAddress(hinst, NAME_OF_GET_LOG_FUN);
    ASSERT(pFunGetLog != NULL);
    if (NULL == pFunGetLog) {
        return NULL;
    }

    pLogCore = pFunGetLog(filename, duration, maxsize);
    ASSERT(pLogCore != NULL);
#endif // WIN32 || WIN64
    return pLogCore;
}

#endif //ILOGSYSTEM_H