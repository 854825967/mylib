#include "WinNetCore.h"
#include "Tools.h"

CNet::CNet(const u8 threadcount, const u16 linkcount, const u16 iowaittime) {
    m_pFunAcceptEx = NULL;
    m_pFunConnectEx = NULL;
    GetAcceptExFun(m_pFunAcceptEx);
    GetConnectExFun(m_pFunConnectEx);
    ASSERT(m_pFunAcceptEx != NULL && m_pFunConnectEx != NULL);

    m_nNetThreadCount = threadcount;
    m_nIOWaitTime = iowaittime;
    m_pRecvFun = NULL;
    m_pConnectedFun = NULL;
    m_pCntFailedFun = NULL;
    m_pCntBreakFun = NULL;
    m_endIocp = FALSE;
    if ((m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0)) == NULL) {
        NET_ERROR("CreateIoCompletionPort error, error code %s", GetLastError());
    }
}

CNet::~CNet() {

}

bool CNet::CListen(const char * ip, const u16 port, const u16 count, const void * buff, const u32 size) {
    SockData * pSockData = m_SockDataPool.Create();
    if(NULL == pSockData) {
        NET_ERROR("Get SockData error");
        return false;
    }
    pSockData->m_address.SetIp(ip);
    pSockData->m_address.SetIPort(port);

    pSockData->m_sock = ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (INVALID_SOCKET == pSockData->m_sock) {
        NET_ERROR("Get error, last error code : %d", GetLastError());
        m_SockDataPool.Recover(pSockData);
        CSleep(1);
        return false;
    }

    int len = 8192;
    if (SOCKET_ERROR == ::setsockopt(pSockData->m_sock, SOL_SOCKET, SO_SNDBUF, (char *)&len, sizeof(int))
        || SOCKET_ERROR == ::setsockopt(pSockData->m_sock, SOL_SOCKET, SO_RCVBUF, (char *)&len, sizeof(int))) {
        NET_ERROR("setsockopt error, last error code : %d", GetLastError());
        closesocket(pSockData->m_sock);
        m_SockDataPool.Recover(pSockData);
        CSleep(1);
        return false;
    }


    if (SOCKET_ERROR == ::bind(pSockData->m_sock, (sockaddr*)&(pSockData->m_address), sizeof(sockaddr_in))) {
        NET_ERROR("Bind error, last error code : %d", GetLastError());
        closesocket(pSockData->m_sock);
        m_SockDataPool.Recover(pSockData);
        CSleep(1);
        return false;
    }
    if (::listen(pSockData->m_sock, count) == SOCKET_ERROR) {
        NET_ERROR("Listen error, last error code : %d", GetLastError());
        closesocket(pSockData->m_sock);
        m_SockDataPool.Recover(pSockData);
        CSleep(1);
        return false;
    } 
    if (NULL == m_pFunAcceptEx) {
        NET_ERROR("AcceptEx Fun address is NULL");
        closesocket(pSockData->m_sock);
        m_SockDataPool.Recover(pSockData);
        CSleep(1);
        return false;
    }
    if (m_hCompletionPort != (CreateIoCompletionPort((HANDLE)pSockData->m_sock, (HANDLE)m_hCompletionPort, (u_long)pSockData, 0))) {
        NET_ERROR("CreateIoCompletionPort error, error code %d", GetLastError());
        closesocket(pSockData->m_sock);
        m_SockDataPool.Recover(pSockData);
        CSleep(1);
        return false;
    }

    NET_TRACE("Start a listen sock, ip : %s, port : %d, socket : %d", ip, port, pSockData->m_sock);
    SockPlus * pSockPlus = pSockData->GetSockPlus(EVENT_ACCEPT);
    if (INVALID_SOCKET == (pSockPlus->m_acceptSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED))) {
        NET_ERROR("Get error, last error code : %d", GetLastError());
        closesocket(pSockData->m_sock);
        closesocket(pSockPlus->m_acceptSock);
        m_SockDataPool.Recover(pSockData);
        CSleep(1);
        return false;
    }

    SockPlus * pConPlus = pSockData->GetSockPlus(EVENT_CONNECT);
    ASSERT(pConPlus);
    if (NULL != buff) {
        if (size <= sizeof(pConPlus->m_buff)) {
            memcpy(pConPlus->m_buff, buff, size);
            pConPlus->m_wBuf.len = size;
        } else {
            memcpy(pConPlus->m_buff, buff, sizeof(pConPlus->m_buff));
            pConPlus->m_wBuf.len = sizeof(pConPlus->m_buff);
        }
    } else {
        pConPlus->m_wBuf.len = 0;
    }

    BOOL result = m_pFunAcceptEx(
        pSockData->m_sock,
        pSockPlus->m_acceptSock,
        pSockPlus->m_buff,
        0,
        sizeof(sockaddr_in) + 16,
        sizeof(sockaddr_in) + 16,
        &pSockPlus->m_dwBytes, 
        (LPOVERLAPPED)pSockPlus
    );

    s32 nError = WSAGetLastError();
    if (result == FALSE && nError != WSA_IO_PENDING) {
        NET_ERROR("AcceptEx error,error code %d", nError);
        closesocket(pSockData->m_sock);
        closesocket(pSockPlus->m_acceptSock);
        m_SockDataPool.Recover(pSockData);
        CSleep(1);
        return true;
    }
    return false;
}

bool CNet::CConnectEx(const char * ip, const u16 port, const void * buff, const u32 size) {
    if (NULL == m_pFunConnectEx) {
        NET_ERROR("ConnectEX函数指针为空");
        CSleep(1);
        return false;
    }

    SockData * pSockData = m_SockDataPool.Create();
    if (NULL == pSockData) {
        NET_ERROR("获取SockData失败");
        CSleep(1);
        return false;
    }

    if (!pSockData->m_address.SetIp(ip) || !pSockData->m_address.SetIPort(port)) {
        NET_ERROR("Set Remote ip error, Remote ip %s, Remote port %d", ip, port);
        m_SockDataPool.Recover(pSockData);
        CSleep(1);
        return false;
    }
    
    if (INVALID_SOCKET == (pSockData->m_sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED)) ) {
        NET_ERROR("WSASocket error, last error code : %d", GetLastError());
        m_SockDataPool.Recover(pSockData);
        CSleep(1);
        return false;
    }
    CInetAddress address;
    if (SOCKET_ERROR == ::bind(pSockData->m_sock, (sockaddr *)&address, sizeof(sockaddr_in))) {
        NET_ERROR("Bind error, socket : %d, last error code : %d", pSockData->m_sock, GetLastError());
        closesocket(pSockData->m_sock);
        m_SockDataPool.Recover(pSockData);
        CSleep(1);
        return false;
    }

    if (m_hCompletionPort != ::CreateIoCompletionPort((HANDLE)pSockData->m_sock, m_hCompletionPort, (u_long)pSockData, 0)) {
        NET_ERROR("CreateIoCompletionPort error, error code %d", GetLastError());
        closesocket(pSockData->m_sock);
        m_SockDataPool.Recover(pSockData);
        CSleep(1);
        return false;
    }

    SockPlus * pSockPlus = pSockData->GetSockPlus(EVENT_CONNECT);
    ASSERT(size < BUFF_SIZE);
    if (size >= BUFF_SIZE) {
        NET_ERROR("Connect 所携带的异步参数超过最大长度 %d", BUFF_SIZE);
        memcpy(pSockPlus->m_buff, buff, BUFF_SIZE - 1);
        pSockPlus->m_buff[BUFF_SIZE - 1] = 0;
        pSockPlus->m_wBuf.buf = pSockPlus->m_buff;
        pSockPlus->m_wBuf.len = BUFF_SIZE - 1;
    } else {
        if (NULL == buff) {
            pSockPlus->m_wBuf.buf = NULL;
            pSockPlus->m_wBuf.len = 0;
        } else {
            memcpy(pSockPlus->m_buff, buff, size);
            pSockPlus->m_wBuf.buf = pSockPlus->m_buff;
            pSockPlus->m_wBuf.len = size;
        }
    }

    u32 nResult = m_pFunConnectEx(pSockData->m_sock, (sockaddr *)&pSockData->m_address, sizeof(sockaddr_in), NULL, 0, &pSockPlus->m_dwBytes, (LPOVERLAPPED)pSockPlus);
    s32 nError = WSAGetLastError();
    if (nResult == FALSE && nError != WSA_IO_PENDING) {
        NET_ERROR("ConnectEx error,error code %d", nError);
        CClose(pSockData);
        CSleep(1);
        return false;
    }

    return true;
}

bool CNet::CSendMsg(const u16 conid, const void * buff, const u16 size) {
    SockData * pSockData = m_SockDataPool.Query(conid);
    if (NULL == pSockData) {
        return false;
    }

    if (pSockData->m_isclose) {
        return false;
    }

    bool isempty = pSockData->StreamIsEmpty();
    if (!pSockData->Write(buff, size)) {
        ASSERT(false);
        return false;
    }
    
    if (isempty) {
        SockPlus * pSockPlus = pSockData->GetSockPlus(EVENT_SEND);
        pSockPlus->m_wBuf.buf = pSockData->Read( (u32 &)(pSockPlus->m_wBuf.len) );
        if (pSockPlus->m_wBuf.len > BUFF_SIZE) {
            pSockPlus->m_wBuf.len = BUFF_SIZE;
        }
        u32 nResult = ::WSASend(pSockData->m_sock, &pSockPlus->m_wBuf, 1, NULL, 0, (LPWSAOVERLAPPED)pSockPlus, NULL);

        if (SOCKET_ERROR == nResult) {
            nResult = ::WSAGetLastError();
            if (nResult != WSA_IO_PENDING) {
                NET_WARN("WSASend error, error code %d", nResult);
                if (CClose(pSockData)) {
                    m_CQueue.AddEvent(pSockData->GetID(), CEVENT_CONNECT_BREAK, NULL, 0);
                }
                CSleep(1);
                return false;
            }
        }
    }

    return true;
}

bool CNet::CClose(const u16 conid) {
    SockData * pSockData = m_SockDataPool.Query(conid);
    if (NULL == pSockData) {
        return false;
    }
    if (pSockData->StreamIsEmpty()) {
        CClose(pSockData);
    } else {
        shutdown(pSockData->m_sock, SD_RECEIVE);
        pSockData->m_isclose = true;
    }
    return true;
}

THREAD_FUN CNet::CEventLoop(LPVOID p) {
    ASSERT(p != NULL);
    CNet * pThis = (CNet *)p;
    while (true) {
        u32 id = 0;
        u8 type = 0;
        void * buff = NULL; 
        u32 size = 0;
        if (pThis->m_CQueue.ReadEvent(id, type, buff, size)) {
            switch (type) {
            case CEVENT_CONNECT_SECUESS:
                ASSERT(pThis->m_pConnectedFun != NULL);
                pThis->m_pConnectedFun(id, buff, size);
                break;
            case CEVENT_CONNECT_FAILED:
                ASSERT(pThis->m_pCntFailedFun != NULL);
                pThis->m_pCntFailedFun(id, buff, size);
                break;
            case CEVENT_CONNECT_BREAK:
                ASSERT(pThis->m_pCntBreakFun != NULL);
                pThis->m_pCntBreakFun(id, buff, size);
                break;
            case CEVENT_NEW_CONNECT:
                ASSERT(pThis->m_pNewConFun != NULL);
                pThis->m_pNewConFun(id, buff, size);
                break;
            case CEVENT_RECV:
                ASSERT(pThis->m_pRecvFun != NULL);
                pThis->m_pRecvFun(id, buff, size);
                break;
            default:
                break;
            }
        }
    }
}

void CNet::CStartLoop(bool demon) {
    for(u32 i=0; i<m_nNetThreadCount; i++) {
        HANDLE hThread = ::CreateThread(NULL, 0, CNet::CLoop, (LPVOID)this, 0, NULL);
        CloseHandle(hThread);
    }

    if (demon) {
        HANDLE hThread = ::CreateThread(NULL, 0, CNet::CEventLoop, (LPVOID)this, 0, NULL);
        CloseHandle(hThread);
    } else {
        CEventLoop(this);
    }

}

bool CNet::CGetRemoteInfo(const u16 conid, const char * & ip, u16 & port) {
    SockData * pSockData = m_SockDataPool.Query(conid);
    if(NULL == pSockData) {
        NET_WARN("the con is not exists, conid : %d", conid);
        return false;
    }

    ip = pSockData->m_address.GetIp();
    port = pSockData->m_address.GetPort();

    return true;
}

void CNet::CSetNetThreadCount(const u8 count) {
    m_nNetThreadCount = count;
}

bool CNet::CSetConnectedCall(const CALL_FUN pcntedfun) {
    if (NULL == pcntedfun) {
        NET_ERROR("ConnectedFun address is NULL");
        return false;
    }
    m_pConnectedFun = pcntedfun;
    return true;
}

bool CNet::CSetConnectFailedCall(const CALL_FUN pcntfailedfun) {
    if (NULL == pcntfailedfun) {
        NET_ERROR("ConnectedFailedFun address is NULL");
        return false;
    }
    m_pCntFailedFun = pcntfailedfun;
    return true;
}

bool CNet::CSetConnectionBreakCall(const CALL_FUN pbreakfun) {
    if (NULL == pbreakfun) {
        NET_ERROR("Connection break Fun address is NULL");
        return false;
    }
    m_pCntBreakFun = pbreakfun ;
    return true;
}

bool CNet::CSetRecvCall(const CALL_FUN precvfun) {
    if (NULL == precvfun) {
        NET_ERROR("RecvFun address is NULL");
        return false;
    }
    m_pRecvFun = precvfun;
    return true;
}

bool CNet::CSetNewConCall(const CALL_FUN pnewconfun) {
    if (NULL == pnewconfun) {
        NET_ERROR("RecvFun address is NULL");
        return false;
    }
    m_pNewConFun = pnewconfun;
    return true;
}


void CNet::CAccept(SockData * pSockData, SockPlus * pSockPlus, const u32 nSize, u32 nErrorCode) {
    ASSERT(pSockData != NULL && pSockPlus != NULL);
    ASSERT( (0 == nErrorCode) || (ERROR_NETNAME_DELETED == nErrorCode));
    if (nErrorCode != ERROR_NETNAME_DELETED) {
        SockData * pNewConSockData = m_SockDataPool.Create();
        if (pNewConSockData != NULL) {
            pNewConSockData->m_sock = pSockPlus->m_acceptSock;
            NET_TRACE("新链接 %d", pNewConSockData->m_sock);
            SockPlus * pRecvSockPlus = pNewConSockData->GetSockPlus(EVENT_RECV);
            ASSERT(pRecvSockPlus != NULL);
            if (m_hCompletionPort == (CreateIoCompletionPort((HANDLE)pNewConSockData->m_sock, (HANDLE)m_hCompletionPort, (u_long)pNewConSockData, 0))) {
                if (SOCKET_ERROR == ::WSARecv(pNewConSockData->m_sock, &pRecvSockPlus->m_wBuf, 1, &pRecvSockPlus->m_dwBytes, 
                    &pRecvSockPlus->m_dwFlags, (LPWSAOVERLAPPED)pRecvSockPlus, NULL)) 
                {
                    s32 res = ::WSAGetLastError();
                    if (WSA_IO_PENDING != res) {
                        NET_WARN("WSARecv error, error code %d", res);
                        shutdown(pNewConSockData->m_sock, SD_BOTH);
                        closesocket(pNewConSockData->m_sock);
                        m_SockDataPool.Recover(pNewConSockData);
                        CSleep(1);
                    }
                    int len = sizeof(sockaddr);
                    setsockopt(pNewConSockData->m_sock, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (const char*) &pSockData->m_sock, sizeof(pSockData->m_sock));
                    if (SOCKET_ERROR == ::getpeername(pNewConSockData->m_sock, (sockaddr*)&pNewConSockData->m_address, &len)) {
                        NET_ERROR("获取远端信息出错, 错误码 %d", GetLastError());
                        shutdown(pNewConSockData->m_sock, SD_BOTH);
                        closesocket(pNewConSockData->m_sock);
                        m_SockDataPool.Recover(pNewConSockData);
                        CSleep(1);
                    } else {
                        SockPlus * pConPlus = pSockData->GetSockPlus(EVENT_CONNECT);
                        SockPlus * pNewConPlus = pNewConSockData->GetSockPlus(EVENT_CONNECT);
                        if (pConPlus->m_wBuf.len != 0) {
                            memcpy(pNewConPlus->m_buff, pConPlus->m_buff, pConPlus->m_wBuf.len);
                        }
                        pNewConPlus->m_wBuf.len = pConPlus->m_wBuf.len;
                        m_CQueue.AddEvent(pNewConSockData->GetID(), CEVENT_NEW_CONNECT, pConPlus->m_buff, pConPlus->m_wBuf.len);
                    }
                }
            } else {
                closesocket(pNewConSockData->m_sock);
                m_SockDataPool.Recover(pNewConSockData);
                NET_ERROR("CreateIoCompletionPort error, error code %d", GetLastError());
            }
        } else {
            NET_ERROR("获取SockData 出错");
        }
    }

    ASSERT(m_pFunAcceptEx != NULL);
    
    if (INVALID_SOCKET == (pSockPlus->m_acceptSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED))) {
        closesocket(pSockData->m_sock);
        m_SockDataPool.Recover(pSockData);
        NET_ERROR("WSASocket error, last error code : %d", GetLastError());
        return;
    }

    BOOL result = m_pFunAcceptEx(
        pSockData->m_sock,
        pSockPlus->m_acceptSock,
        pSockPlus->m_buff,
        0,
        sizeof(sockaddr_in) + 16,
        sizeof(sockaddr_in) + 16,
        &pSockPlus->m_dwBytes, 
        (LPOVERLAPPED)pSockPlus
        );

    s32 nError = WSAGetLastError();
    if (result == FALSE && nError != WSA_IO_PENDING) {
        NET_ERROR("AcceptEx error,error code %d", nError);
        closesocket(pSockData->m_sock);
        closesocket(pSockPlus->m_acceptSock);
        m_SockDataPool.Recover(pSockData);
        CSleep(1);
        return;
    }

    return;
}

bool CNet::CClose(SockData * pSockData) {
    SOCKET sock = pSockData->m_sock;
    if (m_SockDataPool.Recover(pSockData)) {
        shutdown(sock, SD_BOTH);
        closesocket(sock);
        NET_ERROR("回收 sockdata %d, socket %d", pSockData->GetID(), sock);
        return true;
    }
    return false;
}

void CNet::CRecv(SockData * pSockData, SockPlus * pSockPlus, const u32 nSize, u32 nErrorCode) {
    ASSERT( (0 == nErrorCode) || (ERROR_NETNAME_DELETED == nErrorCode) || (ERROR_SEM_TIMEOUT) == nErrorCode);
    switch (nErrorCode) {
    case ERROR_NETNAME_DELETED:
    case ERROR_SEM_TIMEOUT:
        {
            if (CClose(pSockData)) {
                m_CQueue.AddEvent(pSockData->GetID(), CEVENT_CONNECT_BREAK, NULL, 0);
            }
            return;
        }
    }

    if (0 == nSize) {
        if (CClose(pSockData)) {
            m_CQueue.AddEvent(pSockData->GetID(), CEVENT_CONNECT_BREAK, NULL, 0);
        }
        return;
    }

    ASSERT(0 == nErrorCode);
    ASSERT(m_pRecvFun != NULL);
    m_CQueue.AddEvent(pSockData->GetID(), CEVENT_RECV, pSockPlus->m_wBuf.buf, nSize);
    pSockPlus->Initialize();
    
    if (SOCKET_ERROR == ::WSARecv(pSockData->m_sock, &pSockPlus->m_wBuf, 1, &pSockPlus->m_dwBytes, &pSockPlus->m_dwFlags, (LPWSAOVERLAPPED)pSockPlus, NULL)) {
        s32 res = ::WSAGetLastError();
        if (WSA_IO_PENDING != res) {
            NET_WARN("WSARecv error, error code %d", res);
            if (CClose(pSockData)) {
                m_CQueue.AddEvent(pSockData->GetID(), CEVENT_CONNECT_BREAK, NULL, 0);
            }
            CSleep(1);
            return;
        }
    }
}

void CNet::CSend(SockData * pSockData, SockPlus * pSockPlus, const u32 nSize, u32 nErrorCode) {
    ASSERT(0 == nErrorCode);
    if (0 != nSize) {
        pSockData->Out(nSize);
    }

    if (pSockData->StreamIsEmpty()) {
        pSockPlus->m_wBuf.buf = NULL;
        pSockPlus->m_wBuf.len = 0;
        if (pSockData->m_isclose) {
            if (CClose(pSockData)) {
                m_CQueue.AddEvent(pSockData->GetID(), CEVENT_CONNECT_BREAK, NULL, 0);
            }
        }
        return;
    }

    pSockPlus->m_wBuf.buf = pSockData->Read( (u32 &)(pSockPlus->m_wBuf.len) );
    if (pSockPlus->m_wBuf.len > BUFF_SIZE) {
        pSockPlus->m_wBuf.len = BUFF_SIZE;
    }

    u32 nResult = ::WSASend(pSockData->m_sock, &pSockPlus->m_wBuf, 1, NULL, 0, (LPWSAOVERLAPPED)pSockPlus, NULL);

    if (SOCKET_ERROR == nResult) {
        nResult = ::WSAGetLastError();
        if (nResult != WSA_IO_PENDING) {
            NET_WARN("WSASend error, error code %d", nResult);
            if (CClose(pSockData)) {
                m_CQueue.AddEvent(pSockData->GetID(), CEVENT_CONNECT_BREAK, NULL, 0);
            }
            CSleep(1);
        }
    }

}

void CNet::CConnect(SockData * pSockData, SockPlus * pSockPlus, const u32 nSize, u32 nErrorCode) {
    switch (nErrorCode) {
    case ERROR_INVALID_NETNAME:
    case ERROR_SEM_TIMEOUT:
    case ERROR_CONNECTION_REFUSED:
    case ERROR_HOST_UNREACHABLE:
        {
            if (CClose(pSockData)) {
                m_CQueue.AddEvent(pSockData->GetID(), CEVENT_CONNECT_FAILED, NULL, 0);
            }
            return;
        }
    }


    ASSERT(0 == nErrorCode ||
        ERROR_INVALID_NETNAME == nErrorCode ||
        ERROR_SEM_TIMEOUT == nErrorCode ||
        ERROR_CONNECTION_REFUSED == nErrorCode ||
        ERROR_HOST_UNREACHABLE == nErrorCode);

    SockPlus * pRecSockPlus = pSockData->GetSockPlus(EVENT_RECV);
    memset(pRecSockPlus->m_buff, 0, BUFF_SIZE);
    pRecSockPlus->m_wBuf.buf = pRecSockPlus->m_buff;
    pRecSockPlus->m_wBuf.len = BUFF_SIZE;
    if (SOCKET_ERROR == ::WSARecv(pSockData->m_sock, &pRecSockPlus->m_wBuf, 1, &pRecSockPlus->m_dwBytes, &pRecSockPlus->m_dwFlags, (LPWSAOVERLAPPED)pRecSockPlus, NULL)) {
        s32 nRes = ::WSAGetLastError();
        if (WSA_IO_PENDING != nRes) {
            NET_ERROR("WSARecv error,error code %d", nRes);
            if (CClose(pSockData)) {
                m_CQueue.AddEvent(pSockData->GetID(), CEVENT_CONNECT_FAILED, NULL, 0);
            }
            CSleep(1);
            return;
        }
    }

    m_CQueue.AddEvent(pSockData->GetID(), CEVENT_CONNECT_SECUESS, pSockPlus->m_wBuf.buf, pSockPlus->m_wBuf.len);
    return;
}

DWORD WINAPI CNet::CLoop(LPVOID p) {
    ASSERT(p != NULL);
    CNet * pThis = (CNet *)p;
    u16 nWaitTime = pThis->m_nIOWaitTime;
    DWORD nIOBytes = 0;
    BOOL nSucceed;
    SockPlus * pSockPlus = NULL;
    SockData * pSockData = NULL;

    while (true) {
        SetLastError(0);
        nSucceed = ::GetQueuedCompletionStatus(pThis->m_hCompletionPort, &nIOBytes, (PULONG_PTR)&pSockData, (LPOVERLAPPED *)&pSockPlus, nWaitTime);

        if(NULL == pSockPlus || NULL == pSockData) {
            continue;
        }

        int nWSAError = ::WSAGetLastError();
        if (!nSucceed ) {
            if (WAIT_TIMEOUT == nWSAError) {
                CSleep(1);
                continue;
            }
        }

        switch(pSockPlus->m_events) {
        case EVENT_ACCEPT:
            pThis->CAccept(pSockData, pSockPlus, nIOBytes, nWSAError);
            break;
        case EVENT_CONNECT:
            pThis->CConnect(pSockData, pSockPlus, nIOBytes, nWSAError);
            break;
        case EVENT_RECV:
            pThis->CRecv(pSockData, pSockPlus, nIOBytes, nWSAError);
            break;
        case EVENT_SEND:
            pThis->CSend(pSockData, pSockPlus, nIOBytes, nWSAError);
            break;
        }
        pSockData = NULL;
        pSockPlus = NULL;
        nIOBytes = 0;
        CSleep(1);
    }

    return 0;
}