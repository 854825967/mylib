#include "IKernel.h"
#include "IModule.h"
#include "CoreHeader.h"
#include "tinyxml/tinyxml.h"
using namespace tlib;

namespace core {
    typedef vector<IModule *> MODULES_LIST;
    typedef CHashMap<string, IModule *> MODULES_MAP;

    class Kernel : public IKernel {
    public:
        static Kernel * GetKernel();
        void Start();
        void Loop();
        void Release();

        //日志
        virtual void Trace(const char * log);

        //网络方面接口
        virtual void Send(const s32 nConnectID, const void * pBuff, const s32 nSize);
        virtual void * CContext(const s32 nConnectID);

        virtual void RemoteInfo(const s32 nConnectID, const char * & pIP, s32 & nPort);

        virtual bool SetTimer(const s32 id, const s32 tickcount, const s32 calltimes, ITimerHandler * pHandler, const char * debugstr);  //同一个ID只能注册一次
        virtual bool KillTimer(const s32 id, ITimerHandler * pHandler);
        virtual bool KillTimer(ITimerHandler * pHandler); 

        virtual void AddEventCall(const s8 nTypeID, const s32 nEventID, const EVENTCALLFUN pCall, const char * pStrCallName);
        virtual void RemoveEventCall(const s8 nTypeID, const s32 nEventID, const EVENTCALLFUN pCall);

        virtual void CallEvnet(const s8 type, const s32 nEventID, const void * pContext, const s32 nSize);

        virtual bool HGetRequest(const s32 id, const char * url, const s32 urlsize, 
            IHttpHandle * pHandler, const void * context, const s32 contextsize);
        virtual bool HPostRequest(const s32 id, const char * url, const s32 urlsize, const void * data, const s32 datasize, 
            IHttpHandle * pHandler, const void * context, const s32 contextsize);
        virtual void Stop();

        virtual IModule * FindModule(const char * pModuleName);

    private:
        static void NewConnection(const s32 nConnectID, const void * pContext, const s32 nSize);
        static void Recv(const s32 nConnectID, const void * pContext, const s32 nSize);
        static void ConnectionBreak(const s32 nConnectID, const void * pContext, const s32 nSize);
        static void Connected(const s32 nConnectID, const void * pContext, const s32 nSize);
        static void Connectfaild(const s32 nConnectID, const void * pContext, const s32 nSize);
    private:
        Kernel();
        static Kernel * s_pKernel;

        bool m_bStop;
        TiXmlDocument m_CoreConfig;
        TiXmlDocument m_ModulesConfig;
        MODULES_LIST m_Modules_List;
        MODULES_MAP m_Modules_Map;
    };
}