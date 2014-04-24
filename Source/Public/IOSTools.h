//
//  IOSTools.h
//  HelloCpp
//
//  Created by Alax on 14-3-21.
//
//

#ifndef IOSTools_h
#define IOSTools_h
#if defined __IPHONE_OS_VERSION_MAX_ALLOWED || defined IS_IPAD
#include "MultiSys.h"
#include <string>
#include <unistd.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <pthread.h>
#include <errno.h>
#include <netdb.h>

inline ThreadID GetCurrentThreadID() {

    return 0;
}

inline std::string GetHostIp(const char * pUrl){
    struct hostent *hp;
    struct in_addr in;
    hp = gethostbyname(pUrl);
    memcpy (&in.s_addr,hp->h_addr,4);
    return inet_ntoa(in);
}

inline u64 GetCurrentTimeTick() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec/1000;
}

inline s64 GetTimeTickOfDayBeginning() {
    time_t curTime = time(NULL);
    tm localTm = *localtime(&curTime);
    localTm.tm_hour = 0;
    localTm.tm_min = 0;
    localTm.tm_sec = 0;
    return mktime(&localTm) * 1000;
}

inline std::string GetCurrentTimeString(const char * format = "%4d-%02d-%02d %02d:%02d:%02d") {
    struct tm * ptm = NULL;
    time_t t = time(NULL);
    ptm = localtime(&t);
    char temp[64] = {0};
    strftime(temp, sizeof(temp), format, ptm);
    return std::string(temp);
}

inline std::string GetTimeString(const s64 tick, const char * format = "%4d-%02d-%02d %02d:%02d:%02d") {
    struct tm * ptm = NULL;
    time_t t = (time_t) tick;
    ptm = localtime(&t);
    char temp[64] = {0};
    strftime(temp, sizeof(temp), format, ptm);
    return std::string(temp);
}

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
    
    const char * GetAppPath() {
        return "./";
    }
    
    void CopyString(char * buf, size_t len, const char * str)
    {
        const size_t SIZE = ::strlen(str) + 1;
        
        if (SIZE <= len) {
            ::memcpy(buf, str, SIZE);
        } else {
            ::memcpy(buf, str, len - 1);
            buf[len - 1] = 0;
        }
    }
    
#ifdef __cplusplus
};
#endif //__cplusplus

inline u32 StringAsInt(const char * value) {
    ASSERT(value);
    return atoi(value);
}

inline std::string IntAsString(const u32 value) {
    char str[32];
    memset(str, 0, sizeof(str));
    SafeSprintf(str, sizeof(str), "%d", value);
    return str;
}

#endif //defined IS_IPHONE || defined IS_IPAD

#endif
