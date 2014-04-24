#ifndef GATE_H
#define GATE_H

#include "IGate.h"
#include "IHttpHandler.h"
#include "CHashMap.h"
#include "Bird.h"
#include "TPool.h"
#include <list>
using namespace std;
using namespace tlib;
class IMessage;

typedef CHashMap<s32, Bird *> CONNECTION_BIRD_MAP;
typedef list<Bird *> BIRD_LIST;

class Gate : public IGate, core::IHttpHandle {
public:
    /*
    * Initialize Ԥ�������ص� (��Ҫ������ģ���Լ��ڲ���׼��)
    *  @param [in] IKernel * pKernel ����ص�������ӿ�
    *  @return ����ֵ˵���ص��Ƿ���óɹ�
    */
    virtual bool Initialize(IKernel * pKernel);
    /*
    * DelayInitialize ��ʱ�����ص� (��Ҫ������ģ��֮�����������)
    *  @param [in] IKernel * pKernel ����ص�������ӿ�
    *  @return ����ֵ˵���ص��Ƿ���óɹ�
    */
    virtual bool DelayInitialize(IKernel * pKernel);
    /*
    * Destroy ģ������ (ģ���������ݻ���)
    *  @param [in] IKernel * pKernel ����ص�������ӿ�
    *  @return ����ֵ˵���ص��Ƿ���óɹ�
    */
    virtual bool Destroy(IKernel * pKernel);

    virtual void OnHttpBack(const s32 id, const s32 err,const void * data, const s32 datasize, const void * context, const s32 size);

private:
    static s32 OnClientLogin(const char * pStrType, s32 nConnectID, void * pContext, const s32 nSize);
    static s32 OnClientJoin(const char * pStrType, s32 nConnectID, void * pContext, const s32 nSize);
    static s32 OnClientOut(const char * pStrType, s32 nConnectID, void * pContext, const s32 nSize);
    static s32 OnClientBirdStateChanaged(const char * pStrType, s32 nConnectID, void * pContext, const s32 nSize);

    static s32 OnClientBreak(const s8 nEventType, s32 nConnectID, const void * pContext, const s32 nSize);

private:
    static Gate * s_pSelf;
    static IMessage * s_pMessage;
    static TPOOL<Bird> s_BirdPool;
    static CONNECTION_BIRD_MAP s_Connection_Bird_Map;
    static BIRD_LIST s_RoomMemberList;
};

#endif //GATE_H


