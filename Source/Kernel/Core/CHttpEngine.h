#ifndef CHTTPENGINE_H
#define CHTTPENGINE_H

#include "IHttpEngine.h"
#include "TQueue.h"
#include "TPool.h"
#include "CHashMap.h"
#include "CLock.h"
using namespace tlib;

#define HTTP_QUEUE_SIZE 1024

namespace core {
    
    typedef CHashMap<s64, TStream<1024, false> *> MAP_ARGS;

    class CHttpEngine : public IHttpEngine {
    public:
        virtual bool HGetRequest(const s32 id, const char * url, const s32 urlsize, IHttpHandle * pHandler, const void * context, const s32 contextsize);
        virtual bool HPostRequest(const s32 id, const char * url, const s32 urlsize, const void * data, const s32 datasize, IHttpHandle * pHandler, const void * context, const s32 contextsize);
        virtual s64 HttpBackCall(const s64 tick);
        
        virtual void Start(const s8 nThreadCount);
        virtual void Stop();

        ~CHttpEngine();

    private:
        static THREAD_FUN HttpThread(void * p);

    private:
        s32 m_nThreadCount;
        TQueue<TStream<1024, false> *, false, HTTP_QUEUE_SIZE> * m_pHttpQueue;
        TQueue<TStream<1024, false> *, true, HTTP_QUEUE_SIZE> m_backQueue;
        TPOOL<TStream<1024, false>, true, 10240> m_CStreamPool;

        MAP_ARGS m_Map_BackArgs;
        bool m_shutdown;
        CLockUnit m_freelock;
    };
}

#endif //CHTTPENGINE_H
