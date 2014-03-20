#ifndef COREHEADER_H
#define COREHEADER_H

#include "TLogger.h"

enum {
    CORELOG_TRACE = 0,
    CORELOG_DEBUG = 1,
    CORELOG_ERROR = 2,

    //ADD BEFORE THIE
    CORELOG_COUNT
};


extern tlib::TLogger<CORELOG_COUNT> * g_pCoreLogger;

#define CORE_TRACE(format, ...) {\
    char _log[4096] = {0}; \
    memset(_log, 0, sizeof(_log)); \
    SafeSprintf(_log, sizeof(_log), format, ##__VA_ARGS__); \
    g_pCoreLogger->Log(CORELOG_TRACE, _log); \
}

#define CORE_DEBUG(format, ...) {\
    char _log[4096] = {0}; \
    memset(_log, 0, sizeof(_log)); \
    SafeSprintf(_log, sizeof(_log), format, ##__VA_ARGS__); \
    g_pCoreLogger->Log(CORELOG_DEBUG, _log); \
}

#define CORE_ERROR(format, ...) {\
    char _log[4096] = {0}; \
    memset(_log, 0, sizeof(_log)); \
    SafeSprintf(_log, sizeof(_log), format, ##__VA_ARGS__); \
    g_pCoreLogger->Log(CORELOG_ERROR, _log); \
}

namespace core {
    void CoreHeaderInit();
    void CoreHeaderRelease();
}

#endif //COREHEADER_H
