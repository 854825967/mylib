#include "CInetAddress.h"

CInetAddress::CInetAddress(const char * pStrIp, const u16 nPort, const u8 type) {
    Initialize(pStrIp, nPort, type);
}

void CInetAddress::Initialize(const char * pStrIp /* = NULL */, const u16 nPort /* = 0 */, const u8 type /* = AF_INET */) {
    memset(&m_address, 0, sizeof(m_address));
    m_address.sin_family = type;
    if(pStrIp != NULL) {
        SetIp(pStrIp);
        SetIPort(nPort);
    }
}

bool CInetAddress::SetIp(const char * pStrIp) {
    if((m_address.sin_addr.s_addr = inet_addr(pStrIp)) == INADDR_NONE) {
        ECHO_ERROR("Error ip format, ip : %s", pStrIp);
        return false;
    }

    return true;
}

bool CInetAddress::SetIPort(const u16 nPort) {
    m_address.sin_port = htons(nPort);
    return true;
}

const char * CInetAddress::GetIp() {
    return inet_ntoa(m_address.sin_addr);
}

const u16 CInetAddress::GetPort() {
    return ntohs(m_address.sin_port);
}
