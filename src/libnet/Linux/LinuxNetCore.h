#ifdef linux
#ifndef LINUXNETCORE_H
#define LINUXNETCORE_H
#include "INet.h"

class CNet : public INet {
public:
    virtual bool CListen(const char * ip, const u16 port, const u16 count) const;
    virtual bool CConnectEx(const char * ip, const u16 port, const char * buff, const u32 size) const;
    virtual bool CSendMsg(const u16 conid, char * buff, const u16 size) const;
    virtual bool CClose(const u16 conid) const;
    virtual void CStartLoop() const;

    virtual bool CGetRemoteInfo(const u16 conid, const char * & ip, u16 & port) const;

    virtual void CSetNetThreadCount(const u8 count);
    virtual bool CSetConnectedCall(const CALL_FUN pcntedfun);
    virtual bool CSetRecvCall(const CALL_FUN precvfun);
    virtual bool CSetNewConnect(const CALL_FUN pnewconfun);

    CNet(const u8 threadcount, const u16 linkcount, const u16 iowaittime);
    ~CNet();
private:

private:
};

#endif //LINUXNETCORE_H
#endif //linux
