#cmake file for project association                     #表示注释
#author:大T
#created:2013/4/9

#相关头文件的目录
IF(UNIX)
    INCLUDE_DIRECTORIES(
        "${PROJECT_SOURCE_DIR}/Source/Public"
        "${PROJECT_SOURCE_DIR}/Source/Kernel/API"
        "${PROJECT_SOURCE_DIR}/Source/Logic/Interface"
        "${PROJECT_SOURCE_DIR}/Source/Logic/Define"
    )
ENDIF(UNIX)

IF(WIN32)
    INCLUDE_DIRECTORIES(
        "${PROJECT_SOURCE_DIR}/Source/Public"
        "${PROJECT_SOURCE_DIR}/Source/Kernel/API"
        "${PROJECT_SOURCE_DIR}/Source/Logic/Interface"
        "${PROJECT_SOURCE_DIR}/Source/Logic/Define"
    )
ENDIF(WIN32)

IF(WIN64)
    INCLUDE_DIRECTORIES(
        "${PROJECT_SOURCE_DIR}/Source/Public"
        "${PROJECT_SOURCE_DIR}/Source/Kernel/API"
        "${PROJECT_SOURCE_DIR}/Source/Logic/Interface"
        "${PROJECT_SOURCE_DIR}/Source/Logic/Define"
    )
ENDIF(WIN64)

IF(APPLE)
    INCLUDE_DIRECTORIES(
        "${PROJECT_SOURCE_DIR}/Source/Public"
        "${PROJECT_SOURCE_DIR}/Source/Kernel/API"
        "${PROJECT_SOURCE_DIR}/Source/Logic/Interface"
        "${PROJECT_SOURCE_DIR}/Source/Logic/Define"
    )
ENDIF(APPLE)

SET(API
    "${PROJECT_SOURCE_DIR}/Source/Kernel/API/IKernel.h"
    "${PROJECT_SOURCE_DIR}/Source/Kernel/API/IModule.h"
    "${PROJECT_SOURCE_DIR}/Source/Kernel/API/ITimerHandler.h"
    "${PROJECT_SOURCE_DIR}/Source/Kernel/API/IHttpHandler.h"
)

SET(INTERFACE
    "${PROJECT_SOURCE_DIR}/Source/Logic/Interface/IMessage.h"
    "${PROJECT_SOURCE_DIR}/Source/Logic/Interface/IGate.h"
)

SET(DEFINE
    "${PROJECT_SOURCE_DIR}/Source/Logic/Define/Protocol.h"
    "${PROJECT_SOURCE_DIR}/Source/Logic/Define/UpdateFields.h"
    "${PROJECT_SOURCE_DIR}/Source/Logic/Define/UpdateFields.cpp"
)

ADD_SUBDIRECTORY(./LibMessage)
ADD_SUBDIRECTORY(./LibGate)
