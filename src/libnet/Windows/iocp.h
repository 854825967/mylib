/** 
 * @file    iocp.h 
 * @brief   ����һ��IOCP���Ƶķ�װ
 * @author  ��T
 * @version 1.0 
 * @date    2013.8.31 
 * @bug     ��δ���� 
 * @warning ��δ���� 
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
    }; /** ������ */

#define error_code enum _code

    /** ��ʼ��IOCP */
    error_code iocp_init(s64 lWatiTime, OUT s32 * pnError);

    /** �첽���� */
    error_code async_connect(const char * pStrIP, s32 nPort,  OUT s32 * pnError, void * pData);

    /** �첽���� */
    error_code async_listen(const char * pStrIP, s32 nPort, OUT s32 * pnError, void * pData, s32 backlog);

    /** �첽���� */
    error_code async_recv(iocp_event * pEvent, OUT s32 * pnError, void * pData);

    /** �첽���� */
    error_code async_send(iocp_event * pEvent, OUT s32 * pnError, void * pData);

    /** ���õ���io loop��ʱ��, ����Ϊ��λ */
    void set_waittime(s64 lMilliseconds); 

    /** ��ȫ��ֹ socket io */
    void safe_shutdown(struct iocp_event * pEvent);

    /** ǿ�ƹر� socket */
    void safe_close(struct iocp_event * pEvent);

    /** ��ȡio ʱ�� */
    error_code get_event(OUT struct iocp_event ** ppEvent, OUT s32 * pnError);
};

#endif //IOCP_H
