#include "INet.h"
#include "ILog.h"
#include "Tools.h"
#include "CDumper.h"
#include "AES.h"

INet * pNet = NULL;
ILog * pLog = NULL;
static u32 i = 0;
class ConnectInfo {
public:
    ConnectInfo() {
        memset(this, 0, sizeof(ConnectInfo));
    }
    char strip[32];
    u16 nPort;
};

void ConnectFailed(const u16 conid, const void * buff, const u32 size) {
    const ConnectInfo * pInfo =(const ConnectInfo *)buff;
    ECHO_WARN("连接 %s : %d 失败...", pInfo->strip, pInfo->nPort);
    return;
}

void Recv(const u16 conid, const void * buff, const u32 size) {
    static u64 nLength = 0;
    nLength += size;
    ECHO_TRACE("%s", (char *)buff);
    return;
}

void Connected(const u16 conid, const void * buff, const u32 size) {
    return;
}

void NewConnect(const u16 conid, const void * buff, const u32 size) {

    ConnectInfo * pConInfo = (ConnectInfo *)buff;

    ECHO("端口%d, 有新的连接进来", pConInfo->nPort);

    CSleep(10);
    return;
}

void ConnectionBreak(const u16 conid, const void * buff, const u32 size) {
    i --;
    ECHO_ERROR("连接 %d 断开, 当前连接总数 %d", conid, i);
    return;
}

class Dumper {
public:
    Dumper() {
        CDumper::GetInstance().SetDumpFileName("RobotDump.dmp");
    }
};

//初始化Dumper;
Dumper dumper;

#include "../liblog/LogSystem.h"

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
//     int iResult;
//     DWORD dwError;
//     int i = 0;    
//     struct hostent *remoteHost;    
//     char *host_name;    
//     struct in_addr addr;    
//     char **pAlias;
// 
//     host_name = "test.app.haohaowan.com";
//     // 如果用户输入是一个字母, 则我们使用gethostbyname()函数
//     // 假如输入是数字，则判断为IP地址
//     if (isalpha(host_name[0])) {    
//         printf("Calling gethostbyname with %s\n", host_name);        
//         remoteHost = gethostbyname(host_name);    
//     } else {        
//         printf("使用%s调用gethostbyaddr函数%s\n", host_name);    
//         addr.s_addr = inet_addr(host_name);    
//         if (addr.s_addr == INADDR_NONE) {        
//             printf("请输入有效IPv4地址\n");    
//             return 1;    
//         } else    
//         {    
//             remoteHost = gethostbyaddr((char *) &addr, 4, AF_INET);    
//         }
//     }
//     if (remoteHost == NULL) {    
//         dwError = WSAGetLastError();    
//         if (dwError != 0) {    
//             if (dwError == WSAHOST_NOT_FOUND) {        
//                 printf("无法定位到主机\n");
//                 return 1;
//             }else if (dwError == WSANO_DATA) {        
//                 printf("无路由记录\n");    
//                 return 1;    
//             } else {
//                 printf("函数调用出错，代码: %ld\n", dwError);
//                 return 1;    
//             }
//         }
//     } else {
//         printf("函数返回:\n");
//         printf("\t主机名称: %s\n", remoteHost->h_name);
//         for (pAlias = remoteHost->h_aliases; *pAlias != 0; pAlias++) {
//             printf("\t主机别名#%d: %s\n", ++i, *pAlias);
//         }
//         printf("\t地址类型: ");
//         switch (remoteHost->h_addrtype) {
//         case AF_INET:    
//             printf("AF_INET\n");
//             break;
//         case AF_INET6:
//             printf("AF_INET6\n");
//             break;
//         case AF_NETBIOS:
//             printf("AF_NETBIOS\n");
//             break;
//         default:
//             printf(" %d\n", remoteHost->h_addrtype);
//             break;
//         }
//         printf("\t地址长度: %d\n", remoteHost->h_length);      
//         i = 0;
//         while (remoteHost->h_addr_list[i] != 0) {
//             addr.s_addr = *(u_long *) remoteHost->h_addr_list[i++];
//             printf("\t主机IP地址#%d: %s\n", i, inet_ntoa(addr));
//         }    
//     }
//     
//     return 0;
// /*    
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
//     int i = 4;
//     ConnectInfo info;
//     SafeSprintf(info.strip, sizeof(info.strip), "%s", "0.0.0.0");
//     info.nPort = 10028;
//     pNet->CListen("0.0.0.0", 10028, 4, &info, sizeof(info));
//     info.nPort = 10022;
//     pNet->CListen("0.0.0.0", 10022, 4, &info, sizeof(info));
// //     ConnectInfo coninfo;
// //     SafeSprintf(coninfo.strip, sizeof(coninfo.strip), "%s", "192.168.1.110");
// //     coninfo.nPort = 10028;
// //     for (u32 i=0; i<100; i++) {
// //         pNet->CConnectEx(coninfo.strip, coninfo.nPort, &coninfo, sizeof(coninfo));
//     //     }
//     pNet->CConnectEx("127.0.0.1", 10028);
//     pNet->CConnectEx("127.0.0.1", 10022);
// 
//     pNet->CStartLoop(false);
//     return 1;
//     */
// }


typedef struct _person{
	s32 nAge;
	char szName[64];

	_person() {
		memset(this, 0, sizeof(*this));
	}

}Person;

int main() {
	unsigned char * pKey = (unsigned char *)"hello....";
	AES a(pKey);

	Person jack;
	jack.nAge = 10;

	char szTemp[sizeof(jack) + 8];
	memset(szTemp, 0, sizeof(szTemp));
	memcpy(szTemp, &jack, sizeof(jack));

	SafeSprintf(jack.szName, sizeof(jack.szName), "%s", "Jack");

	Person * pAesJack = (Person *)a.Cipher(szTemp, sizeof(jack));


	return 0;
}
