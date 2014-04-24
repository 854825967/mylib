#ifndef CSELECTMODEL_H
#define CSELECTMODEL_H
#include "MultiSys.h"
#include "TQueue.h"
#include "TPool.h"
#include "TStream.h"
#include <vector>
using namespace std;

namespace tlib {
    enum {
        NET_EVENT_CONNECTED = 0,
        NET_EVENT_CONNECT_FAILED,
        NET_EVENT_CONNECTION_BREAK,
        NET_EVENT_RECV,

        //add befor this
        NET_EVENT_END,
        NET_EVENT_CONNECT,
        NET_EVENT_DISCONNECTED,
    };

    struct CSEvent {
        s8 nEventType;
        s32 nConnectID;
        TStream<4096, false> stream;
    };

    typedef enum call_type {
        CALL_CONNECTED = 0, /** 连接远端成功 */
        CALL_CONNECT_FAILED, /** 连接远端失败 */
        CALL_CONNECTION_BREAK, /** 连接断开 */
        CALL_RECV_DATA,  /** 新的数据到达 */

        /** add call type before this */
        CALL_TYPE_COUNT
    }CALLBACK_TYPE;

    enum {
        QUEUE_TYPE_IN = 0,
        QUENE_TYPE_OUT = 1,

        //ADD BEFORE THIS
        QUEUE_TYPE_COUNT
    };

    struct RemoteInfo {
        char szIP[64];
        s32 nPort;

        RemoteInfo() {
            memset(this, 0, sizeof(*this));
        }
    };

    class CConnection {
    public:
        s32 sock;
        RemoteInfo remote;
        bool bShutdown;
        TStream<4096, true> stream;
        TStream<256, false> context;
        CConnection() {
            Clear();
        }

        void Clear() {
            sock = -1;
            bShutdown = false;
            stream.clear();
            context.clear();
            memset(&remote, 0, sizeof(remote));
        }
    };
    
    typedef list<CConnection *> CONNECTION_LIST;

    enum {
        WORK_THREAD_WORK = 0,
        WORK_THREAD_STOPING = 1,
        WORK_THREAD_STOPED = 2
    };

    typedef s32 (*CSCallFun)(const s32, const void *, const s32);

    class CSelectModel {
    public:
        void CSetCallFunAddr(const s8 nCallType, const CSCallFun pFunAddr);
        void CContext(const s32 nConnectID, void * & pContext, s32 & nSize);
        void CConnect(const char * pStrIP, const s32 nPort, const void * pContext, const s32 nSize);
        void CSend(const s32 nConnectID, const void * pContext, const s32 nSize);
        void CClose(const s32 nConnectID);
        s64 CLoop(const s64 lTick);

        void Start();
        void Stop();

        ~CSelectModel();

    private:
        void CheckEvent();
        void CheckRead();
        void CheckWrite();

        void ClearHandle();
        void RemoveConnection(CConnection * pConnection);

    private:
        static THREAD_FUN ReadThread(void * p);
        static THREAD_FUN WriteThread(void * p);
    private:
        TPOOL<CConnection, true, 64> m_ConnectionPool;
        TPOOL<CSEvent, true, 512> m_EventPool;
        TQueue<CSEvent *, true, 256> m_Queue[QUEUE_TYPE_COUNT];
        CONNECTION_LIST m_Connection_List;
        bool m_bShutdown;
        s32 m_nThreadCount;
        CLockUnit m_lock;
        CSCallFun m_pFunAddr[CALL_TYPE_COUNT];
    };
}

#endif //CSELECTMODEL_H
