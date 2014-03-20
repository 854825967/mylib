#ifndef HEADER_H
#define HEADER_H

#include "MultiSys.h"

enum {
    LOG_TRACE = 0,
    LOG_DEBUG = 1,
    LOG_ERROR = 2,

    //add before this
    LOG_FILE_COUNT
};

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
    
    void LogModuleInit();

    void LogModuleStop();

    void Trace(const char * log);

    void Debug(const char * log);

    void Error(const char * log);

#define LOG_TRACE(format, ...) {\
    char _log[4096] = {0}; \
    memset(_log, 0, sizeof(_log)); \
    SafeSprintf(_log, sizeof(_log), format, ##__VA_ARGS__); \
    Trace(_log); \
    }

#define LOG_DEBUG(format, ...) {\
    char _log[4096] = {0}; \
    memset(_log, 0, sizeof(_log)); \
    SafeSprintf(_log, sizeof(_log), format, ##__VA_ARGS__); \
    Debug(_log); \
    }

#define LOG_ERROR(format, ...) {\
    char _log[4096] = {0}; \
    memset(_log, 0, sizeof(_log)); \
    SafeSprintf(_log, sizeof(_log), format, ##__VA_ARGS__); \
    Error(_log); \
    }

#ifdef __cplusplus
};
#endif //__cplusplus
#endif //HEADER_H
