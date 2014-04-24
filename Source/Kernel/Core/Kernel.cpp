#include "Kernel.h"
#include "CoreHeader.h"


IKernel * IModule::s_pKernel = NULL;

namespace core {
    static s8 s_nNetThreadCount = 0;
    static s64 s_lNetFrameTick = 0;
    static s8 s_nHttpThreadCount = 0;
    static s64 s_lHttpFrameTick = 0;

    Kernel * Kernel::s_pKernel = NULL;

    void Kernel::NewConnection(const s32 nConnectID, const void * pContext, const s32 nSize) {

        g_pEventCallEngine->Call(EVENT_TYPE_CORE, CORE_EVENT_REMOTE_CONNECTED, EVENT_TYPE_CORE, nConnectID, pContext, nSize);
    }

    void Kernel::Recv(const s32 nConnectID, const void * pContext, const s32 nSize) {
        g_pEventCallEngine->Call(EVENT_TYPE_CORE, CORE_EVENT_RECV_DATA, EVENT_TYPE_CORE, nConnectID, pContext, nSize);
    }

    void Kernel::ConnectionBreak(const s32 nConnectID, const void * pContext, const s32 nSize) {

        g_pEventCallEngine->Call(EVENT_TYPE_CORE, CORE_EVENT_CONNECTION_BREAK, EVENT_TYPE_CORE, nConnectID, pContext, nSize);
    }

    void Kernel::Connected(const s32 nConnectID, const void * pContext, const s32 nSize) {

        g_pEventCallEngine->Call(EVENT_TYPE_CORE, CORE_EVENT_CONNECTED, EVENT_TYPE_CORE, nConnectID, pContext, nSize);
    }

    void Kernel::Connectfaild(const s32 nConnectID, const void * pContext, const s32 nSize) {

        g_pEventCallEngine->Call(EVENT_TYPE_CORE, CORE_EVENT_CONNECT_FAILED, EVENT_TYPE_CORE, nConnectID, pContext, nSize);
    }

    Kernel * Kernel::GetKernel() {
        if (NULL == s_pKernel) {
            s_pKernel = NEW Kernel;
            ASSERT(s_pKernel);
        }

        return s_pKernel;
    }

    Kernel::Kernel() {

    }

    void Kernel::Start() {
        {
            char pStrPath[512] = {0};
            SafeSprintf(pStrPath, sizeof(pStrPath), "%s/CoreConfig/core.xml", ::GetAppPath());
            
            m_CoreConfig.LoadFile(pStrPath);
            TiXmlElement * pRootElement = m_CoreConfig.FirstChildElement("core");
            {
                TiXmlElement * pElement = pRootElement->FirstChildElement("net");
                TiXmlAttribute * pThreadCount = pElement->FirstAttribute();
                TiXmlAttribute * pFrameTick = pThreadCount->Next();

                s_nNetThreadCount = pThreadCount->IntValue();
                s_lNetFrameTick = pFrameTick->IntValue();

                g_pNetEngine->CNetInitialize(s_nNetThreadCount, s_lNetFrameTick);


                g_pNetEngine->CSetCallBackAddress(CALL_REMOTE_CONNECTED, Kernel::NewConnection);
                g_pNetEngine->CSetCallBackAddress(CALL_CONNECT_FAILED, Kernel::Connectfaild);
                g_pNetEngine->CSetCallBackAddress(CALL_CONNECTED, Kernel::Connected);
                g_pNetEngine->CSetCallBackAddress(CALL_CONNECTION_BREAK, Kernel::ConnectionBreak);
                g_pNetEngine->CSetCallBackAddress(CALL_RECV_DATA, Kernel::Recv);
            }

            {
                TiXmlElement * pElement = pRootElement->FirstChildElement("http");
                TiXmlAttribute * pThreadCount = pElement->FirstAttribute();
                TiXmlAttribute * pFrameTick = pThreadCount->Next();

                s_nHttpThreadCount = pThreadCount->IntValue();
                s_lHttpFrameTick = pFrameTick->IntValue();

                g_pHttpEngine->Start(s_nHttpThreadCount);
            }

            {
                TiXmlElement * pElement = pRootElement->FirstChildElement("listen_list");
                TiXmlElement * pListen = pElement->FirstChildElement("listen");
                while (pListen) {
                    TiXmlAttribute * pType = pListen->FirstAttribute();
                    TiXmlAttribute * pIp = pType->Next();
                    TiXmlAttribute * pPort = pIp->Next();

                    g_pNetEngine->CListen(pIp->Value(), pPort->IntValue(), pType->Value());
                    pListen = pListen->NextSiblingElement();
                }
            }

            {
                TiXmlElement * pElement = pRootElement->FirstChildElement("connect_list");
                TiXmlElement * pListen = pElement->FirstChildElement("connect");
                while (pListen) {
                    TiXmlAttribute * pType = pListen->FirstAttribute();
                    TiXmlAttribute * pIp = pType->Next();
                    TiXmlAttribute * pPort = pIp->Next();

                    g_pNetEngine->CConnectEx(pIp->Value(), pPort->IntValue(), pType->Value());
                    pListen = pListen->NextSiblingElement();
                }
            }
        }

        {
            char pStrPath[512] = {0};
            memset(pStrPath, 0, sizeof(pStrPath));
            SafeSprintf(pStrPath, sizeof(pStrPath), "%s/CoreConfig/module.xml", ::GetAppPath());
            m_ModulesConfig.LoadFile(pStrPath);

            TiXmlElement * pRootElement = m_ModulesConfig.RootElement();
            const char * pModulesDir = pRootElement->Attribute("path");
            TiXmlElement * pModulesElement = pRootElement->FirstChildElement("module");
            while (pModulesElement != NULL) {
                const char * pModuleName = pModulesElement->Attribute("name");
                ASSERT(pModuleName);
                memset(pStrPath, 0, sizeof(pStrPath));
                SafeSprintf(pStrPath, sizeof(pStrPath), "%s/%s/%s", ::GetAppPath(), pModulesDir, pModuleName);
                ECHO("加载 %s", pStrPath);


                HINSTANCE hinst = ::LoadLibrary(pStrPath);
                GET_LOGIC_MODULE pFunGetModule = (GET_LOGIC_MODULE)::GetProcAddress(hinst, FUN_NAME_GETLOGICMODULE);
                ASSERT(pFunGetModule);
                if (NULL == pFunGetModule) {
                    ECHO_ERROR("Load Modules %s Error", pModuleName);
                    return;
                }

                IModule * pModule = pFunGetModule();
                while (pModule != NULL) {
                    m_Modules_List.push_back(pModule);
                    pModule->Initialize(this);
                    ECHO("%s Initialize", pModule->GetName());
                    m_Modules_Map.insert(make_pair(pModule->GetName(), pModule));
                    pModule = pModule->GetNext();
                }

                pModulesElement = pModulesElement->NextSiblingElement();
            }
        }
        m_bStop = false;
    }

    void Kernel::Loop() {
        while (!m_bStop) {
            g_pNetEngine->CLoop(false, s_lNetFrameTick);
            g_pHttpEngine->HttpBackCall(s_lHttpFrameTick);
            g_pTimeEngine->OnTimer(10);
            CSleep(0);
        }
    }

    void Kernel::Release() {
        MODULES_LIST::reverse_iterator ritor = m_Modules_List.rbegin();
        while (ritor != m_Modules_List.rend()) {
            (*ritor)->Destroy(this);
            ritor++;
        }

        ritor = m_Modules_List.rbegin();        
        while (ritor != m_Modules_List.rend()) {
            delete (*ritor);
            ritor++;
        }

        m_Modules_List.clear();
    }

    void Kernel::Stop() {
        if (!m_bStop) {
            g_pHttpEngine->Stop();
            g_pNetEngine->CStop();
            m_bStop = true;
        }
    }

    IModule * Kernel::FindModule(const char * pModuleName) {
        MODULES_MAP::iterator itor = m_Modules_Map.find(pModuleName);
        if (itor != m_Modules_Map.end()) {
            return itor->second;
        }

        return NULL;
    }

    bool Kernel::HGetRequest(const s32 id, const char * url, const s32 urlsize, 
        IHttpHandle * pHandler, const void * context, const s32 contextsize) {
        return g_pHttpEngine->HGetRequest(id, url, urlsize, pHandler, context, contextsize);
    }

    bool Kernel::HPostRequest(const s32 id, const char * url, const s32 urlsize, const void * data, const s32 datasize, 
        IHttpHandle * pHandler, const void * context, const s32 contextsize) {
        return g_pHttpEngine->HPostRequest(id, url, urlsize, data, datasize, pHandler, context, contextsize);
    }

    void Kernel::CallEvnet(const s8 type, const s32 nEventID, const void * pContext, const s32 nSize) {
        ASSERT(type > EVENT_TYPE_CORE);
        g_pEventCallEngine->Call(type, nEventID, type, nEventID, pContext, nSize);
    }

    void Kernel::Send(const s32 nConnectID, const void * pBuff, const s32 nSize) {
        g_pNetEngine->CSend(nConnectID, pBuff, nSize);
    }

    void * Kernel::CContext(const s32 nConnectID) {
        void * pContext = NULL;
        g_pNetEngine->CContext(nConnectID, pContext);
        return pContext;
    }


    void Kernel::RemoteInfo(const s32 nConnectID, const char * & pIP, s32 & nPort) {
        g_pNetEngine->CRemoteInfo(nConnectID, pIP, nPort);
    }

    bool Kernel::SetTimer(const s32 id, const s32 tickcount, const s32 calltimes, ITimerHandler * pHandler, const char * debugstr) {
        //同一个ID只能注册一次 
        return g_pTimeEngine->SetTimer(id, tickcount, calltimes, pHandler, debugstr);
    }

    bool Kernel::KillTimer(const s32 id, ITimerHandler * pHandler) {
        return g_pTimeEngine->KillTimer(id, pHandler);
    }

    bool Kernel::KillTimer(ITimerHandler * pHandler) {
        return g_pTimeEngine->KillTimer(pHandler);
    }
    
    void Kernel::AddEventCall(const s8 nTypeID, const s32 nEventID, const EVENTCALLFUN pCall, const char * pStrCallName) {
        g_pEventCallEngine->RegsCall(nTypeID, nEventID, pCall, pStrCallName);
    }

    void Kernel::RemoveEventCall(const s8 nTypeID, const s32 nEventID, const EVENTCALLFUN pCall) {
        g_pEventCallEngine->UnRegsCall(nTypeID, nEventID, pCall);
    }

    void Kernel::Trace(const char * log) {
        g_pCoreLogger->Log(LOG_LOGIC, log);
    }

}
