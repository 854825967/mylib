/**
 * @file    iocp_event.c
 * @brief   这是一个IOCP通知事件体的封装
 * @author  大T
 * @version 1.0
 * @date    2013.8.31
 * @bug     还未测试
 * @warning 还未测试
 */

#if defined WIN32 || defined WIN64

#include "iocp_event.h"
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

    typedef enum _bool {
        false = 0,
        true = !false,
    }bool;

#ifdef THREAD_SAFE
    static RTL_CRITICAL_SECTION s_critical;
#endif //THREAD_SAFE
    struct iocp_event s_szEvent[MACRO_EVENT_POOL_SIZE];
    s16 s_szPos[MACRO_EVENT_POOL_SIZE];
    s16 s_index = 0;

    void event_mgr_init() {
        static s8 s_init = 0;
        int i = 0;
        if (s_init) {
            return;
        }

        memset(s_szEvent, 0, sizeof(s_szEvent));
        for (i=0; i<MACRO_EVENT_POOL_SIZE; i++) {
            s_szPos[i] = i;
        }

#ifdef THREAD_SAFE
        InitializeCriticalSection(&s_critical);
#endif //THREAD_SAFE
        s_init = 1;
    }


    struct iocp_event * malloc_event() {
        struct iocp_event * pEvent = NULL;
#ifdef THREAD_SAFE
        EnterCriticalSection(&s_critical);
#endif //THREAD_SAFE
        if (s_index >= MACRO_EVENT_POOL_SIZE || -1 == s_szPos[s_index]) {
            ASSERT(false);
#ifdef THREAD_SAFE
            LeaveCriticalSection(&s_critical);
#endif //THREAD_SAFE
            return NULL;
        }

        pEvent = &s_szEvent[s_szPos[s_index]];
        s_szPos[s_index++] = -1;
#ifdef THREAD_SAFE
        LeaveCriticalSection(&s_critical);
#endif //THREAD_SAFE
        memset(pEvent, 0, sizeof(struct iocp_event));
        return pEvent;
    }


    void free_event(struct iocp_event * pEvent) {
        s16 pos = 0;
        if ( (char *)pEvent < (char *)s_szEvent
            || (char *)pEvent > (char *)(s_szEvent + MACRO_EVENT_POOL_SIZE - 1)
            || 0 != ((char *)pEvent - (char *)s_szEvent)%sizeof(struct iocp_event) ) {
                ASSERT(false);
                return;
        }
#ifdef THREAD_SAFE
        EnterCriticalSection(&s_critical);
#endif //THREAD_SAFE
        pos = ((char *)pEvent - (char *)s_szEvent)/sizeof(struct iocp_event);
        if (s_szPos[--s_index] != -1) {
            ASSERT(false);
#ifdef THREAD_SAFE
            LeaveCriticalSection(&s_critical);
#endif //THREAD_SAFE
            return;
        }

#ifdef _DEBUG
        {
            int i=0;
            for(i=0; i<MACRO_EVENT_POOL_SIZE; i++) {
                if (s_szPos[i] == pos) {
                    ASSERT(false);
                    return;
                }
            }
        }
#endif //_DEBUG

        s_szPos[s_index] = pos;
#ifdef THREAD_SAFE
        LeaveCriticalSection(&s_critical);
#endif //THREAD_SAFE
        return;
    }


#ifdef __cplusplus
};
#endif //__cplusplus

#endif //#if defined WIN32 || defined WIN64
