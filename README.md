mylib
=====
src\libnet\Interface
First u must set callback address...
    virtual bool CSetConnectedCall(const CALL_FUN pcntedfun) = 0; //callback when connect host succeed
    virtual bool CSetConnectFailedCall(const CALL_FUN pcntfailedfun) = 0; //callback when connected failed
    virtual bool CSetConnectionBreakCall(const CALL_FUN pbreakfun) = 0; //callback when break
    virtual bool CSetRecvCall(const CALL_FUN precvfun) = 0;	//callback when recv msg
    virtual bool CSetNewConCall(const CALL_FUN precvfun) = 0; //callback when a new remoter connected

Use this interface start net event
    virtual bool CListen(const char * ip, const u16 port, const u16 count = 200)  = 0;//async listen
    virtual bool CConnectEx(const char * ip, const u16 port, const void * buff = NULL, const u32 size = 0)  = 0; //async connect
    virtual bool CClose(const u16 conid)  = 0; //close link
    virtual bool CSendMsg(const u16 conid, const void * buff, const u16 size)  = 0; //async send msg
    virtual bool CGetRemoteInfo(const u16 conid, const char * & ip, u16 & port)  = 0; //get remote info

    virtual void CStartLoop(u32 waitTime = 5, bool demon = false)  = 0;  //start network thread

liblog,libnet
