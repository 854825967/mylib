#include "CHttpEngine.h"
#include "CURL/curl.h"
#include "Tools.h"

enum {
    HTTP_REQUEST_GET,
    HTTP_REQUEST_POST,
    //HTTP_REQUEST_HTTPS,

    //ADD BEFORE THIS
    HTTP_REQUEST_TYPE_COUNT
};

enum {
    NO_CONTEXT = 0,
    HAS_CONTEXT = 1,
};

namespace core {
    bool CHttpEngine::HGetRequest(const s32 id, const char * url, const s32 urlsize, IHttpHandle * pHandler, const void * context, const s32 contextsize) {
        if (m_shutdown) {
            return false;
        }

        ASSERT(url != NULL && urlsize != 0);
        static const s8 type = HTTP_REQUEST_GET;
        static s32 queueid = 0;
        
        TStream<1024, false> * pStream = m_CStreamPool.Create();
        ASSERT(pStream);
        pStream->in(&id, sizeof(id));

        pStream->in(&type, sizeof(type));

        pStream->in(&urlsize, sizeof(urlsize));
        pStream->in(url, urlsize);

        pStream->in(&pHandler, sizeof(pHandler));

        if (context != NULL && contextsize != 0) {
            s8 tag = HAS_CONTEXT;
            pStream->in(&tag, sizeof(tag));

            TStream<1024, false> * pContext = m_CStreamPool.Create();
            ASSERT(pContext);
            pContext->in(context, contextsize);
            s32 nIndex = m_CStreamPool.QueryID(pContext);
            
            pStream->in(&nIndex, sizeof(nIndex));
        } else {
            s8 tag = NO_CONTEXT;
            pStream->in(&tag, sizeof(tag));
        }

        return m_pHttpQueue[queueid++ % m_nThreadCount].add2(pStream);
    }

    bool CHttpEngine::HPostRequest(const s32 id, const char * url, const s32 urlsize, const void * data, const s32 datasize, IHttpHandle * pHandler, const void * context, const s32 contextsize) {
        if (m_shutdown) {
            return false;
        }

        ASSERT(url != NULL 
            && urlsize != 0
            && data != NULL
            && datasize != NULL);

        static const s8 type = HTTP_REQUEST_POST;
        static s32 queueid = 0;

        TStream<1024, false> * pStream = m_CStreamPool.Create();
        ASSERT(pStream);

        pStream->in(&id, sizeof(id));

        pStream->in(&type, sizeof(type));

        pStream->in(&urlsize, sizeof(urlsize));
        pStream->in(url, urlsize);

        pStream->in(&pHandler, sizeof(pHandler));

        if (context != NULL && contextsize != 0) {
            s8 tag = HAS_CONTEXT;
            pStream->in(&tag, sizeof(tag));

            TStream<1024, false> * pContext = m_CStreamPool.Create();
            ASSERT(pContext);
            pContext->in(context, contextsize);
            s32 nIndex = m_CStreamPool.QueryID(pContext);

            pStream->in(&nIndex, sizeof(nIndex));
        } else {
            s8 tag = NO_CONTEXT;
            pStream->in(&tag, sizeof(tag));
        }

        pStream->in(&datasize, sizeof(datasize));
        pStream->in(data, datasize);

        return m_pHttpQueue[queueid++ % m_nThreadCount].add2(pStream);
    }

    s64 CHttpEngine::HttpBackCall(const s64 lTick) {
        s64 lStartTick = ::GetCurrentTimeTick();
        TStream<1024, false> * pStream = NULL;

        while (true) {
            if (m_backQueue.read(pStream)) {
                s32 nID = *(s32 *)(pStream->buff());
                pStream->out(sizeof(nID));

                IHttpHandle * pHandler = *(IHttpHandle ** )(pStream->buff());
                pStream->out(sizeof(pHandler));

                s8 tag = *(s8 *)(pStream->buff());
                pStream->out(sizeof(tag));

                TStream<1024, false> * pContext = NULL;

                if (HAS_CONTEXT == tag) {
                    s32 nIndex = *(s32 *)(pStream->buff());
                    pStream->out(sizeof(nIndex));
                    pContext = m_CStreamPool[nIndex];
                }

                s32 nResCode = *(s32 *)(pStream->buff());
                pStream->out(sizeof(nResCode));
                
                s32 nLen = 0;
                void * pData = NULL;
                if (CURLE_OK == nResCode) {
                    nLen = *(s32 *)(pStream->buff());
                    pStream->out(sizeof(nLen));
                    pData = (void *)pStream->buff();
                }

                if (nLen > 0) {
                    nLen--;
                }

                if (pHandler != NULL) {
                    if (pContext != NULL) {
                        pHandler->OnHttpBack(nID, nResCode, pData, nLen, pContext->buff(), pContext->size());
                    } else {
                        pHandler->OnHttpBack(nID, nResCode, pData, nLen, NULL, 0);
                    }
                }

                if (pContext != NULL) {
                    pContext->clear();
                    m_CStreamPool.Recover(pContext);
                }

                pStream->clear();
                m_CStreamPool.Recover(pStream);
            } else if (m_backQueue.IsEmpty()) {
                return ::GetCurrentTimeTick() - lStartTick;
            }

            s64 lRunTick = ::GetCurrentTimeTick() - lStartTick;
            if (lRunTick >= lTick) {
                return lRunTick;
            }
        }
    }
    
    CHttpEngine::~CHttpEngine() {
        Stop();

        delete[] m_pHttpQueue;
    }

    struct ThreadArgs {
        CHttpEngine * pHttpEngine;
        s32 nQueueID;

        ThreadArgs() {
            memset(this, 0, sizeof(*this));
        }
    };

    void CHttpEngine::Start(const s8 nThreadCount) {
        m_shutdown = false;
        m_nThreadCount = nThreadCount;
        m_pHttpQueue = NEW TQueue<TStream<1024, false> *, false, HTTP_QUEUE_SIZE>[nThreadCount];
        for (s32 i=0; i<nThreadCount; i++) {
            ThreadArgs * pArgs = NEW ThreadArgs;
            pArgs->pHttpEngine = this;
            pArgs->nQueueID = i;

            HANDLE hThread = ::CreateThread(NULL, 0, CHttpEngine::HttpThread, (LPVOID)pArgs, 0, NULL);
            CloseHandle(hThread);
        }

    }

    void CHttpEngine::Stop() {
        if (!m_shutdown) {
            m_shutdown = true;
            while (m_nThreadCount != 0){
                CSleep(10);
            }
            HttpBackCall(9999999);
        }
    }

    static size_t write_callback(void *ptr, size_t size, size_t nmemb, void * stream) {
        ((TStream<1024, false> *)stream)->in(ptr, nmemb * size);
        char end = '\0';
        ((TStream<1024, false> *)stream)->in(&end, sizeof(end));

        return nmemb * size;
    }  

    THREAD_FUN CHttpEngine::HttpThread(void * p) {
        CHttpEngine * pThis = ((ThreadArgs *) p)->pHttpEngine;
        s32 nQueueID = ((ThreadArgs *) p)->nQueueID;
        TQueue<TStream<1024, false> *, false, HTTP_QUEUE_SIZE> * pQueue = &(pThis->m_pHttpQueue[nQueueID]);
        TStream<1024, false> * pStream = NULL;

        CURL * pCurlHandle = curl_easy_init();

        while (true) {
            if (pQueue->read(pStream)) {
                curl_easy_reset(pCurlHandle);
                curl_easy_setopt(pCurlHandle, CURLOPT_TIMEOUT, 10);
                curl_easy_setopt(pCurlHandle, CURLOPT_VERBOSE, 0L);  
                curl_easy_setopt(pCurlHandle, CURLOPT_WRITEFUNCTION, write_callback); 

                s32 nID = *(s32 *)(pStream->buff());
                pStream->out(sizeof(nID));

                s8 nType = *(s8 *)(pStream->buff());
                pStream->out(sizeof(nType));

                s32 nUrlLen = *(s32 *)(pStream->buff());
                pStream->out(sizeof(nUrlLen));

                const char * pUrl = (const char *)(pStream->buff());
                pStream->out(nUrlLen);

                IHttpHandle * pHandler = *(IHttpHandle **)(pStream->buff());
                pStream->out(sizeof(pHandler));

                s8 tag = *(s8 *)(pStream->buff());
                pStream->out(sizeof(tag));
                s32 nIndex;
                if (HAS_CONTEXT== tag) {
                    nIndex = *(s32 *)(pStream->buff());
                    pStream->out(sizeof(nIndex));
                }

                curl_easy_setopt(pCurlHandle, CURLOPT_URL, pUrl);

                s32 nError = 0;
                const void * pReturn = NULL;
                s32 nReturnSize = 0;

                TStream<1024, false> * pBack = pThis->m_CStreamPool.Create();
                //这个变量可作为接收或传递数据的作用
                curl_easy_setopt(pCurlHandle, CURLOPT_WRITEDATA, pBack); 

                s32 nResCode = 0;
                switch (nType) {
                case HTTP_REQUEST_GET:
                    {
                        //nResCode = curl_easy_perform(pCurlHandle);
                    }
                    break;
                case HTTP_REQUEST_POST:
                    {
                        s32 nDataLen = *(s32 *)(pStream->buff());
                        pStream->out(sizeof(nDataLen));
                        curl_easy_setopt(pCurlHandle, CURLOPT_POSTFIELDS, pStream->buff(), nDataLen);
                    }
                    break;
                }

                nResCode = curl_easy_perform(pCurlHandle);
                
                pStream->clear();
                pStream->in(&nID, sizeof(nID));
                pStream->in(&pHandler, sizeof(pHandler));
                pStream->in(&tag, sizeof(tag));
                if (HAS_CONTEXT == tag) {
                    pStream->in(&nIndex, sizeof(nIndex));
                }

                pStream->in(&nResCode, sizeof(nResCode));
                if (CURLE_OK == nResCode) {
                    s32 nLen = pBack->size();
                    pStream->in(&nLen, sizeof(nLen));
                    pStream->in(pBack->buff(), nLen);
                }
                pBack->clear();
                pThis->m_CStreamPool.Recover(pBack);
                pThis->m_backQueue.add(pStream);
            } else if (pQueue->IsEmpty() && pThis->m_shutdown){
                curl_easy_cleanup(pCurlHandle);
                delete p;
                CAutoLock(&(pThis->m_freelock));
                pThis->m_nThreadCount--;
                return 0;
            } else {
                CSleep(10);
            }
        }


    }
}
