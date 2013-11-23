/**
 * @file    cepoll_event.c
 * @brief   这是一个epoll通知事件体的封装
 * @author  大T
 * @version 1.0
 * @date    2013.8.31
 * @bug     还未测试
 * @warning 还未测试
 */
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus
#include "cepoll_event.h"

    typedef enum _bool {
        false = 0,
        true = !false,
    }bool;

#ifdef THREAD_SAFE
    static pthread_mutex_t s_mutex;
#endif //THREAD_SAFE
    static struct cepoll_event s_szEvent[MACRO_EVENT_POOL_SIZE];
    static s16 s_szPos[MACRO_EVENT_POOL_SIZE];
    static s16 s_index = 0;

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
        pthread_mutex_init(&s_mutex, NULL);// = PTHREAD_MUTEX_INITIALIZER;
#endif //THREAD_SAFE
        s_init = 1;
    }


    struct cepoll_event * malloc_event() {
        struct cepoll_event * pEvent = NULL;
#ifdef THREAD_SAFE
        pthread_mutex_lock(&s_mutex);
#endif //THREAD_SAFE
        if (s_index >= MACRO_EVENT_POOL_SIZE || -1 == s_szPos[s_index]) {
            assert(false);
#ifdef THREAD_SAFE
            pthread_mutex_unlock(&s_mutex);
#endif //THREAD_SAFE
            return NULL;
        }

        pEvent = &s_szEvent[s_szPos[s_index]];
        s_szPos[s_index++] = -1;
#ifdef THREAD_SAFE
        pthread_mutex_unlock(&s_mutex);
#endif //THREAD_SAFE
        memset(pEvent, 0, sizeof(struct cepoll_event));
        return pEvent;
    }


    void free_event(struct cepoll_event * pEvent) {
        s16 pos = 0;
        if ( (char *)pEvent < (char *)s_szEvent
            || (char *)pEvent > (char *)(s_szEvent + MACRO_EVENT_POOL_SIZE - 1)
            || 0 != ((char *)pEvent - (char *)s_szEvent)%sizeof(struct cepoll_event) ) {
                assert(0);
                return;
        }
#ifdef THREAD_SAFE
        pthread_mutex_lock(&s_mutex);
#endif //THREAD_SAFE
        pos = ((char *)pEvent - (char *)s_szEvent)/sizeof(struct cepoll_event);
        if (s_szPos[--s_index] != -1) {
            assert(0);
#ifdef THREAD_SAFE
            pthread_mutex_unlock(&s_mutex);
#endif //THREAD_SAFE
            return;
        }

#ifdef _DEBUG
        {
            int i=0;
            for(i=0; i<MACRO_EVENT_POOL_SIZE; i++) {
                if (s_szPos[i] == pos) {
                    assert(0);
                    return;
                }
            }
        }
#endif //_DEBUG

        s_szPos[s_index] = pos;
#ifdef THREAD_SAFE
        pthread_mutex_unlock(&s_mutex);
#endif //THREAD_SAFE
        return;
    }


#ifdef __cplusplus
};
#endif //__cplusplus

