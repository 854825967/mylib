/** 
 * @file    iocp_event.h 
 * @brief   ����һ��IOCP֪ͨ�¼���ķ�װ
 * @author  ��T
 * @version 1.0 
 * @date    2013.8.31 
 * @bug     ��δ���� 
 * @warning ��δ���� 
 */  


#if defined WIN32 || defined WIN64

#ifndef IOCP_EVENT_H
#define IOCP_EVENT_H

#include "MultiSys.h"
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#define THREAD_SAFE
#define MACRO_EVENT_POOL_SIZE 8192
    /** IO�¼����� */
    enum event_type {
        EVENT_ASYNC_CONNECT,
        EVENT_ASYNC_ACCEPT,
        EVENT_ASYNC_RECV,
        EVENT_ASYNC_SEND,

        //ADD EVENT TYPE BEFORE HERE
        EVENT_TYPE_COUT,
        EVENT_TYPE_NO_USE = EVENT_TYPE_COUT
    };

    /**  
     * �¼���
     */ 
    struct iocp_event {
        OVERLAPPED ol;  /** ������,�ó�Ա����������ڵ�һλ */
        SOCKET s;   /** socket */
        struct sockaddr_in remote;
        s8 event;   /** �¼����� */
        s32 nerron; /** ������ */
        WSABUF wbuf; /** ��д�������ṹ����� */
        DWORD ioBytes, dwBytes, dwFlags; /** һЩ�ڶ�дʱ�õ��ı�־�Ա��� */
        char buff[BUFF_SIZE]; /** �Լ��Ļ����� */
        void * p; /** ������ָ�� */
    };

    /** ��ʼ���¼������� */
    void event_mgr_init();
    /** ����һ���¼��� */
    struct iocp_event * malloc_event();
    /** ����һ���¼��� */
    void free_event(struct iocp_event * pEvent);

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //IOCP_EVENT_H

#endif //#if defined WIN32 || defined WIN64
