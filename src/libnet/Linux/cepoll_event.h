/**
 * @file    epoll_event.h
 * @brief   这是一个epoll通知事件体的封装
 * @author  大T
 * @version 1.0
 * @date    2013.8.31
 * @bug     还未测试
 * @warning 还未测试
 */

#ifndef CEPOLL_EVENT_H
#define CEPOLL_EVENT_H

#include "MultiSys.h"


#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#define THREAD_SAFE
#define MACRO_EVENT_POOL_SIZE 8192
    /** IO事件类型 */
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
     * 事件体
     */
    struct cepoll_event {
        s32 s;   /** socket */
        struct sockaddr_in remote;
        s8 event;   /** 事件类型 */
        s32 nerron; /** 错误码 */
        void * p; /** 上下文指针 */
    };

    /** 初始化事件管理器 */
    void event_mgr_init();
    /** 分配一个事件体 */
    struct cepoll_event * malloc_event();
    /** 回收一个事件体 */
    void free_event(struct cepoll_event * pEvent);

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //CEPOLL_EVENT_H
