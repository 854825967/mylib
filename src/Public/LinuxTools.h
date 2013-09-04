#ifdef linux
#ifndef LINUXTOOLS_H
#define LINUXTOOLS_H
#include "MultiSys.h"
#include <unistd.h>
#include <libgen.h>
#include <linux/unistd.h>
#include <linux/types.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <pthread.h>
#include <errno.h>


#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

    const char * GetAppPath() {
        static char * pStrPath = NULL;
        if (NULL == pStrPath) {
            pStrPath = NEW char[1024];
            char link[1024];
            memset(link, 0, sizeof(link));
            SafeSprintf(link, sizeof(link), "/proc/self/exe", getpid());
            s32 nCount = readlink(link, pStrPath, sizeof(link));
            if (nCount >= sizeof(link)) {
                ASSERT(false);
            }
            pStrPath = dirname(pStrPath);
        }

        return pStrPath;
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

#endif //LINUXTOOLS_H
#endif //#ifdef linux
