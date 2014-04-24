//
//  IosSys.h
//  HelloCpp
//
//  Created by Alax on 14-3-21.
//
//
#ifndef IosSys_h
#define IosSys_h

#ifdef CC_TARGET_OS_IPHONE

#include <unistd.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long int u64;

typedef char s8;
typedef short s16;
typedef int s32;
typedef long long int s64;
#undef CSleep
#define CSleep(n) usleep(n*1000)
#define SafeSprintf snprintf

#define SD_RECEIVE SHUT_RD
#define SD_SEND SHUT_WR
#define SD_BOTH SHUT_RDWR
#define closesocket close

typedef void * THREAD_FUN;
typedef void * LPVOID;
typedef pthread_t ThreadID;

#define ECHO(format, a...) {\
    char log[4096] = {0}; \
    SafeSprintf(log, 4096, format, ##a); \
    printf("%s\n", log); \
}

#define ECHO_TRACE(format, a...) {\
    char log[4096] = {0}; \
    SafeSprintf(log, 4096, format, ##a); \
    printf("[info] %s|%d|%s\n\t%s\n", __FILE__, __LINE__, __FUNCTION__, log); \
}

#define ECHO_WARN(format, a...) {\
    char log[4096] = {0}; \
    SafeSprintf(log, 4096, format, ##a); \
    printf("[warn]%s|%d|%s\n\t%s\n", __FILE__, __LINE__, __FUNCTION__, log); \
}

#define ECHO_ERROR(format, a...) {\
    char log[4096] = {0}; \
    SafeSprintf(log, 4096, format, ##a); \
    printf("[error]%s|%d|%s\n\t%s\n", __FILE__, __LINE__, __FUNCTION__, log); \
}

#endif // CC_TARGET_OS_IPHONE

#endif //IosSys_h
