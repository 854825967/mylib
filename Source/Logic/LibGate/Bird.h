#ifndef BIRD_H
#define BIRD_H
#include "protocol.h"
#include "UpdateFields.h"
#include "TAttribute.h"
using namespace tlib;

class IKernel;

class Bird : public TAttribute<BIRD_FIELD_END, szBirdFieldLen> {
public:
    Bird();
    void SetConnection(IKernel * pKernel, const s32 nConnectID, const s64 lAccountID);
    void Send(const s32 nMsgID, const void * pData, const s32 nSize);
    void Clear();
    s64 GetAccountID();
protected:
    virtual void Sync(const s32 nAttribID, const void * pData, const s32 nLen);
private:
    IKernel * m_pKernel;
    BirdStates m_States;
    s32 m_nConnectID;
    s64 m_lAccountID;
};

#endif //BIRD_H
