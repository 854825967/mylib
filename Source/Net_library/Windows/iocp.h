/**
 * @file    iocp.h
 * @brief   ����һ��IOCP���Ƶķ�װ
 * @author  ��T
 * @version 1.0
 * @date    2013.8.31
 * @bug     ��δ����
 * @warning ��δ����
 */

 #if defined WIN32 || defined WIN64

#ifndef IOCP_H
#define IOCP_H

#include "iocp_event.h"
#include "Header.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

    /** ������ */
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

    /** ��ʼ��IOCP */
    error_code iocp_init(s64 lWatiTime, OUT s32 * pnError);

    /** �첽���� */
    error_code async_connect(const char * pStrIP, s32 nPort,  OUT s32 * pnError, void * pData);

    /** �첽���� */
    error_code async_listen(const char * pStrIP, s32 nPort, OUT s32 * pnError, void * pData, s32 backlog);

    /** �첽���� */
    error_code async_recv(struct iocp_event * pEvent, OUT s32 * pnError, void * pData);

    /** �첽���� */
    error_code async_send(struct iocp_event * pEvent, OUT s32 * pnError, void * pData);

    /** ���õ���io loop��ʱ��, ����Ϊ��λ */
    void set_waittime(s64 lMilliseconds);

    /** ��ȫ��ֹ socket io */
    void safe_shutdown(struct iocp_event * pEvent);

    /** ǿ�ƹر� socket */
    void safe_close(struct iocp_event * pEvent);

    /** ��ȡio ʱ�� */
    error_code get_event(OUT struct iocp_event ** ppEvent, OUT s32 * pnError);

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //IOCP_H

#endif //#if defined WIN32 || defined WIN64
