#ifndef CEPOLL_H
#define CEPOLL_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#include "cepoll_event.h"

#define EVENT_QUENE_SIZE 4096
#define MAX_FDS 4096
    enum _code {
        ERROR_UNKNOWN,
        ERROR_NO_ERROR = 1,
        ERROR_CREATE_EPOLL,
        ERROR_FCNTL,
        ERROR_MALLOC_EVENT,
        ERROR_IP_FORMAT,

        /** ERROR ADD BEFORE ERROR_CODE_MAX */
        ERROR_CODE_MAX
    };

#define error_code enum _code

    /** 初始化epoll机制 */
    error_code cepoll_init(s64 ms, s32 * pnError);

    /** 异步连接 */
    error_code async_connect(const char * pStrIP, s32 nPort, OUT s32 * pnError, void * pData);

    /** 异步侦听 */
    error_code async_listen(const char * pStrIP, s32 nPort, OUT s32 * pnError, void * pData, s32 backlog);

    /** 异步接收 */
    error_code async_recv(struct epoll_event * pEvent, OUT s32 * pnError, void * pData);

    /** 异步发送 */
    error_code async_send(struct epoll_event * pEvent, OUT s32 * pnError, void * pData);

    /** 设置单词io loop的时间, 毫秒为单位 */
    void set_waittime(s64 lMilliseconds);

    /** 安全终止 socket io */
    void safe_shutdown(struct epoll_event * pEvent);

    /** 强制关闭 socket */
    void safe_close(struct epoll_event * pEvent);

    /** 获取io 时间 */
    error_code get_event(OUT struct epoll_event ** ppEvent, OUT s32 * pnCount, OUT s32 * pnError);


#ifdef __cplusplus
};
#endif //__cplusplus
#endif //CEPOLL_H
