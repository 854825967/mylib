/** 
 * @file    iocp.h 
 * @brief   这是一个IOCP机制的封装
 * @author  大T
 * @version 1.0 
 * @date    2013.8.31 
 * @bug     还未测试 
 * @warning 还未测试 
 */  
#ifndef IOCP_H
#define IOCP_H

#include "iocp_event.h"

extern "C" {
    enum _code {
        ERROR_UNKNOWN,
        ERROR_NO_ERROR = 1,
        ERROR_GET_ACCEPTEX_FUN,
        ERROR_GET_CONNECTEX_FUN,
        ERROR_INIT_IOCP,
        ERROR_WSASOCKET_ERROR,
        ERROR_GET_EVENT,
        ERROR_GET_EVENT_TIME_OUT,
        ERROR_CONNECTEX_BIND,
        ERROR_LISTEN_BIND,
        ERROR_RELATE_IO_COMPLETIONPORT,
        ERROR_IP_FORMAT,
        ERROR_CONNECTEX,
        ERROR_ACCEPTEX,
        ERROR_SET_FIONBIO,
        ERROR_SET_SOCKET_OPT,
        ERROR_LISTEN,
        ERROR_MALLOC_EVENT,
        ERROR_WSARECV,
        ERROR_WSASEND,
        ERROR_GET_PEER_NAME,

        /** ERROR ADD BEFORE ERROR_CODE_MAX */
        ERROR_CODE_MAX
    }; /** 错误码 */

#define error_code enum _code

    /** 初始化IOCP */
    error_code iocp_init(s64 lWatiTime, OUT s32 * pnError);

    /** 异步连接 */
    error_code async_connect(const char * pStrIP, s32 nPort,  OUT s32 * pnError, void * pData);

    /** 异步侦听 */
    error_code async_listen(const char * pStrIP, s32 nPort, OUT s32 * pnError, void * pData, s32 backlog);

    /** 异步接收 */
    error_code async_recv(iocp_event * pEvent, OUT s32 * pnError, void * pData);

    /** 异步发送 */
    error_code async_send(iocp_event * pEvent, OUT s32 * pnError, void * pData);

    /** 设置单词io loop的时间, 毫秒为单位 */
    void set_waittime(s64 lMilliseconds); 

    /** 安全终止 socket io */
    void safe_shutdown(struct iocp_event * pEvent);

    /** 强制关闭 socket */
    void safe_close(struct iocp_event * pEvent);

    /** 获取io 时间 */
    error_code get_event(OUT struct iocp_event ** ppEvent, OUT s32 * pnError);
};

#endif //IOCP_H
