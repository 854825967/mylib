/*
 * @defgroup IModule 逻辑模块接口类

 * @{

 */
#ifndef IMODULE_H
#define IMODULE_H

#include "MultiSys.h"

#define MAC_MODULE_NAME_LENGTH 64
class IKernel;

class IModule {
public:
    /*
    * Initialize 预备启动回调 (主要用作各模块自己内部做准备)
    *  @param [in] IKernel * pKernel 传入回调的引擎接口
    *  @return 返回值说明回调是否调用成功
    */
    virtual bool Initialize(IKernel * pKernel) = 0;
    /*
    * DelayInitialize 延时启动回调 (主要用作各模块之间的联合启动)
    *  @param [in] IKernel * pKernel 传入回调的引擎接口
    *  @return 返回值说明回调是否调用成功
    */
    virtual bool DelayInitialize(IKernel * pKernel) = 0;
    /*
    * Destroy 模块销毁 (模块销毁数据回收)
    *  @param [in] IKernel * pKernel 传入回调的引擎接口
    *  @return 返回值说明回调是否调用成功
    */
    virtual bool Destroy(IKernel * pKernel) = 0;

    /*
    *
    * OnDBRet 数据库异步操作返回
    *  @param [in] IKernel * pKernel 传入回调的引擎接口
    *  @param [in] const s32 nIndex 请求索引
    *  @param [in] const s32 nRequestID 请求ID
    *  @param [in] const s32 nRequesterID 请求者ID用于负载均衡
    */
//     virtual s32 OnDBRet(IKernel * pKernel, const s32 nIndex, const s32 nRequestID, const s64 nRequesterID, 
//         const void * pInData, const s32 nInLen, const void * pOutData, const s32 nOutLen) = 0;

    //virtual void OnDBError(IKernel * pKernel, const s32 nIndex, const s32 nRequestID, ) = 0;

    /*
    * 
    */
    //virtual s32 OnRedisRet(IKernel * pKernel, const s32 nIndex, const s32 nRequestID, const ) = 0;
    //virtual s32 OnHttpRet(IKernel * pKernel);


public:
    IModule() {
        m_pNextModule = NULL;
        m_pName[MAC_MODULE_NAME_LENGTH - 1] = 0;
    }

    bool SetNext(IModule * & pModule) {
        m_pNextModule = pModule;
        return true;
    }

    IModule * GetNext() {
        return m_pNextModule;
    }

    void SetName(const char * pName) {
        SafeSprintf(m_pName, sizeof(m_pName), "%s", pName);
    }

    const char * GetName() {
        return m_pName;
    }

    static IKernel * s_pKernel;
private:
    IModule * m_pNextModule;
    char m_pName[MAC_MODULE_NAME_LENGTH];
};

static IModule * g_pModule = NULL;
#define CREATE_MODULE(name) \
class factroy##name {    \
public:    \
    factroy##name(IModule * & pModule) { \
        IModule * pModule##name = NEW name; \
        pModule##name->SetName(#name); \
        pModule##name->SetNext(pModule); \
        pModule = pModule##name; \
    } \
}; \
factroy##name factroy##name(g_pModule);


#define GET_DLL_ENTRANCE \
    IKernel * IModule::s_pKernel = NULL; \
    extern "C" __declspec(dllexport) IModule * __cdecl GetLogicModule() {    \
        return g_pModule; \
    } \
    BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) { \
        return TRUE;    \
    }


#define FUN_NAME_GETLOGICMODULE "GetLogicModule"
typedef IModule * (__cdecl *GET_LOGIC_MODULE)();

#endif  //IModule_H
/** @} */ // 模块结尾
