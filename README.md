I create a net lib named **Super Net worker**,
Interface
------------------
src\libnet\Interface\INet.h<br />
First u must set callback address...<br />
		//callback when connect host succeed<br />
    virtual bool CSetConnectedCall(const CALL_FUN pcntedfun) = 0; <br />
    //callback when connected failed<br />
    virtual bool CSetConnectFailedCall(const CALL_FUN pcntfailedfun) = 0;<br />
    //callback when break<br />
    virtual bool CSetConnectionBreakCall(const CALL_FUN pbreakfun) = 0; <br />
    //callback when recv msg<br />
    virtual bool CSetRecvCall(const CALL_FUN precvfun) = 0;	<br />
    //callback when a new remoter connected<br />
    virtual bool CSetNewConCall(const CALL_FUN precvfun) = 0; <br />
<br />
Use this interface start net event<br />
		//async listen<br />
    virtual bool CListen(const char * ip, const u16 port, const u16 count = 200) = 0;<br />
    //async connect<br />
    virtual bool CConnectEx(const char * ip, const u16 port, const void * buff = NULL, const u32 size = 0) = 0;<br /> 
    //close link<br />
    virtual bool CClose(const u16 conid) = 0; <br />
    //async send msg<br />
    virtual bool CSendMsg(const u16 conid, const void * buff, const u16 size) = 0;<br /> 
    //get remote info<br />
    virtual bool CGetRemoteInfo(const u16 conid, const char * & ip, u16 & port) = 0; <br />
    //start network thread<br />
    virtual void CStartLoop(u32 waitTime = 5, bool demon = false)  = 0;<br />

Contact Me
------------------
   * Email: [Alax's email][1]
   
[1]: 854825967@qq.com "854825967@qq.com"
