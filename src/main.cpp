// #include "INet.h"
// #include "ILog.h"
// #include "Tools.h"
// #include "CDumper.h"
// 
// INet * pNet = NULL;
// ILog * pLog = NULL;
// static u32 i = 0;
// class ConnectInfo {
// public:
//     ConnectInfo() {
//         memset(this, 0, sizeof(ConnectInfo));
//     }
//     char strip[32];
//     u16 nPort;
// };
// 
// void ConnectFailed(const u16 conid, const void * buff, const u32 size) {
//     const ConnectInfo * pInfo =(const ConnectInfo *)buff;
//     ECHO_WARN("连接 %s : %d 失败...", pInfo->strip, pInfo->nPort);
//     return;
// }
// 
// void Recv(const u16 conid, const void * buff, const u32 size) {
//     static u64 nLength = 0;
//     nLength += size;
//     ECHO_TRACE("%s", (char *)buff);
//     //ECHO_TRACE("链接 %d  消息长度 %d 消息内容 %s 该条连接 发的总长度为 %ld", conid, size, (char *)buff, nLength);
//     pNet->CSendMsg(conid, "helloworldhelloworldhelloworldhelloworld", strlen("helloworldhelloworldhelloworldhelloworld"));
//     return;
// }
// 
// void Connected(const u16 conid, const void * buff, const u32 size) {
//     i ++;
//     const ConnectInfo * pInfo =(const ConnectInfo *)buff;
//     pNet->CConnectEx(pInfo->strip, pInfo->nPort, pInfo, size);
//     ECHO_TRACE("连接 %s : %d 成功...当前连接总数 %d", pInfo->strip, pInfo->nPort, i);
//     pNet->CSendMsg(conid, "helloworldhelloworldhelloworldhelloworld", strlen("helloworldhelloworldhelloworldhelloworld"));
//     pNet->CClose(conid);
//     CSleep(1);
//     return;
// }
// 
// void NewConnect(const u16 conid, const void * buff, const u32 size) {
//     i ++;
//     const char * ip = NULL;
//     u16 nPort = 0;
//     pNet->CGetRemoteInfo(conid, ip, nPort);
//     ECHO_TRACE("新的连接来自 %s : %d ...当前连接总数 %d", ip, nPort, i);
//     CSleep(10);
//     return;
// }
// 
// void ConnectionBreak(const u16 conid, const void * buff, const u32 size) {
//     i --;
//     ECHO_ERROR("连接 %d 断开, 当前连接总数 %d", conid, i);
//     return;
// }
// 
// class Dumper {
// public:
//     Dumper() {
//         CDumper::GetInstance().SetDumpFileName("RobotDump.dmp");
//     }
// };
// 
// //初始化Dumper;
// Dumper dumper;
// 
// #include "../liblog/LogSystem.h"
// 
// int main() {
// #if defined WIN32 || defined WIN64
//     WSAData wsd;
//     if (WSAStartup(MAKEWORD(2, 2),&wsd) != 0) {
//         ECHO_ERROR("初始化网络出错");
//         ASSERT(false);
//         return -1;
//     }
// #endif
//     
//     char dllPath[128] = {0};
//     SafeSprintf(dllPath, sizeof(dllPath), "%s/../../libnet/debug/%s", ::GetAppPath(), "libnet.dll");
//     pNet = GetNetWorker(dllPath, 2);
//     if (NULL == pNet) {
//         ECHO_ERROR("Get point of netcore failed");
//         ASSERT(false);
//         return 1;
//     }
//     SafeSprintf(dllPath, sizeof(dllPath), "%s/../../liblog/debug/%s", ::GetAppPath(), "liblog.dll");
//     pLog = GetLogWorker(dllPath, "test");
//     if (NULL == pLog) {
//         ECHO_ERROR("Get point of logcore failed");
//         ASSERT(false);
//         return 1;
//     }
// 
//     pNet->CSetConnectFailedCall(ConnectFailed);
//     pNet->CSetConnectedCall(Connected);
//     pNet->CSetRecvCall(Recv);
//     pNet->CSetConnectionBreakCall(ConnectionBreak);
//     pNet->CSetNewConCall(NewConnect);
//     pNet->CListen("0.0.0.0", 10028);
// //     ConnectInfo coninfo;
// //     SafeSprintf(coninfo.strip, sizeof(coninfo.strip), "%s", "192.168.1.110");
// //     coninfo.nPort = 10028;
// //     for (u32 i=0; i<100; i++) {
// //         pNet->CConnectEx(coninfo.strip, coninfo.nPort, &coninfo, sizeof(coninfo));
// //     }
// 
//     pNet->CStartLoop(false);
//     return 1;
// }
