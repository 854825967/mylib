/**
 * @file    iocp.h
 * @brief   这是一个IOCP机制的封装
 * @author  大T
 * @version 1.0
 * @date    2013.8.31
 * @bug     还未测试
 * @warning 还未测试
 */

 #if defined WIN32 || defined WIN64

#ifndef IOCP_H
#define IOCP_H

#include "iocp_event.h"
#include "Header.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

    /** 错误码 */
    enum _code {
        ERROR_UNKNOWN = 0,
        ERROR_NO_ERROR = 1,
        ERROR_GET_ACCEPTEX_FUN = 2,
        ERROR_GET_CONNECTEX_FUN = 3,
        ERROR_INIT_IOCP = 4,
        ERROR_WSASOCKET_ERROR = 5,
        ERROR_GET_EVENT = 6,
        ERROR_GET_EVENT_TIME_OUT = 7,
        ERROR_CONNECTEX_BIND = 8,
        ERROR_LISTEN_BIND = 9,
        ERROR_RELATE_IO_COMPLETIONPORT = 10,
        ERROR_IP_FORMAT = 11,
        ERROR_CONNECTEX = 12,
        ERROR_ACCEPTEX = 13,
        ERROR_SET_FIONBIO = 14,
        ERROR_SET_SOCKET_OPT = 15,
        ERROR_LISTEN = 16,
        ERROR_MALLOC_EVENT = 17,
        ERROR_WSARECV = 18,
        ERROR_WSASEND = 19,
        ERROR_GET_PEER_NAME = 20,

        /** ERROR ADD BEFORE ERROR_CODE_MAX */
        ERROR_CODE_MAX
    };

#define error_code enum _code

    /** 初始化IOCP */
    error_code iocp_init(s64 lWatiTime, OUT s32 * pnError);

    /** 异步连接 */
    error_code async_connect(const char * pStrIP, s32 nPort,  OUT s32 * pnError, void * pData);

    /** 异步侦听 */
    error_code async_listen(const char * pStrIP, s32 nPort, OUT s32 * pnError, void * pData, s32 backlog);

    /** 异步接收 */
    error_code async_recv(struct iocp_event * pEvent, OUT s32 * pnError, void * pData);

    /** 异步发送 */
    error_code async_send(struct iocp_event * pEvent, OUT s32 * pnError, void * pData);

    /** 设置单词io loop的时间, 毫秒为单位 */
    void set_waittime(s64 lMilliseconds);

    /** 安全终止 socket io */
    void safe_shutdown(struct iocp_event * pEvent);

    /** 强制关闭 socket */
    void safe_close(struct iocp_event * pEvent);

    /** 获取io 时间 */
    error_code get_event(OUT struct iocp_event ** ppEvent, OUT s32 * pnError);

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //IOCP_H

#endif //#if defined WIN32 || defined WIN64
