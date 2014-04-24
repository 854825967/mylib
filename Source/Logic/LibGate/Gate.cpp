#include "Gate.h"
#include "IKernel.h"
#include "protocol.h"
#include "IMessage.h"
#include "IHttpHandler.h"
#include "json/json.h"
#include "Tools.h"
#include "IMessage.h"
#include <algorithm>
using namespace core;

Gate * Gate::s_pSelf = NULL;
IMessage * Gate::s_pMessage = NULL;  
TPOOL<Bird> Gate::s_BirdPool;
CONNECTION_BIRD_MAP Gate::s_Connection_Bird_Map;
BIRD_LIST Gate::s_RoomMemberList;

bool Gate::Initialize(IKernel * pKernel) {
    s_pSelf = this;
    s_pKernel = pKernel;
    s_pMessage = (IMessage *)s_pKernel->FindModule("Message");
    ASSERT(s_pSelf && s_pMessage && s_pKernel);
    RGS_MSG_CALL(s_pMessage, "Client", CLIENT_REQ_LOGIN, Gate::OnClientLogin);
    RGS_MSG_CALL(s_pMessage, "Client", CLIENT_REQ_JOIN_HUNDRED_ONLINE, Gate::OnClientJoin);
    RGS_MSG_CALL(s_pMessage, "Client", CLIENT_REQ_OUT_HUNDRED_ONLINE, Gate::OnClientOut);
    RGS_MSG_CALL(s_pMessage, "Client", CLIENT_REQ_STATES_CHANGED, Gate::OnClientBirdStateChanaged);

    RGS_EVENT_CALL(EVENT_TYPE_CORE, CORE_EVENT_CONNECTION_BREAK, Gate::OnClientBreak);

//     {/*test code*/
//         Bird bird;
//         bird.SetInt32(BIRD_FIELD_TEST_INT32, 100);
//         bird.SetString(BIOLOGY_FIELD_NAME, "maoxiang");
// 
//         ECHO("%d, %s", bird.GetInt32(BIRD_FIELD_TEST_INT32), bird.GetString(BIOLOGY_FIELD_NAME));
//     }

    return true;
}

void Gate::OnHttpBack(const s32 id, const s32 err, const void * data, const s32 datasize, const void * context, const s32 size) {
    if (NULL != data && 0 != datasize) {

    }

    switch (id) {
    case HTTP_REQUEST_LOGIN:
        {
            GateRes_Login  res;
            s32 nConnectID = *(s32 *)context;
            static s64 i = 0;
            res.nResult = ERROR_CODE_NO_ERROR;
            res.lAccountID = i++;
            SEND_MSG(s_pKernel, nConnectID, GATE_RES_LOGIN, res);
            Bird * pBird = s_BirdPool.Create();
            ASSERT(pBird);
            pBird->SetConnection(s_pKernel, nConnectID, res.lAccountID);
            s_Connection_Bird_Map.insert(make_pair(nConnectID, pBird));
            break;

            if (NULL == data || 0 == datasize) {
                res.nResult = ERROR_CODE_UNKNONW;
                SEND_MSG(s_pKernel, *(s32 *)context, GATE_RES_LOGIN, res);
                return;
            }
            ECHO((const char *)data);

            Json::Reader reader;
            Json::Value root;
            if (reader.parse((const char *)data, root)) {  
                 Json::Value rescode = root["res_code"];
                 s32 nValue = rescode.asInt();

                 if (1 == nValue) {
                     res.nResult = ERROR_CODE_NO_ERROR;
                     Json::Value account = root["data"]["ACC_ID"];
                     res.lAccountID = account.asInt();
                     SEND_MSG(s_pKernel, *(s32 *)context, GATE_RES_LOGIN, res);

                     Bird * pBird = s_BirdPool.Create();
                     ASSERT(pBird);
                     pBird->SetConnection(s_pKernel, nConnectID, res.lAccountID);
                     s_Connection_Bird_Map.insert(make_pair(nConnectID, pBird));
                 } else {
                     res.nResult = ERROR_CODE_LOGIN_FAILED;
                     SEND_MSG(s_pKernel, nConnectID, GATE_RES_LOGIN, res);
                 }
            }
        }
        break;
    default:
        break;
    }
}

s32 Gate::OnClientLogin(const char * pStrType, s32 nConnectID, void * pContext, const s32 nSize) {
    if (nSize != sizeof(ClientReq_Loigin)) {
        LOGIC_ERROR("Error proto ClientReq_Loigin");
        return -1;
    }

    ClientReq_Loigin * pLogin = (ClientReq_Loigin *)pContext;
    pLogin->szUserName[sizeof(pLogin->szUserName) - 1] = 0;
    pLogin->szPasswd[sizeof(pLogin->szPasswd) - 1] = 0;
    
    char szData[128] = {0};
    SafeSprintf(szData, sizeof(szData), "name=%s&&passwd=%s", pLogin->szUserName, pLogin->szPasswd);
    s_pKernel->HPostRequest(HTTP_REQUEST_LOGIN, "10.10.0.188/Account.php?function=2", strlen("10.10.0.188/Account.php?function=2") + 1, szData, strlen(szData), s_pSelf, &nConnectID, sizeof(nConnectID));
    return 0;
}

s32 Gate::OnClientJoin(const char * pStrType, s32 nConnectID, void * pContext, const s32 nSize) {
    CONNECTION_BIRD_MAP::iterator itor = s_Connection_Bird_Map.find(nConnectID);
    if (itor == s_Connection_Bird_Map.end()) {
        LOGIC_ERROR("当前链接 %d 未登陆", nConnectID);
        return -1;
    }

    BIRD_LIST::iterator iend = s_RoomMemberList.end();
    BIRD_LIST::iterator ifind = ::find(s_RoomMemberList.begin(), iend, itor->second);
    if (ifind != iend) {
        LOGIC_ERROR("账号 %ld 重复发送进入房间消息", itor->second->GetAccountID());
        return -1;
    }

    s_RoomMemberList.push_back(itor->second);

    LOGIC_TRACE("账号 %lx 进入房间", itor->second->GetAccountID());
    
    return 0;
}


s32 Gate::OnClientOut(const char * pStrType, s32 nConnectID, void * pContext, const s32 nSize) {
    CONNECTION_BIRD_MAP::iterator itor = s_Connection_Bird_Map.find(nConnectID);
    if (itor == s_Connection_Bird_Map.end()) {
        LOGIC_ERROR("当前链接 %d 未登陆", nConnectID);
        return -1;
    }

    BIRD_LIST::iterator iend = s_RoomMemberList.end();
    BIRD_LIST::iterator ifind = ::find(s_RoomMemberList.begin(), iend, itor->second);
    if (ifind == iend) {
        LOGIC_ERROR("账号 %ld 未进入房间", itor->second->GetAccountID());
        return -1;
    }

    s_RoomMemberList.erase(ifind);
    LOGIC_TRACE("账号 %lx 退出房间", itor->second->GetAccountID());
    
    return 0;
}

s32 Gate::OnClientBirdStateChanaged(const char * pStrType, s32 nConnectID, void * pContext, const s32 nSize) {
    if (nSize != sizeof(BirdStates)) {
        LOGIC_ERROR("Error proto BirdStates");
        return -1;
    }

    CONNECTION_BIRD_MAP::iterator itor = s_Connection_Bird_Map.find(nConnectID);
    if (itor == s_Connection_Bird_Map.end()) {
        LOGIC_ERROR("当前链接 %d 未登陆", nConnectID);
        return -1;
    }

    if (itor->second->GetAccountID() != ((BirdStates *)pContext)->account_id) {
        LOGIC_ERROR("当前消息账号信息与服务器账号信息不符");
        return -1;
    }


    BIRD_LIST::iterator iend = s_RoomMemberList.end();
    BIRD_LIST::iterator ifind = ::find(s_RoomMemberList.begin(), iend, itor->second);
    if (ifind == iend) {
        LOGIC_ERROR("账号 %ld 未进入房间", itor->second->GetAccountID());
        return -1;
    }

    BIRD_LIST::iterator ibegin = s_RoomMemberList.begin();
    while (ibegin != iend) {
        if ((*ibegin) != itor->second) {
            (*ibegin)->Send(GATE_FWD_BIRD_STATES, pContext, nSize);
        }

        ibegin ++;
    }

}

s32 Gate::OnClientBreak(const s8 nEventType, s32 nConnectID, const void * pContext, const s32 nSize) {
    ASSERT(nEventType == EVENT_TYPE_CORE);
    CONNECTION_BIRD_MAP::iterator itor = s_Connection_Bird_Map.find(nConnectID);
    if (itor == s_Connection_Bird_Map.end()) {
        LOGIC_TRACE("当前链接 %d 未登陆", nConnectID);
        return 0;
    }

    BIRD_LIST::iterator iend = s_RoomMemberList.end();
    BIRD_LIST::iterator ifind = ::find(s_RoomMemberList.begin(), iend, itor->second);
    if (ifind == iend) {
        LOGIC_TRACE("账号 %ld 未进入房间", itor->second->GetAccountID());
        return 0;
    }

    s_RoomMemberList.erase(ifind);
    LOGIC_TRACE("账号 %lx 退出房间", itor->second->GetAccountID());
    itor->second->Clear();
    s_BirdPool.Recover(itor->second);
    s_Connection_Bird_Map.erase(itor);
    return 0;
}

bool Gate::DelayInitialize(IKernel * pKernel) {

    return true;
}

bool Gate::Destroy(IKernel * pKernel) {

    return true;
}
