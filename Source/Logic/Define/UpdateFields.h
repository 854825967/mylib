#ifndef UPDATEFIELDS_H
#define UPDATEFIELDS_H

#include "MultiSys.h"

#define LEN_INT8 (sizeof(s8))
#define LEN_INT16 (sizeof(s16))
#define LEN_INT32 (sizeof(s32))
#define LEN_INT64 (sizeof(s64))
#define LEN_DOUBLE (sizeof(double))
#define LEN_STRING 128
#define LEN_STRUCT 255

#define INHERIT(son, father) { \
    for (s32 i=0; i<sizeof(son); i++) { \
        (son)[i] = LEN_INT32; \
    } \
    memcpy((son), (father), sizeof(father)); \
}

enum _ObjectFields{
    OBJECT_FIELD_TYPE = 0x0000,	// 对象类型
    OBJECT_FIELD_ID, // 对象ID
    OBJECT_FIELD_MODEL_ID, // 对象模版ID

    OBJECT_END
};
extern s16 szObjectFieldLen[OBJECT_END];

enum _BiologyFields {
    BIOLOGY_FIELD_BGN = OBJECT_END,
    BIOLOGY_FIELD_NAME = BIOLOGY_FIELD_BGN, // 名字
    BIOLOGY_FIELD_LEVEL,    // 等级

    BIOLOGY_FIELD_END
};
extern s16 szBiologyFieldLen[BIOLOGY_FIELD_END];

enum _BirdFields {
    BIRD_FIELD_BGN = BIOLOGY_FIELD_END,
    BIRD_FIELD_GOLD = BIRD_FIELD_BGN,
    BIRD_FIELD_TEST_INT8,
    BIRD_FIELD_TEST_INT32,
    BIRD_FIELD_TEST_INT64,
    BIRD_FIELD_TEST_INT16,
    BIRD_FIELD_TEST_DOUBLE,

    BIRD_FIELD_END
};
extern s16 szBirdFieldLen[BIRD_FIELD_END];

namespace UpdateField {
    class _Format {
    public:
        _Format() {
            {//_ObjectFieldLen
                for (s32 i=0; i<sizeof(szObjectFieldLen); i++) {
                    szObjectFieldLen[i] = LEN_INT32;
                }
                szObjectFieldLen[OBJECT_FIELD_TYPE] = LEN_INT16;
                szObjectFieldLen[OBJECT_FIELD_MODEL_ID] = LEN_INT16;
            }

            {//_BiologyFields
                INHERIT(szBiologyFieldLen, szObjectFieldLen);
                szBiologyFieldLen[BIOLOGY_FIELD_NAME] = LEN_STRING;
                szBiologyFieldLen[BIOLOGY_FIELD_LEVEL] = LEN_INT16;
            }

            {//_BirdFieldLen
                INHERIT(szBirdFieldLen, szBiologyFieldLen);


                szBirdFieldLen[BIRD_FIELD_TEST_INT8] = LEN_INT8;
                szBirdFieldLen[BIRD_FIELD_TEST_INT32] = LEN_INT32;
                szBirdFieldLen[BIRD_FIELD_TEST_INT64] = LEN_INT64;
                szBirdFieldLen[BIRD_FIELD_TEST_INT16] = LEN_INT16;
                szBirdFieldLen[BIRD_FIELD_TEST_DOUBLE] = LEN_DOUBLE;
            }
        }
    };
}

#endif //UPDATEFIELDS_H
