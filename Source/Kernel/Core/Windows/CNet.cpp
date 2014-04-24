#if defined WIN32 || defined WIN64

#include "CNet.h"
#include "Tools.h"
#include "iocp.h"
#include "Header.h"

CNet::CNet() {
    m_lWaitMs = 0;
    m_nFrameMs = 0;
    m_nThreadCount = 0;
    m_stop = true;

    memset(m_szCallAddress, 0, sizeof(m_szCallAddress));
}

CNet::~CNet() {
    CStop();
}


bool CNet::CNetInitialize(const s8 nThreadCount, const s64 lWaitMs) {
    LogModuleInit();
    m_demo = false;
    m_stop = false;

    m_nThreadCount = nThreadCount;
    m_lWaitMs = lWaitMs;
    NET_TRACE("网络引擎初始化,线程数 %d, 单帧最大时间 %d", nThreadCount, lWaitMs);

    s32 err;
    s32 code = iocp_init(lWaitMs, &err);
    if (code != ERROR_NO_ERROR) {
        NET_ERROR("init iocp error, last code : %d", err);
        ASSERT(false);
    } else {
        NET_TRACE("init iocp success");
        for (s32 i=0; i<nThreadCount; i++) {
            HANDLE hThread = ::CreateThread(NULL, 0, CNet::NetLoop, (LPVOID)this, 0, NULL);
            CloseHandle(hThread);
        }
    }
    return true;
}

void CNet::CStop() {
    ASSERT(!m_stop);
    LogModuleStop();
    m_stop = true;
    iocp_stop();
    while (m_nThreadCount != 0) {
        CSleep(10);
    }
    NET_TRACE("网络停止工作");
}

bool CNet::CListen(const char * pStrIP, const s32 nPort, const void * pContext, const s32 nBacklog) {
    s32 err;
    s32 code = async_listen(pStrIP, nPort, &err, (char *)pContext, nBacklog);
    if (code != ERROR_NO_ERROR) {
        ASSERT(false);
        NET_ERROR("async_listen error, last code : %d", err);
        return false;
    }

    NET_TRACE("async_listen ok, ip:%s, port:%d, context:%ld, nbocklog %d", pStrIP, nPort, pContext, nBacklog);
    return true;
}

bool CNet::CConnectEx(const char * pStrIP, const s32 nPort, const void * pContext) {
    s32 err;
    if (async_connect(pStrIP, nPort, &err, (char *)pContext) != ERROR_NO_ERROR) {
        NET_ERROR("async_connect error, last code : %d", err);
        return false;
    }

    NET_TRACE("async_connect ok, ip:%s, port:%d, context:%ld", pStrIP, nPort, pContext);
    return true;
}

void CNet::CContext(const s32 nConnectID, void * & pContext) {
    CConnection * pCConnection = m_ConnectPool[nConnectID];
    ASSERT(pCConnection);
    pContext = pCConnection->pContext;
}

void CNet::SafeShutdwon(struct iocp_event * pEvent, s32 sdtags) {
    CConnection * pCConnection = (CConnection *) (pEvent->p);
    pCConnection->bShutdown = true;
    CAutoLock(&(pCConnection->sdlock));
    pCConnection->nSDTags  = pCConnection->nSDTags | sdtags;

    switch(pCConnection->nSDTags) {
    case CSD_RECV:
        shutdown(pEvent->s, SD_RECEIVE);
        break;
    case CSD_SEND:
        shutdown(pEvent->s, SD_SEND);
        break;
    case CSD_BOTH:
        shutdown(pEvent->s, SD_BOTH);
        break;
    default:
        ASSERT(false);
        return;
    }

    if (pCConnection->nSDTags == CSD_BOTH) {
        pCConnection->stream.clear();
        pCConnection->nSDTags = 0;
        pCConnection->Clear();
        m_ConnectPool.Recover(pCConnection);
        safe_close(pEvent);
    }

    closesocket(pCConnection->s);
}

void CNet::CSend(const s32 nConnectID, const void * pData, const s32 nSize) {
    CConnection * pCConnection = m_ConnectPool[nConnectID];
    if (pCConnection->bShutdown) {
        return;
    }
    s32 size = pCConnection->stream.size();
    pCConnection->stream.in(pData, nSize);
    if (0 == size) {
        struct iocp_event * pEvent = malloc_event();
        ASSERT(pEvent);
        pEvent->event = EVENT_ASYNC_SEND;
        pEvent->wbuf.buf = (char *) pCConnection->stream.buff();
        pEvent->wbuf.len = nSize;
        pEvent->p = pCConnection;
        pEvent->s = pCConnection->s;
        s32 err;
        {
            CAutoLock(&(pCConnection->sdlock));
            pCConnection->nSDTags = pCConnection->nSDTags & CSD_RECV;
        }
        if (ERROR_NO_ERROR != async_send(pEvent, &err, pCConnection)) {
            //some problem in here must be killed// like 10054
            NET_DEBUG("async_send failed, err %d", err);
            SafeShutdwon(pEvent, CSD_SEND);
        }
    }

}

bool CNet::CClose(const s32 nConnectID) {
    CConnection * pCConnection = m_ConnectPool[nConnectID];
    pCConnection->bShutdown = true;
    if (0 == pCConnection->stream.size()) {
        CAutoLock(&pCConnection->sdlock);
        pCConnection->nSDTags = pCConnection->nSDTags | CSD_SEND;
        shutdown(pCConnection->s, SD_SEND);
        closesocket(pCConnection->s);
    }

    return true;
}

THREAD_FUN CNet::NetLoop(LPVOID p) {
    CNet * pThis = (CNet *)p;
    if (NULL == pThis) {
        ASSERT(false);
        return -1;
    }
    s32 code = 0;
    s32 err = 0;
    struct iocp_event * pEvent;
    while (true) {
        if (pThis->m_stop) {
            CAutoLock(&(pThis->m_freelock));
            pThis->m_nThreadCount--;
            return 0;
        }

        pEvent = NULL;
        code = get_event(&pEvent, &err);
        if (ERROR_NO_ERROR == code && pEvent != NULL) {
            NET_TRACE("event %d, code %d, last error %d, io bytes %d", pEvent->event, code, err, pEvent->ioBytes);
            pThis->m_queue.add(pEvent);
        } else {
            if (pEvent != NULL) {
                NET_ERROR("event %d, code %d, error %d", pEvent->event, code, err);
            }
            CSleep(10);
        }
    }
}


void CNet::DealConnectEvent(struct iocp_event * pEvent) {
    ASSERT(m_szCallAddress[CALL_CONNECTED]);
    if (m_szCallAddress[CALL_CONNECTED] != NULL) {
        if (ERROR_SUCCESS == pEvent->nerron) {
            s32 nConnectID = m_ConnectPool.CreateID();
            m_ConnectPool[nConnectID]->s = pEvent->s;
            SafeSprintf(m_ConnectPool[nConnectID]->szIP, sizeof(m_ConnectPool[nConnectID]->szIP),
                "%s", inet_ntoa(pEvent->remote.sin_addr));
            m_ConnectPool[nConnectID]->nPort = ntohs(pEvent->remote.sin_port);

            m_szCallAddress[CALL_CONNECTED](nConnectID, pEvent->p, 0);
            pEvent->event = EVENT_ASYNC_RECV;
            pEvent->wbuf.buf = pEvent->buff;
            pEvent->wbuf.len = sizeof(pEvent->buff);
            m_ConnectPool[nConnectID]->pContext = pEvent->p;
            pEvent->p = m_ConnectPool[nConnectID];
            s32 err, code;
            if (ERROR_NO_ERROR != (code = async_recv(pEvent, &err, pEvent->p))) {
                NET_ERROR("async_recv error %d code %d", err, code);
                m_szCallAddress[CALL_CONNECT_FAILED](-1, pEvent->p, 0);
                SafeShutdwon(pEvent, CSD_BOTH);
                return;
            }
        } else {
            NET_TRACE("connect failed, return code %d", pEvent->nerron);
            m_szCallAddress[CALL_CONNECT_FAILED](-1, pEvent->p, 0);
            safe_close(pEvent);
        }

    }

}

void CNet::DealAcceptEvent(struct iocp_event * pEvent) {
    ASSERT(m_szCallAddress[CALL_REMOTE_CONNECTED]);
    if (m_szCallAddress[CALL_REMOTE_CONNECTED] != NULL) {
        if (ERROR_SUCCESS == pEvent->nerron) {
            s32 nConnectID = m_ConnectPool.CreateID();

            m_ConnectPool[nConnectID]->s = pEvent->s;
            SafeSprintf(m_ConnectPool[nConnectID]->szIP, sizeof(m_ConnectPool[nConnectID]->szIP),
                "%s", inet_ntoa(pEvent->remote.sin_addr));
            m_ConnectPool[nConnectID]->nPort = ntohs(pEvent->remote.sin_port);

            m_szCallAddress[CALL_REMOTE_CONNECTED](nConnectID, pEvent->p, 0);

            pEvent->event = EVENT_ASYNC_RECV;
            pEvent->wbuf.buf = pEvent->buff;
            pEvent->wbuf.len = sizeof(pEvent->buff);
            m_ConnectPool[nConnectID]->pContext = pEvent->p;
            pEvent->p = m_ConnectPool[nConnectID];
            s32 err;
            if (ERROR_NO_ERROR != async_recv(pEvent, &err, pEvent->p)) {
                NET_ERROR("async_recv error %d code %d", err, pEvent->nerron);
                SafeShutdwon(pEvent, CSD_BOTH);
                return;
            }
        } else {
            safe_close(pEvent);
        }
    }
}

void CNet::DealRecvEvent(struct iocp_event * pEvent) {
    ASSERT(m_szCallAddress[CALL_CONNECTION_BREAK]);
    ASSERT(m_szCallAddress[CALL_RECV_DATA]);
    CConnection * pCConnection = (CConnection *) (pEvent->p);
    if (ERROR_SUCCESS == pEvent->nerron) {
        if (0 == pEvent->ioBytes) {
            m_szCallAddress[CALL_CONNECTION_BREAK](m_ConnectPool.QueryID(pCConnection), pCConnection->pContext, 0);
            SafeShutdwon(pEvent, CSD_RECV);
            return;
        } else {
            m_szCallAddress[CALL_RECV_DATA](m_ConnectPool.QueryID(pCConnection), pEvent->wbuf.buf, pEvent->ioBytes);
            pEvent->event = EVENT_ASYNC_RECV;
            pEvent->wbuf.buf = pEvent->buff;
            pEvent->wbuf.len = sizeof(pEvent->buff);
            s32 err;
            if (ERROR_NO_ERROR != async_recv(pEvent, &err, pEvent->p)) {
                m_szCallAddress[CALL_CONNECTION_BREAK](m_ConnectPool.QueryID(pCConnection), pCConnection->pContext, 0);
                SafeShutdwon(pEvent, CSD_RECV);
                return;
            }
        }
    } else {
        m_szCallAddress[CALL_CONNECTION_BREAK](m_ConnectPool.QueryID(pCConnection), pCConnection->pContext, 0);
        SafeShutdwon(pEvent, CSD_RECV);
    }
}

void CNet::DealSendEvent(struct iocp_event * pEvent) {
    if (ERROR_SUCCESS == pEvent->nerron) {
        CConnection * pCConnection = (CConnection *) (pEvent->p);
        pCConnection->stream.out(pEvent->ioBytes);
        s32 nSize = pCConnection->stream.size();
        if (0 == nSize) {
            if (pCConnection->bShutdown) {
                SafeShutdwon(pEvent, CSD_SEND);
            } else {
                free_event(pEvent);
            }
            CAutoLock(&(pCConnection->sdlock));
            pCConnection->nSDTags = pCConnection->nSDTags | CSD_SEND;
        } else {
            s32 err;
            pEvent->wbuf.buf = (char *)pCConnection->stream.buff();
            pEvent->wbuf.len = nSize;
            if (ERROR_NO_ERROR != async_send(pEvent, &err, pEvent->p)) {
                SafeShutdwon(pEvent, CSD_SEND);
            }
        }
    } else {
        SafeShutdwon(pEvent, CSD_SEND);
    }
}

THREAD_FUN CNet::DemonLoop(LPVOID p) {
    CNet * pThis = (CNet *)p;
    s64 lTick = ::GetCurrentTimeTick();
    while (true) {
        struct iocp_event * pEvent = NULL;
        if (pThis->m_queue.read(pEvent)) {
            if (pEvent != NULL) {
                switch (pEvent->event) {
                case EVENT_ASYNC_ACCEPT:
                    pThis->DealAcceptEvent(pEvent);
                    break;
                case EVENT_ASYNC_CONNECT:
                    pThis->DealConnectEvent(pEvent);
                    break;
                case EVENT_ASYNC_RECV:
                    pThis->DealRecvEvent(pEvent);
                    break;
                case EVENT_ASYNC_SEND:
                    pThis->DealSendEvent(pEvent);
                    break;
                default:
                    break;
                }
            }
        } else if (pThis->m_stop && pThis->m_queue.IsEmpty()) {
            NET_TRACE("DemonLoop 停止工作");
            return 0;
        }

        if(!pThis->m_demo) {
            if (::GetCurrentTimeTick() - lTick > (u32)(pThis->m_nFrameMs) && pThis->m_nFrameMs != 0) {
                break;
            }
        }
    }
    return 0;
}

void CNet::CLoop(bool demon, s32 nFramems) {
    if (demon) {
        m_demo = true;
        HANDLE hThread = ::CreateThread(NULL, 0, CNet::DemonLoop, (LPVOID)this, 0, NULL);
        CloseHandle(hThread);
    } else {
        if (m_demo) {
            NET_ERROR("has already looped as demon");
            return;
        }
        m_nFrameMs = nFramems;
        DemonLoop(this);
    }
}

void CNet::CRemoteInfo(const s32 nConnectID, const char * & ip, s32 & nPort) {
    ip = m_ConnectPool[nConnectID]->szIP;
    nPort = m_ConnectPool[nConnectID]->nPort;
}

void CNet::CSetCallBackAddress(const CALLBACK_TYPE eType, const CALL_FUN address) {
    if (eType >= CALL_TYPE_COUNT) {
        NET_ERROR("unknown call type");
        ASSERT(false);
    } else {
        m_szCallAddress[eType] = address;
    }
}

#endif //#if defined WIN32 || defined WIN64
