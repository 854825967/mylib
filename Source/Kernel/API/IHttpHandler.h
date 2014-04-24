#ifndef IHTTPHANDLE_H
#define IHTTPHANDLE_H

#include "TStream.h"
using namespace tlib;

namespace core {
    class IHttpHandle {
    public:
        virtual void OnHttpBack(const s32 id, const s32 err,const void * data, const s32 datasize, const void * context, const s32 size) = 0;
    };
}

#endif //IHTTPHANDLE_H
