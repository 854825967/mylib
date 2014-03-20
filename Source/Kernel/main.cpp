#include "MultiSys.h"
#include "CDumper.h"
#include "Tools.h"
#include "TLogger.h"
#include "CoreHeader.h"
#include "CTimerEngine.h"
#include <vector>
#include <iostream>
using namespace tlib;
using namespace core;

#ifdef WIN32
class InitDumper {
public:
    InitDumper() {
        CDumper::GetInstance().SetDumpFileName("Kernel_dump");
    }
};
#endif //WIN32

enum {
    LOG_TRACE = 0,
    LOG_DEBUG = 1,
    LOG_ERROR = 2,

    //add before this
    LOG_FILE_COUNT
};

void testLogger() {
    TLogger<LOG_FILE_COUNT> logger;
    logger.Start();

    std::string filePath = ::GetAppPath();
    logger.OpenLogFile(LOG_TRACE, filePath.c_str(), "Trace");
    logger.OpenLogFile(LOG_DEBUG, filePath.c_str(), "Debug");
    logger.OpenLogFile(LOG_ERROR, filePath.c_str(), "Error");

    s32 i = 0;
    while (true) {
        s8 type = i++%LOG_FILE_COUNT;
        logger.Log(type, "妈妈说日志一定要长,单挑一定要非常长,这样才能测试出数据稳不稳定,不知道妈妈说的对不对,反正我信了...");
        if (type = LOG_ERROR) {
            CSleep(1);
        }
    }

    logger.Stop();
}

class TestTimerHandler : public ITimerHandler {
    virtual s32 OnTimer(const s32 id) {
        switch (id)
        {
        case 4:
            CSleep(10);
            break;
        case 5:
            CSleep(20);
            break;
        case 6:
            CSleep(30);
            break;
        case 7:
            CSleep(10);
            break;
        default:
            break;
        }
        return 0;
    }
};

s32 timetick[4] = {1000, 2000, 3000, 4000};

void testCTimerEngine() {
    ITimerEngine * pTimerEngine = NEW CTimerEngine;
    TestTimerHandler timerHandler;
    pTimerEngine->SetTimer(1, 1000, 1, &timerHandler, "Test TimerHandler 1_1");
    pTimerEngine->KillTimer(1, &timerHandler);
    pTimerEngine->SetTimer(1, 1000, 1, &timerHandler, "Test TimerHandler 1_2");
    pTimerEngine->SetTimer(2, 1000, 1, &timerHandler, "Test TimerHandler 2");
    pTimerEngine->KillTimer(&timerHandler);

    pTimerEngine->SetTimer(4, timetick[0], 0, &timerHandler, "定时器 4 间隔时间1000毫秒 执行4次");
    pTimerEngine->SetTimer(5, timetick[1], 0, &timerHandler, "定时器 5 间隔时间2000毫秒 执行5次");
    pTimerEngine->SetTimer(6, timetick[2], 0, &timerHandler, "定时器 6 间隔时间3000毫秒 执行6次");
    pTimerEngine->SetTimer(7, timetick[3], 0, &timerHandler, "定时器 7 间隔时间4000毫秒 执行7次");

    while (true) {
        pTimerEngine->OnTimer(100);
    }

}

void testHashMap() {
    CHashMap<s32, s32> testmap;
    for (s32 i=0; i<10; i++) {
        testmap.insert(make_pair(i, i));
    }
    CHashMap<s32, s32>::iterator itor = testmap.begin();
    CHashMap<s32, s32>::iterator iend = testmap.end();
    s32 n = 0;
    while (itor != iend) {
        if (n%3 == 0) {
            itor = testmap.erase(itor);
            n++;
            continue;
        }
        itor++;
        n++;
    }
    itor = testmap.begin();
    iend = testmap.end();

    while (itor != iend) {
        printf("%d\n", itor->second);
        itor ++;
    }
}

s32 main(int argc, char * args[]) {
    testHashMap();
    core::CoreHeaderInit();

    testCTimerEngine();

    core::CoreHeaderRelease();
    CSleep(1000);
    return 0;
}

