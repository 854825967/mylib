#ifndef EVENT_QUEUE_H
#define EVENT_QUEUE_H

#define QUEUE_SIZE 4096
#include "MultiSys.h"
extern "C" {
    static struct iocp_event ** s_pEQueue = NULL;
    static s32 s_read = 0;
    static s32 s_write = 0;

    void queue_init() {
        s_pEQueue = (iocp_event **)malloc(QUEUE_SIZE * sizeof(iocp_event *));
        memset(s_pEQueue, 0, QUEUE_SIZE * sizeof(iocp_event *));
    }

    void in_queue(struct iocp_event * pEvent) {
        
    }

    struct iocp_event * out_queue() {

    }
};

#endif //EVENT_QUEUE_H
