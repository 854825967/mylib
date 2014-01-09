#ifndef HEAD_H
#define HEAD_H

// #include "MultiSys.h"
// #include <map>
// #include <string>
// using namespace std;
// 
// #define MAC_MODULE_NAME_LENGTH 64
// 
// class ICore {
// public:
//     virtual bool Initialize() = 0;
//     virtual bool DelayInitialize() = 0;
//     virtual bool Destroy() = 0;
// 
// public:
//     ICore() {
//         memset(this, 0, sizeof(*this));
//     }
// 
//     void SetName(const char * pName) {
//         SafeSprintf(m_pName, sizeof(m_pName), "%s", pName);
//     }
// 
//     const char * GetName() {
//         return m_pName;
//     }
// 
// private:
//     char m_pName[MAC_MODULE_NAME_LENGTH];
// };
// 
// typedef map<string, ICore *> CORE_MAP;
// extern CORE_MAP g_mapcore;
// #define MAC_BUILD_CORE(name) { \
//     ICore * pCore = NEW name; \
//     pCore->SetName(#name); \
//     g_mapcore.insert(make_pair(#name, pCore)); \
// }
// 
// ICore * FindCore(const char * pStrName);

#endif //HEAD_H
