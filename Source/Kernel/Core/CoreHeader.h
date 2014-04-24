#ifndef COREHEADER_H
#define COREHEADER_H

#include "TLogger.h"
#include "CTimerEngine.h"
#include "CCallEngine.h"
#include "CHttpEngine.h"

#if defined WIN32 || defined WIN64
#include "Windows/CNet.h"
#endif //defined WIN32 || defined WIN64

namespace core {

    enum {
        LOG_CORE_TRACE = 0,
        LOG_CORE_DEBUG = 1,
        LOG_CORE_ERROR = 2,
        LOG_LOGIC = 3,
        //ADD BEFORE THIS
        LOG_CORE_COUNT
    };

    enum {
        CALL_MESSAGE = 0,
        CALL_GLOBAL_EVENT = 1,
        CALL_OBJECT_EVENT = 2,

        //ADD BEFORE THIS
        CALL_TYPE_COUNT
    };


    extern tlib::TLogger<LOG_CORE_COUNT> * g_pCoreLogger;
    extern ITimerEngine * g_pTimeEngine;
    extern INet * g_pNetEngine;
    extern ICallEngine<s8, EVENTCALLFUN, EVENTCALL_ARGS> * g_pEventCallEngine;
    extern IHttpEngine * g_pHttpEngine;

#define CORE_TRACE(format, ...) {\
    char _log[4096] = {0}; \
    memset(_log, 0, sizeof(_log)); \
    SafeSprintf(_log, sizeof(_log), format, ##__VA_ARGS__); \
    char __log[4096] = {0}; \
    SafeSprintf(__log, sizeof(__log), "[info] %s|%d|%s>>>%s\n", __FILE__, __LINE__, __FUNCTION__, _log); \
    g_pCoreLogger->Log(LOG_CORE_TRACE, __log); \
}

#define CORE_DEBUG(format, ...) {\
    char _log[4096] = {0}; \
    memset(_log, 0, sizeof(_log)); \
    SafeSprintf(_log, sizeof(_log), format, ##__VA_ARGS__); \
    char __log[4096] = {0}; \
    SafeSprintf(__log, sizeof(__log), "[info] %s|%d|%s>>>%s\n", __FILE__, __LINE__, __FUNCTION__, _log); \
    g_pCoreLogger->Log(LOG_CORE_DEBUG, __log); \
}

#define CORE_ERROR(format, ...) {\
    char _log[4096] = {0}; \
    memset(_log, 0, sizeof(_log)); \
    SafeSprintf(_log, sizeof(_log), format, ##__VA_ARGS__); \
    char __log[4096] = {0}; \
    SafeSprintf(__log, sizeof(__log), "[info] %s|%d|%s>>>%s\n", __FILE__, __LINE__, __FUNCTION__, _log); \
    g_pCoreLogger->Log(LOG_CORE_ERROR, __log); \
}

    void CoreHeaderInit();
    void CoreHeaderRelease();
}

#endif //COREHEADER_H
