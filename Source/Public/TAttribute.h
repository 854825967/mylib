#ifndef TATTRIBUTE_H
#define TATTRIBUTE_H

#include "MultiSys.h"

namespace tlib {
    typedef struct attr_info {
        s16 len;
        char * pData;
    }AttrInfo;

    template <s32 attrib_count, s16 attrib_len[attrib_count], s32 max_buff_size = 4096>
    class TAttribute {
    public:
        TAttribute() {
            memset(this, 0, sizeof(*this));
            s32 index = 0;
            for (s32 i=0; i<attrib_count; i++) {
                szAttribInfo[i].len = attrib_len[i];
                szAttribInfo[i].pData = szBuff + index;
                index += szAttribInfo[i].len;
            }
        }

        virtual void Initialize(const char * buff, const s32 nSize) {
            ASSERT(nSize <= max_buff_size);
            memcpy(szBuff, buff, nSize);
        }

        virtual void SetInt8(const s32 nAttribID, const s8 value, const bool bSync = false) {
            ASSERT(nAttribID < attrib_count 
                && sizeof(value) == szAttribInfo[nAttribID].len);
            memcpy(szAttribInfo[nAttribID].pData, &value, sizeof(value));
            szAttribInfo[nAttribID].len;
            if (bSync) {
                Sync(nAttribID, &value, sizeof(value));
            }
        }

        virtual s8 GetInt8(const s32 nAttribID) {
            ASSERT(nAttribID < attrib_count
                && sizeof(s8) == szAttribInfo[nAttribID].len);
            return *(s8 *)szAttribInfo[nAttribID].pData;
        }

        virtual void SetInt16(const s32 nAttribID, const s16 value, const bool bSync = false) {
            ASSERT(nAttribID < attrib_count 
                && sizeof(value) == szAttribInfo[nAttribID].len);
            memcpy(szAttribInfo[nAttribID].pData, &value, sizeof(value));
            szAttribInfo[nAttribID].len;
            if (bSync) {
                Sync(nAttribID, &value, sizeof(value));
            }
        }

        virtual s16 GetInt16(const s32 nAttribID) {
            ASSERT(nAttribID < attrib_count
                && sizeof(s16) == szAttribInfo[nAttribID].len);
            return *(s16 *)szAttribInfo[nAttribID].pData;
        }

        virtual void SetInt32(const s32 nAttribID, const s32 value, const bool bSync = false) {
            ASSERT(nAttribID < attrib_count 
                && sizeof(value) == szAttribInfo[nAttribID].len);
            memcpy(szAttribInfo[nAttribID].pData, &value, sizeof(value));
            szAttribInfo[nAttribID].len;
            if (bSync) {
                Sync(nAttribID, &value, sizeof(value));
            }
        }

        virtual s32 GetInt32(const s32 nAttribID) {
            ASSERT(nAttribID < attrib_count
                && sizeof(s32) == szAttribInfo[nAttribID].len);
            return *(s32 *)szAttribInfo[nAttribID].pData;
        }

        virtual void SetInt64(const s32 nAttribID, const s64 value, const bool bSync = false) {
            ASSERT(nAttribID < attrib_count 
                && sizeof(value) == szAttribInfo[nAttribID].len);
            memcpy(szAttribInfo[nAttribID].pData, &value, sizeof(value));
            if (bSync) {
                Sync(nAttribID, &value, sizeof(value));
            }
        }

        virtual s64 GetInt64(const s32 nAttribID) {
            ASSERT(nAttribID < attrib_count
                && sizeof(s64) == szAttribInfo[nAttribID].len);
            return *(s64 *)szAttribInfo[nAttribID].pData;
        }

        virtual void SetDouble(const s32 nAttribID, const double value, const bool bSync = false) {
            ASSERT(nAttribID < attrib_count 
                && sizeof(value) == szAttribInfo[nAttribID].len);
            memcpy(szAttribInfo[nAttribID].pData, &value, sizeof(value));
            if (bSync) {
                Sync(nAttribID, &value, sizeof(value));
            }
        }

        virtual double GetDouble(const s32 nAttribID) {
            ASSERT(nAttribID < attrib_count
                && sizeof(double) == szAttribInfo[nAttribID].len);
            return *(double *)szAttribInfo[nAttribID].pData;
        }

        virtual void SetString(const s32 nAttribID, const char * pString, const bool bSync = false) {
            ASSERT(nAttribID < attrib_count && LEN_STRING == szAttribInfo[nAttribID].len);
            s32 nLen = strlen(pString) + 1;
            if (nLen >= szAttribInfo[nAttribID].len) {
                nLen = szAttribInfo[nAttribID].len;
            }
            if (nLen - 1 != 0) {
                memcpy(szAttribInfo[nAttribID].pData, pString, nLen - 1);
            }

            ((char *)szAttribInfo[nAttribID].pData)[nLen - 1] = 0;

            if (bSync) {
                Sync(nAttribID, pString, nLen);
            }
        }

        virtual const char * GetString(const s32 nAttribID) {
            ASSERT(nAttribID < attrib_count);
            return (const char *)szAttribInfo[nAttribID].pData;
        }

        virtual void SetAttrib(const s32 nAttribID, const void * pData, const s32 nLen, const bool bSync = false) {
            ASSERT(nAttribID < attrib_count 
                && nLen <= szAttribInfo[nAttribID].len);
            memcpy(szAttribInfo[nAttribID].pData, pData, nLen);
            if (bSync) {
                Sync(nAttribID, pData, nLen);
            }
        }

        virtual void * GetAttrib(const s32 nAttribID) {
            ASSERT(nAttribID < attrib_count);
            return szAttribInfo[nAttribID].pData;
        }

        virtual void * GetBuff(s32 & nLen) {
            nLen = max_buff_size;
            return szBuff;
        }

    protected:
        virtual void Sync(const s32 nAttribID, const void * pData, const s32 nLen) = 0;

    private:
        char szBuff[max_buff_size];
        AttrInfo szAttribInfo[attrib_count];
    };
}

#endif //TATTRIBUTE_H
