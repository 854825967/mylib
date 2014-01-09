#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#include "cepoll.h"
    //声明epoll句柄
    static s32 s_epfd;
    //声明cepoll_event结构体的变量,ev用于注册事件,数组用于回传要处理的事件
    static struct cepoll_event s_events[EVENT_QUENE_SIZE];
    //单次epoll_wait的超时时长
    static s64 s_waittime;

#define RETURN_RES(code) {\
    *pnError = errno; \
    return code;}

    /** 初始化epoll机制 */
    error_code cepoll_init(s64 ms, s32 * pnError) {
        errno = 0;
        s_epfd = epoll_create(MAX_FDS);
        if (0 == s_epfd) {
            RETURN_RES(ERROR_CREATE_EPOLL);
        }
        RETURN_RES(ERROR_NO_ERROR);
    }

    /** 异步连接 */
    error_code async_connect(const char * pStrIP, s32 nPort, OUT s32 * pnError, void * pData) {
        struct epoll_event e;
        struct cepoll_event * pEvent = NULL;
        s32 s = socket(AF_INET, SOCK_STREAM, 0);
        errno = 0;
        if (-1 == fcntl(s, F_SETFL, fcntl(s, F_GETFL) | O_NONBLOCK)) {
            RETURN_RES(ERROR_FCNTL);
        }

        pEvent = malloc_event();
        if (NULL == pEvent) {
            //assert(false);
            ECHO_ERROR("%s", "get cepoll_event error");
            RETURN_RES(ERROR_MALLOC_EVENT);
        }
        pEvent->s = s;
        pEvent->p = pData;
        pEvent->event = EVENT_ASYNC_CONNECT;


        e.events = EPOLLOUT | EPOLLET;
        e.data.ptr = pEvent;
        epoll_ctl(s_epfd, EPOLL_CTL_ADD, s, &e);

        if((pEvent->remote.sin_addr.s_addr = inet_addr(pStrIP)) == INADDR_NONE) {
            free_event(pEvent);
            RETURN_RES(ERROR_IP_FORMAT);
        }

        *pnError = connect(s, (struct sockaddr*)&pEvent->remote, sizeof(pEvent->remote));
        /*
        getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);
        if( !error )
        {
        //OK
        }
        */
        *pnError = 100;
        return ERROR_CODE_MAX;
    }

    error_code async_listen(const char * pStrIP, s32 nPort, OUT s32 * pnError, void * pData, s32 backlog) {
        ASSERT(pStrIP != NULL && pnError != NULL);
        errno = 0;
        struct epoll_event e;
        struct cepoll_event * pEvent = NULL;
        s32 s = socket(AF_INET, SOCK_STREAM, 0);
        if (-1 == (s = socket(AF_INET, SOCK_STREAM, 0))) {
            ECHO_ERROR("socket error %d", errno);
            return false;
        }
        //把socket设置为非阻塞方式
        if (!Setnonblocking(s)) {
            ECHO_ERROR("Setnonblocking error %d", errno);
            return false;
        }

        pEvent = malloc_event();
        if (NULL == pEvent) {
            //assert(false);
            ECHO_ERROR("%s", "get cepoll_event error");
            RETURN_RES(ERROR_MALLOC_EVENT);
        }

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof (addr));
        addr.sin_family = AF_INET;
        inet_aton(pStrIp, &(addr.sin_addr));
        addr.sin_port = htons(nPort);

        if (-1 == bind(s, (sockaddr *) & addr, sizeof (addr)) ||
                -1 == (listen(s, LISTEN_QUENE))) {
            Assert(false);
            return false;
        }

        struct epoll_event ev;
        ev.data.ptr = pListen;
        ev.events = EPOLLIN; // | EPOLLET;
        epoll_ctl(m_epfd, EPOLL_CTL_ADD, listenfd, &ev);

        return true;
    }

#ifdef __cplusplus
};
#endif //__cplusplus
