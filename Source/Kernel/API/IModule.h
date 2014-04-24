/*
 * @defgroup IModule �߼�ģ��ӿ���

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
    * Initialize Ԥ�������ص� (��Ҫ������ģ���Լ��ڲ���׼��)
    *  @param [in] IKernel * pKernel ����ص�������ӿ�
    *  @return ����ֵ˵���ص��Ƿ���óɹ�
    */
    virtual bool Initialize(IKernel * pKernel) = 0;
    /*
    * DelayInitialize ��ʱ�����ص� (��Ҫ������ģ��֮�����������)
    *  @param [in] IKernel * pKernel ����ص�������ӿ�
    *  @return ����ֵ˵���ص��Ƿ���óɹ�
    */
    virtual bool DelayInitialize(IKernel * pKernel) = 0;
    /*
    * Destroy ģ������ (ģ���������ݻ���)
    *  @param [in] IKernel * pKernel ����ص�������ӿ�
    *  @return ����ֵ˵���ص��Ƿ���óɹ�
    */
    virtual bool Destroy(IKernel * pKernel) = 0;

    /*
    *
    * OnDBRet ���ݿ��첽��������
    *  @param [in] IKernel * pKernel ����ص�������ӿ�
    *  @param [in] const s32 nIndex ��������
    *  @param [in] const s32 nRequestID ����ID
    *  @param [in] const s32 nRequesterID ������ID���ڸ��ؾ���
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
/** @} */ // ģ���β
