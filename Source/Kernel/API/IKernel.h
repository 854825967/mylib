/*
 * @defgroup 模块名 模块的说明文字

 * @{

 */
 
#include "MultiSys.h"
#include "IModule.h"
#include "ITimerHandler.h"
#include "IHttpHandler.h"

enum error_code {
    KERNEL_ERROR_CODE_MAX   /** @brief 简要说明文字（在前面加 @brief 是标准格式） */
};

#define ANY_CALL 0
#define EVENTCALL_ARGS const s8, const s32, const void *, const s32
typedef s32 (*EVENTCALLFUN)(EVENTCALL_ARGS);

enum EVENT_TYPE {
    EVENT_TYPE_CORE = 0,
    EVENT_TYPE_LOGIC = 1,

    //add before this
    EVENT_TYPE_COUNT
};

enum CORE_EVENT {
    CORE_EVENT_CONNECTED = 0, /** 连接远端成功 */
    CORE_EVENT_CONNECT_FAILED, /** 连接远端失败 */
    CORE_EVENT_REMOTE_CONNECTED, /** 新的远端连接进来 */
    CORE_EVENT_CONNECTION_BREAK, /** 连接断开 */
    CORE_EVENT_RECV_DATA
};

#define RGS_EVENT_CALL(type, id, call) \
     s_pKernel->AddEventCall(type, id, call, #call);

#define CONNECTION_BREAK -1


#define LOGIC_TRACE(format, ...) {\
    char _log[4096] = {0}; \
    memset(_log, 0, sizeof(_log)); \
    SafeSprintf(_log, sizeof(_log), "Trace : "##format, ##__VA_ARGS__); \
    s_pKernel->Trace(_log); \
    }

#define LOGIC_DEBUG(format, ...) {\
    char _log[4096] = {0}; \
    memset(_log, 0, sizeof(_log)); \
    SafeSprintf(_log, sizeof(_log), "Debug : "##format, ##__VA_ARGS__); \
    s_pKernel->Trace(_log); \
    }

#define LOGIC_ERROR(format, ...) {\
    char _log[4096] = {0}; \
    memset(_log, 0, sizeof(_log)); \
    SafeSprintf(_log, sizeof(_log), "Error : "##format, ##__VA_ARGS__); \
    s_pKernel->Trace(_log); \
    }

class IKernel {
public:
    /*
    * 简要的函数说明文字 
    *  @param [in] param1 参数1说明
    *  @param [out] param2 参数2说明
    *  @return 返回值说明
    */

    //日志
    virtual void Trace(const char * log) = 0;

    //网络方面接口
    virtual void Send(const s32 nConnectID, const void * pBuff, const s32 nSize) = 0;
    virtual void * CContext(const s32 nConnectID) = 0;

    virtual void RemoteInfo(const s32 nConnectID, const char * & pIP, s32 & nPort) = 0;

    virtual void AddEventCall(const s8 nTypeID, const s32 nEventID, const EVENTCALLFUN pCall, const char * pStrCallName) = 0;
    virtual void RemoveEventCall(const s8 nTypeID, const s32 nEventID, const EVENTCALLFUN pCall) = 0;

    virtual bool SetTimer(const s32 id, const s32 tickcount, const s32 calltimes, core::ITimerHandler * pHandler, const char * debugstr) = 0;  //同一个ID只能注册一次
    virtual bool KillTimer(const s32 id, core::ITimerHandler * pHandler) = 0;
    virtual bool KillTimer(core::ITimerHandler * pHandler) = 0; 
    virtual void CallEvnet(const s8 type, const s32 nEventID, const void * pContext, const s32 nSize) = 0;

    virtual bool HGetRequest(const s32 id, const char * url, const s32 urlsize, 
        core::IHttpHandle * pHandler, const void * context, const s32 contextsize) = 0;
    virtual bool HPostRequest(const s32 id, const char * url, const s32 urlsize, const void * data, const s32 datasize, 
        core::IHttpHandle * pHandler, const void * context, const s32 contextsize) = 0;
    virtual IModule * FindModule(const char * pModuleName) = 0;

    virtual void Stop() = 0;
};

/** @} */ // 模块结尾
