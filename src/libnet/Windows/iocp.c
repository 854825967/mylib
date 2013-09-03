#include "iocp.h"
extern "C" {
#define RETURN_RES(code) {\
    *pnError = ::GetLastError(); \
    return code;}

    static LPFN_ACCEPTEX s_pFunAcceptEx = NULL;
    static LPFN_CONNECTEX s_pFunConnectEx = NULL;
    static u64 s_lWaittiem = 10;
    static HANDLE s_hCompletionPort = NULL;
    static bool s_bInit = false;
    static bool s_bShutdown = false;

    LPFN_ACCEPTEX GetAcceptExFun() {
        GUID GuidAcceptEx = WSAID_ACCEPTEX;
        DWORD dwBytes = 0;
        SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        LPFN_ACCEPTEX pAcceptFun = NULL;
        WSAIoctl(sock, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx, 
            sizeof(GuidAcceptEx), &pAcceptFun, sizeof(pAcceptFun), 
            &dwBytes, NULL, NULL);

        if (NULL == pAcceptFun) {
            ECHO_ERROR("Get AcceptEx fun error, error code : %d", ::WSAGetLastError());
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
            ECHO_ERROR("Get ConnectEx fun error, error code : %d", ::WSAGetLastError());
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

    error_code async_connect(const char * pStrIP, s32 nPort, OUT s32 * pnError, void * pData) {;
        SetLastError(0);
        if (!s_bInit) {
            RETURN_RES(ERROR_INIT_IOCP);
        }
        SOCKET s = INVALID_SOCKET;
        if (INVALID_SOCKET == (s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED)) ) {
            RETURN_RES(ERROR_WSASOCKET_ERROR);
        }

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        if (SOCKET_ERROR == ::bind(s, (sockaddr *)&addr, sizeof(sockaddr_in))) {
            RETURN_RES(ERROR_CONNECTEX_BIND);
        }
        
        DWORD dwValue = 0;
        if (SOCKET_ERROR == ioctlsocket(s, FIONBIO, &dwValue)) {
            closesocket(s);
            RETURN_RES(ERROR_SET_FIONBIO);
        }

        if (s_hCompletionPort != ::CreateIoCompletionPort((HANDLE)s, s_hCompletionPort, (u_long)s, 0)) {
            closesocket(s);
            RETURN_RES(ERROR_RELATE_IO_COMPLETIONPORT);
        }

        struct iocp_event * pEvent = malloc_event();
        pEvent->s = s;
        pEvent->p = pData;
        pEvent->event = EVENT_ASYNC_CONNECT;
        if((pEvent->remote.sin_addr.s_addr = inet_addr(pStrIP)) == INADDR_NONE) {
            free_event(pEvent);
            RETURN_RES(ERROR_IP_FORMAT);
        }
        pEvent->remote.sin_port = htons(nPort);

        s32 res = s_pFunConnectEx(
            s, 
            (struct sockaddr *)&pEvent->remote, 
            sizeof(struct sockaddr_in), 
            NULL, 
            0, 
            &pEvent->dwBytes, 
            (LPOVERLAPPED)pEvent
        );

        s32 err = WSAGetLastError();
        if (res == FALSE && err != WSA_IO_PENDING) {
            closesocket(s);
            free_event(pEvent);
            RETURN_RES(ERROR_CONNECTEX);
        }

        RETURN_RES(ERROR_NO_ERROR);
    }

    error_code async_listen(const char * pStrIP, s32 nPort, OUT s32 * pnError, void * pData, s32 backlog) {
        SetLastError(0);
        if (!s_bInit) {
            RETURN_RES(ERROR_INIT_IOCP);
        }

        SOCKET s = INVALID_SOCKET;
        if (INVALID_SOCKET == (s = ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED))) {
            RETURN_RES(ERROR_WSASOCKET_ERROR);
        }

        int len = BUFF_SIZE;
        if (SOCKET_ERROR == ::setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char *)&len, sizeof(int))
            || SOCKET_ERROR == ::setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char *)&len, sizeof(int))) {
                closesocket(s);
                RETURN_RES(ERROR_SET_SOCKET_OPT);
        }

        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        if(INADDR_NONE == (addr.sin_addr.s_addr = inet_addr(pStrIP))) {
            RETURN_RES(ERROR_IP_FORMAT);
        }
        addr.sin_port = htons(nPort);
        if (SOCKET_ERROR == ::bind(s, (sockaddr*)&(addr), sizeof(sockaddr_in))) {
            RETURN_RES(ERROR_LISTEN_BIND);
        }
        if (::listen(s, backlog) == SOCKET_ERROR) {
            RETURN_RES(ERROR_LISTEN);
        } 

        if (s_hCompletionPort != (CreateIoCompletionPort((HANDLE)s, (HANDLE)s_hCompletionPort, (u_long)s, 0))) {
            RETURN_RES(ERROR_RELATE_IO_COMPLETIONPORT);
        }

        struct iocp_event * pEvent = malloc_event();
        if (NULL == pEvent) {
            ASSERT(false);
            RETURN_RES(ERROR_MALLOC_EVENT);
        }
        pEvent->p = pData;
        pEvent->event = EVENT_ASYNC_ACCEPT;

        if (INVALID_SOCKET == (pEvent->s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED))) {
            RETURN_RES(ERROR_WSASOCKET_ERROR);
        }

        BOOL res = s_pFunAcceptEx(
            s,
            pEvent->s,
            pEvent->buff,
            0,
            sizeof(sockaddr_in) + 16,
            sizeof(sockaddr_in) + 16,
            &pEvent->dwBytes, 
            (LPOVERLAPPED)pEvent
            );

        s32 err = WSAGetLastError();
        if (res == FALSE && err != WSA_IO_PENDING) {
            RETURN_RES(ERROR_ACCEPTEX);
        }

        RETURN_RES(ERROR_NO_ERROR);
    }

    error_code async_recv(iocp_event * pEvent, OUT s32 * pnError, void * pData) {
        SetLastError(0);
        if (!s_bInit) {
            RETURN_RES(ERROR_INIT_IOCP);
        }

        SOCKET s = pEvent->s;
        memset(pEvent, 0, sizeof(struct iocp_event));
        pEvent->wbuf.buf = pEvent->buff;
        pEvent->wbuf.len = sizeof(pEvent->buff);
        pEvent->s = s;
        pEvent->p = pData;
        pEvent->event = EVENT_ASYNC_RECV;
        
        if (SOCKET_ERROR == ::WSARecv(pEvent->s, &pEvent->wbuf, 1, &pEvent->dwBytes, &pEvent->dwFlags, (LPWSAOVERLAPPED)pEvent, NULL)) {
            *pnError = ::GetLastError();
            if (ERROR_IO_PENDING == *pnError) {
                return ERROR_NO_ERROR;
            } else {
                return ERROR_WSARECV;
            }
        }

        RETURN_RES(ERROR_NO_ERROR);
    }

    error_code async_send(iocp_event * pEvent, OUT s32 * pnError, void * pData) {
        SetLastError(0);
        if (!s_bInit) {
            RETURN_RES(ERROR_INIT_IOCP);
        }
        pEvent->p = pData;

        u32 res = ::WSASend(pEvent->s, &pEvent->wbuf, 1, NULL, 0, (LPWSAOVERLAPPED)pEvent, NULL);
        if (SOCKET_ERROR == res) {
            *pnError = ::WSAGetLastError();
            if (*pnError != WSA_IO_PENDING) {
                return ERROR_WSASEND;
            }
        }
        return ERROR_NO_ERROR;
    }

    void safe_shutdown(struct iocp_event * pEvent) {
        shutdown(pEvent->s, SD_BOTH);
    }

    void safe_close(struct iocp_event * pEvent) {
        struct linger so_linger;
        so_linger.l_linger = 0;
        so_linger.l_onoff = 1;
        s32 err = setsockopt(pEvent->s, SOL_SOCKET, SO_LINGER, (char *)&so_linger, sizeof(so_linger));
        closesocket(pEvent->s);
        free_event(pEvent);
    }

    error_code get_event(OUT struct iocp_event ** ppEvent, OUT s32 * pnError) {
        SetLastError(0);
        DWORD nIOBytes = 0;
        BOOL nSucceed;
        s32 nerrno;
        SOCKET s = INVALID_SOCKET;
        if (!s_bInit) {
            RETURN_RES(ERROR_INIT_IOCP);
        }
        nSucceed = ::GetQueuedCompletionStatus(s_hCompletionPort, &nIOBytes, (PULONG_PTR)&s, (LPOVERLAPPED *)ppEvent, s_lWaittiem);
        if(NULL == *ppEvent) {
            RETURN_RES(ERROR_GET_EVENT);
        }

        nerrno = GetLastError();
        if (!nSucceed ) {
            if (WAIT_TIMEOUT == errno) {
                RETURN_RES(ERROR_GET_EVENT_TIME_OUT)
            }
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

                BOOL res = s_pFunAcceptEx(s, pEvent->s, pEvent->buff, 0,
                    sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, &pEvent->dwBytes, (LPOVERLAPPED)pEvent);

                s32 err = WSAGetLastError();
                if (res == FALSE && err != WSA_IO_PENDING) {
                    RETURN_RES(ERROR_ACCEPTEX);
                }

                if (ERROR_SUCCESS == nerrno) {
                    setsockopt((*ppEvent)->s, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (const char*) &s, sizeof(s));
                    s32 nLen = sizeof(sockaddr);
                    if (SOCKET_ERROR == ::getpeername((*ppEvent)->s, (sockaddr*)&(*ppEvent)->remote, &nLen)) {
                        ECHO_ERROR("getpeername error");
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
                    if (SOCKET_ERROR == setsockopt(s, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0)) {
                        RETURN_RES(ERROR_SET_SOCKET_OPT);
                    }

                    s32 nLen = sizeof(sockaddr);
                    if (SOCKET_ERROR == ::getpeername(s, (sockaddr*)&(*ppEvent)->remote, &nLen)) {
                        ECHO_ERROR("getpeername error");
                        RETURN_RES(ERROR_GET_PEER_NAME);
                    }
                    RETURN_RES(ERROR_NO_ERROR);
                }

                break;
            }
        }
        RETURN_RES(ERROR_NO_ERROR);
    }

};
