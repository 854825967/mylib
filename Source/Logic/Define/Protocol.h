#ifndef PROTOCOL_H
#define PROTOCOL_H
#include "MultiSys.h"

enum eTag {
    TAG_CLIENT  = 0,
    TAG_GATE = 1,
};

/* ============================Message ID=========================== */
#define MAKE_ID(tag, id) ((tag<<24) + id)
enum eClientMessage {
    CLIENT_REQ_LOGIN = MAKE_ID(TAG_CLIENT, 0X01),
    CLIENT_REQ_JOIN_HUNDRED_ONLINE = MAKE_ID(TAG_CLIENT, 0X02),
    CLIENT_REQ_OUT_HUNDRED_ONLINE = MAKE_ID(TAG_CLIENT, 0X03),
    CLIENT_REQ_STATES_CHANGED = MAKE_ID(TAG_CLIENT, 0X04),

};

enum eGateMessage {
    GATE_RES_LOGIN = MAKE_ID(TAG_GATE, 0X01),
    GATE_FWD_BIRD_STATES = MAKE_ID(TAG_GATE, 0X02),

};
/* ===========================Message ID============================ */

enum eErrorCode {
    ERROR_CODE_UNKNONW = 0,
    ERROR_CODE_NO_ERROR = 1,
    ERROR_CODE_LOGIN_FAILED = 2,
};

#pragma pack(push,1)
//===============CLIENT_REQ_LOGIN========
struct ClientReq_Loigin {
    char szUserName[64];
    char szPasswd[128];
};

struct GateRes_Login {
    s32 nResult;
    s64 lAccountID;
};
//=============GATE_RES_LOGIN==========
struct Position {
    float x;
    float y;
};

struct BirdStates {
    s64 account_id;
    Position pos;
    float rotation;
    float speedx;
    float speedy;
};
#pragma pack(pop)

enum eHttpRequestID {
    HTTP_REQUEST_LOGIN = 0,
};
#endif //PROTOCOL_H
