#if defined WIN32 || defined WIN64
#include "iocp.h"
#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

    typedef enum _bool {
        false = 0,
        true = !false,
    }bool;

#define RETURN_RES(code) {*pnError = GetLastError();return code;}

    LPFN_ACCEPTEX s_pFunAcceptEx = NULL;
    LPFN_CONNECTEX s_pFunConnectEx = NULL;
    u64 s_lWaittiem = 10;
    HANDLE s_hCompletionPort = NULL;
    bool s_bInit = false;

    LPFN_ACCEPTEX GetAcceptExFun() {
        GUID GuidAcceptEx = WSAID_ACCEPTEX;
        DWORD dwBytes = 0;
        SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        LPFN_ACCEPTEX pAcceptFun = NULL;
        WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx,
            sizeof(GuidAcceptEx), &pAcceptFun, sizeof(pAcceptFun),
            &dwBytes, NULL, NULL);

        if (NULL == pAcceptFun) {
            s32 nError = WSAGetLastError();
            NET_ERROR("Get AcceptEx fun error, error code : %d", nError);
            ASSERT(false);
        }

        return pAcceptFun;
    }

    LPFN_CONNECTEX GetConnectExFun() {
        GUID GuidConnectEx = WSAID_CONNECTEX;
        DWORD dwBytes = 0;
        SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        LPFN_CONNECTEX pConnectFun = NULL;
        WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidConnectEx,
            sizeof(GuidConnectEx), &pConnectFun, sizeof(pConnectFun),
            &dwBytes, NULL, NULL);

        if (NULL == pConnectFun) {
            NET_ERROR("Get ConnectEx fun error, error code : %d", WSAGetLastError());
            ASSERT(false);
        }

        return pConnectFun;
    }

    error_code iocp_init(s64 lWatiTime, OUT s32 * pnError) {
        SetLastError(0);
        if (NULL == (s_pFunAcceptEx = GetAcceptExFun())) {
            RETURN_RES(ERROR_GET_ACCEPTEX_FUN);
        }
        if (NULL == (s_pFunConnectEx = GetConnectExFun())) {
            RETURN_RES(ERROR_GET_CONNECTEX_FUN);
        }
        s_lWaittiem = lWatiTime;
        if ((s_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0)) == NULL) {
            RETURN_RES(ERROR_INIT_IOCP);
        } else {
            s_bInit = true;
        }
        event_mgr_init();
        RETURN_RES(ERROR_NO_ERROR);
    }

    void iocp_stop() {
        if (s_bInit) {
            s_bInit = false;
            CloseHandle(s_hCompletionPort);
        }
        return;
    }

    error_code async_connect(const char * pStrIP, s32 nPort, OUT s32 * pnError, void * pData) {
        s32 s = INVALID_SOCKET;
        struct sockaddr_in addr;
        DWORD dwValue = 0;
        struct iocp_event * pEvent = NULL;
        s32 res, err;
        SetLastError(0);
        if (!s_bInit) {
            RETURN_RES(ERROR_INIT_IOCP);
        }

        if (INVALID_SOCKET == (s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED)) ) {
            RETURN_RES(ERROR_WSASOCKET_ERROR);
        }

        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        if (SOCKET_ERROR == bind(s, (struct sockaddr *)&addr, sizeof(struct sockaddr_in))) {
            RETURN_RES(ERROR_CONNECTEX_BIND);
        }

        if (SOCKET_ERROR == ioctlsocket(s, FIONBIO, &dwValue)) {
            closesocket(s);
            RETURN_RES(ERROR_SET_FIONBIO);
        }

        if (s_hCompletionPort != CreateIoCompletionPort((HANDLE)s, s_hCompletionPort, (u_long)s, 0)) {
            closesocket(s);
            RETURN_RES(ERROR_RELATE_IO_COMPLETIONPORT);
        }

        pEvent = malloc_event();
        pEvent->s = s;
        pEvent->p = pData;
        pEvent->event = EVENT_ASYNC_CONNECT;
        pEvent->remote.sin_family = AF_INET;
        if((pEvent->remote.sin_addr.s_addr = inet_addr(pStrIP)) == INADDR_NONE) {
            free_event(pEvent);
            RETURN_RES(ERROR_IP_FORMAT);
        }
        pEvent->remote.sin_port = htons(nPort);

        res = s_pFunConnectEx(
            s,
            (struct sockaddr *)&pEvent->remote,
            sizeof(struct sockaddr_in),
            NULL,
            0,
            &pEvent->dwBytes,
            (LPOVERLAPPED)pEvent
        );

        err = WSAGetLastError();
        if (res == FALSE && err != WSA_IO_PENDING) {
            closesocket(s);
            free_event(pEvent);
            RETURN_RES(ERROR_CONNECTEX);
        }

        RETURN_RES(ERROR_NO_ERROR);
    }

    error_code async_listen(const char * pStrIP, s32 nPort, OUT s32 * pnError, void * pData, s32 backlog) {
        SOCKET s = INVALID_SOCKET;
        int len = BUFF_SIZE;
        struct sockaddr_in addr;
        struct iocp_event * pEvent = NULL;
        s32 res, err;

        SetLastError(0);
        if (!s_bInit) {
            RETURN_RES(ERROR_INIT_IOCP);
        }

        if (INVALID_SOCKET == (s = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED))) {
            RETURN_RES(ERROR_WSASOCKET_ERROR);
        }

        if (SOCKET_ERROR == setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char *)&len, sizeof(int))
            || SOCKET_ERROR == setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char *)&len, sizeof(int))) {
                closesocket(s);
                RETURN_RES(ERROR_SET_SOCKET_OPT);
        }

        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        if(INADDR_NONE == (addr.sin_addr.s_addr = inet_addr(pStrIP))) {
            RETURN_RES(ERROR_IP_FORMAT);
        }
        addr.sin_port = htons(nPort);
        if (SOCKET_ERROR == bind(s, (struct sockaddr*)&(addr), sizeof(struct sockaddr_in))) {
            RETURN_RES(ERROR_LISTEN_BIND);
        }
        if (listen(s, backlog) == SOCKET_ERROR) {
            RETURN_RES(ERROR_LISTEN);
        }

        if (s_hCompletionPort != (CreateIoCompletionPort((HANDLE)s, (HANDLE)s_hCompletionPort, (u_long)s, 0))) {
            RETURN_RES(ERROR_RELATE_IO_COMPLETIONPORT);
        }

        pEvent = malloc_event();
        if (NULL == pEvent) {
            ASSERT(false);
            RETURN_RES(ERROR_MALLOC_EVENT);
        }
        pEvent->p = pData;
        pEvent->event = EVENT_ASYNC_ACCEPT;

        if (INVALID_SOCKET == (pEvent->s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED))) {
            RETURN_RES(ERROR_WSASOCKET_ERROR);
        }

        res = s_pFunAcceptEx(
            s,
            pEvent->s,
            pEvent->buff,
            0,
            sizeof(struct sockaddr_in) + 16,
            sizeof(struct sockaddr_in) + 16,
            &pEvent->dwBytes,
            (LPOVERLAPPED)pEvent
            );

        err = WSAGetLastError();
        if (res == FALSE && err != WSA_IO_PENDING) {
            RETURN_RES(ERROR_ACCEPTEX);
        }

        RETURN_RES(ERROR_NO_ERROR);
    }

    error_code async_recv(struct iocp_event * pEvent, OUT s32 * pnError, void * pData) {
        SOCKET s;
        SetLastError(0);
        if (!s_bInit) {
            RETURN_RES(ERROR_INIT_IOCP);
        }

        s = pEvent->s;
        memset(pEvent, 0, sizeof(struct iocp_event));
        pEvent->wbuf.buf = pEvent->buff;
        pEvent->wbuf.len = sizeof(pEvent->buff);
        pEvent->s = s;
        pEvent->p = pData;
        pEvent->event = EVENT_ASYNC_RECV;

        if (SOCKET_ERROR == WSARecv(pEvent->s, &pEvent->wbuf, 1, &pEvent->dwBytes, &pEvent->dwFlags, (LPWSAOVERLAPPED)pEvent, NULL)) {
            *pnError = GetLastError();
            if (ERROR_IO_PENDING == *pnError) {
                return ERROR_NO_ERROR;
            } else {
                return ERROR_WSARECV;
            }
        }

        RETURN_RES(ERROR_NO_ERROR);
    }

    error_code async_send(struct iocp_event * pEvent, OUT s32 * pnError, void * pData) {
        s32 res;
        SetLastError(0);
        if (!s_bInit) {
            RETURN_RES(ERROR_INIT_IOCP);
        }
        pEvent->p = pData;

        res = WSASend(pEvent->s, &pEvent->wbuf, 1, NULL, 0, (LPWSAOVERLAPPED)pEvent, NULL);
        if (SOCKET_ERROR == res) {
            *pnError = WSAGetLastError();
            if (*pnError != WSA_IO_PENDING) {
                return ERROR_WSASEND;
            }
        }
        return ERROR_NO_ERROR;
    }

    void safe_shutdown(struct iocp_event * pEvent) {
        shutdown(pEvent->s, SD_SEND);
    }

    void safe_close(struct iocp_event * pEvent) {
        s32 err;
        struct linger so_linger;
        so_linger.l_linger = 0;
        so_linger.l_onoff = 1;
        err = setsockopt(pEvent->s, SOL_SOCKET, SO_LINGER, (char *)&so_linger, sizeof(so_linger));
        closesocket(pEvent->s);
        free_event(pEvent);
    }

    error_code get_event(OUT struct iocp_event ** ppEvent, OUT s32 * pnError) {
        DWORD nIOBytes;
        BOOL nSucceed;
        s32 nerrno;
        SOCKET s;
        SetLastError(0);
        while (true) {
            if (!s_bInit) {
                NET_ERROR("%s", "iocp_init() first");
                RETURN_RES(ERROR_INIT_IOCP);
            }

            nIOBytes = 0;
            s = INVALID_SOCKET;
            SetLastError(0);
            nSucceed = GetQueuedCompletionStatus(s_hCompletionPort, &nIOBytes, (PULONG_PTR)&s, (LPOVERLAPPED *)ppEvent, s_lWaittiem);

            if(NULL == *ppEvent) {
                CSleep(10);
                continue;
            }

            nerrno = GetLastError();
            if (!nSucceed ) {
                if (WAIT_TIMEOUT == errno) {
                    CSleep(10);
                    continue;
                }
            }
            break;
        }

        (*ppEvent)->nerron = nerrno;
        (*ppEvent)->ioBytes = nIOBytes;
        /*
        if (SOCKET_ERROR == setsockopt(s, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0)) {
            RETURN_RES(ERROR_SET_SOCKET_OPT);
        }
        */
        switch ((*ppEvent)->event) {
        case EVENT_ASYNC_ACCEPT:
            {
                s32 res, err;
                struct iocp_event * pEvent = malloc_event();
                if (NULL == pEvent) {
                    ASSERT(false);
                    RETURN_RES(ERROR_MALLOC_EVENT);
                }
                pEvent->p = (*ppEvent)->p;
                pEvent->event = EVENT_ASYNC_ACCEPT;
                if (INVALID_SOCKET == (pEvent->s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED))) {
                    RETURN_RES(ERROR_WSASOCKET_ERROR);
                }

                res = s_pFunAcceptEx(s, pEvent->s, pEvent->buff, 0,
                    sizeof(struct sockaddr_in) + 16, sizeof(struct sockaddr_in) + 16, &pEvent->dwBytes, (LPOVERLAPPED)pEvent);

                err = WSAGetLastError();
                if (res == FALSE && err != WSA_IO_PENDING) {
                    RETURN_RES(ERROR_ACCEPTEX);
                }

                if (ERROR_SUCCESS == nerrno) {
                    s32 nLen = sizeof(struct sockaddr);
                    setsockopt((*ppEvent)->s, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (const char*) &s, sizeof(s));
                    if (SOCKET_ERROR == getpeername((*ppEvent)->s, (struct sockaddr*)&(*ppEvent)->remote, &nLen)) {
                        NET_ERROR("%s", "getpeername error");
                        RETURN_RES(ERROR_GET_PEER_NAME);
                    }
                    if (s_hCompletionPort != (CreateIoCompletionPort((HANDLE)((*ppEvent)->s), (HANDLE)s_hCompletionPort, (u_long)(HANDLE)((*ppEvent)->s), 0))) {
                        RETURN_RES(ERROR_RELATE_IO_COMPLETIONPORT);
                    }
                    RETURN_RES(ERROR_NO_ERROR);
                }

                break;
            }
        case EVENT_ASYNC_CONNECT:
            {
                if (ERROR_SUCCESS == nerrno) {
                    s32 nLen = sizeof(struct sockaddr);
                    if (SOCKET_ERROR == setsockopt(s, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0)) {
                        RETURN_RES(ERROR_SET_SOCKET_OPT);
                    }

                    if (SOCKET_ERROR == getpeername(s, (struct sockaddr*)&(*ppEvent)->remote, &nLen)) {
                        NET_ERROR("%s", "getpeername error");
                        RETURN_RES(ERROR_GET_PEER_NAME);
                    }
                    RETURN_RES(ERROR_NO_ERROR);
                }

                break;
            }
        }
        RETURN_RES(ERROR_NO_ERROR);
    }

#ifdef __cplusplus
};
#endif //__cplusplus

#endif //#if defined WIN32 || defined WIN64
