#ifdef linux
#ifndef LINUXSYS_H
#define LINUXSYS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <tr1/unordered_map>
using namespace std::tr1;

#define CHashMap unordered_map
#define CSleep(n) usleep((n) * 1000)
#define SafeSprintf snprintf

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
    ASSERT(false); \
    }

    typedef unsigned char u8;
    typedef unsigned short u16;
    typedef uint32_t u32;
    typedef uint64_t u64;

    typedef char s8;
    typedef short s16;
    typedef int32_t s32;
    typedef int64_t s64;

#endif  //LINUXSYS_H
#endif //#ifdef linux
