#include "CSelectModel.h"
#include "Tools.h"
#include "CLock.h"

namespace tlib {
    CSelectModel::~CSelectModel() {
        Stop();
    }

    void CSelectModel::CSetCallFunAddr(const s8 nCallType, const CSCallFun pFunAddr) {
        ASSERT(nCallType < CALL_TYPE_COUNT && pFunAddr != NULL);

        m_pFunAddr[nCallType] = pFunAddr;
    }

    void CSelectModel::CConnect(const char * pStrIP, const s32 nPort, const void * pContext, const s32 nSize) {
        CSEvent * pEvent = m_EventPool.Create();
        ASSERT(pEvent);

        pEvent->nEventType = NET_EVENT_CONNECT;
        RemoteInfo info;
        SafeSprintf(info.szIP, sizeof(info.szIP), "%s", pStrIP);
        info.nPort = nPort;

        pEvent->stream.in(&info, sizeof(info));
        pEvent->stream.in(&nSize, sizeof(nSize));
        if (nSize != 0) {
            pEvent->stream.in(pContext, nSize);
        }

        m_Queue[QUEUE_TYPE_IN].add(pEvent);
    }

    void CSelectModel::CSend(const s32 nConnectID, const void * pContext, const s32 nSize) {
        if (m_ConnectionPool[nConnectID]->bShutdown || 0 == nSize) {
            return;
        }

        m_ConnectionPool[nConnectID]->stream.in(pContext, nSize);
    }


    void CSelectModel::CClose(const s32 nConnectID) {
        if (m_ConnectionPool[nConnectID]->bShutdown) {
            ASSERT(false);
            return;
        }
        m_ConnectionPool[nConnectID]->bShutdown = true;
        shutdown(m_ConnectionPool[nConnectID]->sock, SD_RECEIVE);
    }


    void CSelectModel::CContext(const s32 nConnectID, void * & pContext, s32 & nSize) {
        CConnection * pConnection = m_ConnectionPool[nConnectID];
        nSize = pConnection->context.size();
        if (0 == nSize) {
            pContext = NULL;
        } else {
            pContext = (void *)pConnection->context.buff();
        }
    }

    s64 CSelectModel::CLoop(const s64 lTick) {
        s64 lStartTick = ::GetCurrentTimeTick();
        CSEvent * pEvent = NULL;
        while (true) {
            if (m_Queue[QUENE_TYPE_OUT].read(pEvent)) {
                ASSERT(pEvent);
                switch(pEvent->nEventType) {
                case NET_EVENT_CONNECTED:
                    {
                        ASSERT(m_pFunAddr[CALL_CONNECTED]);
                        pEvent->stream.out(sizeof(RemoteInfo));
                        s32 nSize = pEvent->stream.size();
                        if (0 == nSize) {
                            m_pFunAddr[CALL_CONNECTED](pEvent->nConnectID, NULL, 0);
                        } else {
                            m_pFunAddr[CALL_CONNECTED](pEvent->nConnectID, pEvent->stream.buff(), nSize);
                        }
                    }
                    break;
                case NET_EVENT_CONNECT_FAILED:
                    {
                        ASSERT(m_pFunAddr[CALL_CONNECT_FAILED]);
                        pEvent->stream.out(sizeof(RemoteInfo));
                        s32 nSize = *(s32 *)(pEvent->stream.buff());
                        pEvent->stream.out(sizeof(nSize));
                        if (0 == nSize) {
                            m_pFunAddr[CALL_CONNECT_FAILED](pEvent->nConnectID, NULL, 0);
                        } else {
                            m_pFunAddr[CALL_CONNECT_FAILED](pEvent->nConnectID, pEvent->stream.buff(), nSize);
                        }
                    }
                    break;
                case NET_EVENT_RECV:
                    {
                        ASSERT(m_pFunAddr[CALL_RECV_DATA]);
                        m_pFunAddr[CALL_RECV_DATA](pEvent->nConnectID, pEvent->stream.buff(), pEvent->stream.size());
                    }
                    break;
                case NET_EVENT_CONNECTION_BREAK:
                    {
                        ASSERT(m_pFunAddr[CALL_CONNECTION_BREAK]);
                        CConnection * pConnection = m_ConnectionPool[pEvent->nConnectID];
                        s32 nSize = pConnection->context.size();
                        if (0 == nSize) {
                            m_pFunAddr[CALL_CONNECTION_BREAK](pEvent->nConnectID, NULL, 0);
                        } else {
                            m_pFunAddr[CALL_CONNECTION_BREAK](pEvent->nConnectID, pConnection->context.buff(), nSize);
                        }
                        pConnection->Clear();
                        m_ConnectionPool.Recover(pConnection);
                    }
                    break;
                default:
                    ASSERT(false);
                    break;
                }

                pEvent->stream.clear();
                m_EventPool.Recover(pEvent);
                pEvent = NULL;
            } else if (m_Queue[QUENE_TYPE_OUT].IsEmpty()){
                return ::GetCurrentTimeTick() - lStartTick;
            } else {
                s64 lRunTick = ::GetCurrentTimeTick() - lStartTick;
                if (lRunTick >= lTick) {
                    return lRunTick;
                }
            }
        }
    }

    void CSelectModel::CheckEvent() {
        CSEvent * pEvent = NULL;
        if (m_Queue[QUEUE_TYPE_IN].read(pEvent)) {
            ASSERT(pEvent);
            switch (pEvent->nEventType) {
            case NET_EVENT_CONNECT:
                {
                    RemoteInfo * pInfo = (RemoteInfo *)pEvent->stream.buff();
                    s32 sock = socket(AF_INET,SOCK_STREAM,0);
                    if (sock < 0) {
                        pEvent->nEventType = NET_EVENT_CONNECT_FAILED;
                        m_Queue[QUENE_TYPE_OUT].add(pEvent);
                        return;
                    }

                    struct sockaddr_in remote;
                    memset(&remote, 0, sizeof(remote));
                    remote.sin_family = AF_INET;
                    remote.sin_port = htons(pInfo->nPort);
                    remote.sin_addr.s_addr = inet_addr(pInfo->szIP);
                        
                    s32 nRet = connect(sock, (struct sockaddr *)&remote, sizeof(remote));
                        
                    if(nRet != 0){
                        pEvent->nEventType = NET_EVENT_CONNECT_FAILED;
                        m_Queue[QUENE_TYPE_OUT].add(pEvent);
                        return;
                    }
                    pEvent->stream.out(sizeof(RemoteInfo));
                        
                    CConnection * pConnection = m_ConnectionPool.Create();
                    ASSERT(pConnection);
                    pConnection->sock = sock;
                    memcpy(&(pConnection->remote), pInfo, sizeof(pConnection->remote));

                    s32 nSize = *(s32 *)(pEvent->stream.buff());
                    pEvent->stream.out(sizeof(nSize));

                    if (0 != nSize) {
                        pConnection->context.in(pEvent->stream.buff(), nSize);
                    }

                    pEvent->nEventType = NET_EVENT_CONNECTED;
                    pEvent->nConnectID = m_ConnectionPool.QueryID(pConnection);
                    pEvent->stream.clear();
                    pEvent->stream.in(pInfo, sizeof(RemoteInfo));
                    pEvent->stream.in(pConnection->context.buff(), pConnection->context.size());
                    m_Queue[QUENE_TYPE_OUT].add(pEvent);
                    CAutoLock autoLock(&m_lock);
                    m_Connection_List.push_back(pConnection);
                }
                break;
            default:
                ASSERT(false);
                break;
            }
        }
    }

    void CSelectModel::CheckRead() {
        CONNECTION_LIST temp_list;
        {
            CAutoLock autoLock(&m_lock);
            std::copy(m_Connection_List.begin(), m_Connection_List.end(), std::back_inserter(temp_list));
        }

        fd_set fdread;
        FD_ZERO(&fdread);
        CONNECTION_LIST::iterator itor = temp_list.begin();
        CONNECTION_LIST::iterator iend = temp_list.end();
        if (itor == iend) {
            return;
        }
        s32 maxfdp = 0;
        while (itor != iend) {
            FD_SET((*itor)->sock, &fdread);
            if ((*itor)->sock > maxfdp) {
                maxfdp = (*itor)->sock;
            }
            itor ++;
        }

        maxfdp++;

        // We only care read event
        struct timeval tv = {0, 10};
        s32 ret = select(maxfdp, &fdread, NULL, NULL, &tv);
        if (0 < ret) {
            itor = temp_list.begin();
            char szBuff[BUFF_SIZE] = {0};

            while (itor != iend) {
                if (FD_ISSET((*itor)->sock, &fdread)) {
                    CConnection * pConnection = (*itor);
                    memset(szBuff, 0, sizeof(szBuff));
                    s32 nRevRet = recv(pConnection->sock, szBuff, BUFF_SIZE, 0);
                    CSEvent * pEvent = NULL;
                    pEvent = m_EventPool.Create();
                    ASSERT(pEvent);
                    if (nRevRet <= 0) {
                        // Client socket closed, add connection break event
                        pEvent->nConnectID = m_ConnectionPool.QueryID(pConnection);
                        pEvent->nEventType = NET_EVENT_CONNECTION_BREAK;
                        pConnection->bShutdown = true;
                        //ECHO_TRACE("Connection %d socket %d closed.", m_ConnectionPool.QueryID((*itor)), (*itor)->sock);
                        shutdown(pConnection->sock, SD_BOTH);
                        closesocket(pConnection->sock);
                        RemoveConnection(pConnection);
                        itor = temp_list.erase(itor);
                        m_Queue[QUENE_TYPE_OUT].add(pEvent);
                        continue;
                    } else {
                        pEvent->nConnectID = m_ConnectionPool.QueryID(pConnection);
                        pEvent->nEventType = NET_EVENT_RECV;
                        pEvent->stream.in(szBuff, nRevRet);
                        m_Queue[QUENE_TYPE_OUT].add(pEvent);
                    }
                }

                itor++;
            }
        }
    }

    void CSelectModel::CheckWrite() {
        CONNECTION_LIST temp_list;
        {
            CAutoLock autoLock(&m_lock);
            std::copy(m_Connection_List.begin(), m_Connection_List.end(), std::back_inserter(temp_list));
        }

        fd_set fdread;
        FD_ZERO(&fdread);
        CONNECTION_LIST::iterator itor = temp_list.begin();
        CONNECTION_LIST::iterator iend = temp_list.end();
        if (itor == iend) {
            return;
        }

        s32 maxfdp = 0;
        while (itor != iend) {
            FD_SET((*itor)->sock, &fdread);
            if ((*itor)->sock > maxfdp) {
                maxfdp = (*itor)->sock;
            }
            itor ++;
        }

        maxfdp++;
        struct timeval tv = {0, 10};
        s32 ret = select(maxfdp, NULL, &fdread, NULL, &tv);
        if (0 < ret) {
            itor = temp_list.begin();
            while (itor != iend) {
                CConnection * pConnection = (*itor);
                s32 nSize = pConnection->stream.size();
                if (nSize > 0) {
                    s32 nSendRet  = send(pConnection->sock, pConnection->stream.buff(), pConnection->stream.size(), 0);
                    if (nSendRet < 0) {
                        CSEvent * pEvent = NULL;
                        pEvent = m_EventPool.Create();
                        ASSERT(pEvent);
                        // Client socket closed, add connection break event
                        pEvent->nConnectID = m_ConnectionPool.QueryID(pConnection);
                        pEvent->nEventType = NET_EVENT_CONNECTION_BREAK;
                        pConnection->bShutdown = true;
                        //ECHO_TRACE("Connection %d socket %d closed.", m_ConnectionPool.QueryID((*itor)), (*itor)->sock);
                        shutdown(pConnection->sock, SD_BOTH);
                        closesocket(pConnection->sock);
                        RemoveConnection(pConnection);
                        itor = temp_list.erase(itor);
                        m_Queue[QUENE_TYPE_OUT].add(pEvent);
                        continue;
                    }
                    pConnection->stream.out(nSendRet);
                }

                itor++;
            }
        }
    }

    void CSelectModel::RemoveConnection(CConnection * pConnection) {
        CAutoLock autoLock(&m_lock);
        CONNECTION_LIST::iterator itor = m_Connection_List.begin();
        CONNECTION_LIST::iterator iend = m_Connection_List.end();
        while (itor != iend) {
            if ((*itor)->sock == pConnection->sock) {
                //m_ConnectionPool.Recover(*itor);
                m_Connection_List.erase(itor);
                return;
            }
            itor ++;
        }
    }

    void CSelectModel::ClearHandle() {
        CONNECTION_LIST::iterator itor = m_Connection_List.begin();
        CONNECTION_LIST::iterator iend = m_Connection_List.end();

        while (itor != iend) {
            shutdown((*itor)->sock, SD_BOTH);
            closesocket((*itor)->sock);
            itor++;
        }

        m_Connection_List.clear();
    }

    void CSelectModel::Start() {
        m_bShutdown = false;
        m_nThreadCount = 0;
#if defined WIN32 || defined WIN64
        HANDLE hRThread = ::CreateThread(NULL, 0, CSelectModel::ReadThread, (LPVOID)this, 0, NULL);
        CloseHandle(hRThread);
        HANDLE hWThread = ::CreateThread(NULL, 0, CSelectModel::WriteThread, (LPVOID)this, 0, NULL);
        CloseHandle(hWThread);
#endif //defined WIN32 || defined WIN64
        
#if defined linux || defined CC_TARGET_OS_IPHONE
        pthread_t id;
        s32 hRThread = pthread_create(&id, NULL, CSelectModel::ReadThread, this);
        ASSERT(hRThread == 0);
        s32 hWThread = pthread_create(&id, NULL, CSelectModel::WriteThread, this);
        ASSERT(hWThread == 0);
#endif //defined linux || defined CC_TARGET_OS_IPHONE
    }
    
    void CSelectModel::Stop() {
        if (!m_bShutdown) {
            m_bShutdown = true;
            while (m_nThreadCount != 0) {
                CSleep(1);
            }
        }
    }

    THREAD_FUN CSelectModel::ReadThread(void * p) {
        CSelectModel * pThis = (CSelectModel *) p;
        ASSERT(pThis);
        {
            CAutoLock autoLock(&pThis->m_lock);
            pThis->m_nThreadCount++;
        }

        while (true) {
            if (pThis->m_bShutdown) {
                CAutoLock autoLock(&pThis->m_lock);
                pThis->ClearHandle();
                pThis->m_nThreadCount--;
                return 0;
            }

            pThis->CheckEvent();
            pThis->CheckRead();
            CSleep(0);
        }

        return 0;
    }

    THREAD_FUN CSelectModel::WriteThread(void * p) {
        CSelectModel * pThis = (CSelectModel *) p;
        ASSERT(pThis);
        {
            CAutoLock autoLock(&pThis->m_lock);
            pThis->m_nThreadCount++;
        }

        while (true) {
            if (pThis->m_bShutdown) {
                CAutoLock autoLock(&pThis->m_lock);
                pThis->ClearHandle();
                pThis->m_nThreadCount--;
                return 0;
            }

            pThis->CheckWrite();
            CSleep(0);
        }

        return 0;
    }
}