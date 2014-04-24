#cmake file for project association
#author:大T
#created:2013/4/9

#相关头文件的目录
IF(UNIX)
    INCLUDE_DIRECTORIES(
        "../Public"
        "../Library"
        "./API"
        "./Core"
    )
ENDIF(UNIX)

IF(WIN32)
    INCLUDE_DIRECTORIES(
        "../Public"
        "../Library"
        "./API"
        "./Core"
        "./Core/Interface"
        "./Core/Subunit"
    )
    
    SET(STATIC_LIBRARYS
        "${PROJECT_SOURCE_DIR}/Source/Library/CURL/libcurl_imp.lib"
    )

ENDIF(WIN32)

IF(WIN64)
    INCLUDE_DIRECTORIES(
        "../Public"
        "../Library"
        "./API"
        "./Core"
        "./Core/Interface"
        "./Core/Subunit"
    )
    
    SET(STATIC_LIBRARYS
        "${PROJECT_SOURCE_DIR}/Source/Library/CURL/libcurl_imp.lib"
    )
ENDIF(WIN64)

IF(APPLE)
    INCLUDE_DIRECTORIES(
        "../Public"
        "../Library"
        "./API"
        "./Core"
        "./Core/Interface"
        "./Core/Subunit"
    )
ENDIF(APPLE)

#设置变量，表示所有的源文件
SET(SOURCE_FILES
    "main.cpp"
)

SET(CORE_FILES
    "./Core/CoreHeader.h"
    "./Core/CoreHeader.cpp"
    "./Core/CCallEngine.h"
    "./Core/CHttpEngine.h"
    "./Core/CHttpEngine.cpp"
    "./Core/CTimer.h"
    "./Core/CTimer.cpp"
    "./Core/CTimerEngine.h"
    "./Core/CTimerEngine.cpp"
    "./Core/DBProxy.h"
    "./Core/DBProxy.cpp"
    "./Core/ITimerHandler.cpp"
    "./Core/Kernel.h"
    "./Core/Kernel.cpp"
)

SET(CORE_NET_SUBUNIT
    "./Core/Subunit/Header.h"
    "./Core/Subunit/Header.cpp"
    "./Core/Subunit/CInetAddress.h"
    "./Core/Subunit/CInetAddress.cpp"
)

SET(CORE_NET_WINDOWS
    "./Core/Windows/CConnection.h"
    "./Core/Windows/CNet.h"
    "./Core/Windows/CNet.cpp"
    "./Core/Windows/iocp.h"
    "./Core/Windows/iocp.c"
    "./Core/Windows/iocp_event.h"
    "./Core/Windows/iocp_event.c"
)

SET(CORE_INTERFACE
    "./Core/Interface/ICore.h"
    "./Core/Interface/ICallEngine.h"
    "./Core/Interface/IDBProxy.h"
    "./Core/Interface/INet.h"
    "./Core/Interface/ITimerEngine.h"
)

SET(API
    "./API/IKernel.h"
    "./API/IModule.h"
    "./API/ITimerHandler.h"
    "./API/IHttpHandler.h"
)

SOURCE_GROUP(\\ FILES ${SOURCE_FILES})
SOURCE_GROUP(Core FILES ${CORE_FILES})
SOURCE_GROUP(Core\\Interface FILES ${CORE_INTERFACE})
SOURCE_GROUP(Core\\Subunit FILES ${CORE_NET_SUBUNIT})
SOURCE_GROUP(Core\\Windows FILES ${CORE_NET_WINDOWS})
SOURCE_GROUP(Public\\tinyxml FILES ${TINYXML_FILES})
SOURCE_GROUP(Public FILES ${PUBLIC_FILES})
SOURCE_GROUP(API FILES ${API})

MESSAGE(STATUS "Public : ${PUBLIC_FILES}")
MESSAGE(STATUS "API : ${API}")
MESSAGE(STATUS "Core : ${CORE_FILES}")
MESSAGE(STATUS "Core/Interface : ${CORE_INTERFACE}")

#相关库文件的目录
#LINK_DIRECTORIES(
#/usr/local/lib
#)

#设置输出路径,并且生成可执行引擎文件
SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")

#依赖的库文件
ADD_EXECUTABLE(Kernel
    ${SOURCE_FILES}
    ${CORE_FILES}
    ${CORE_INTERFACE}
    ${CORE_NET_SUBUNIT}
    ${CORE_NET_WINDOWS}
    ${PUBLIC_FILES}
    ${TINYXML_FILES}
    ${API}
)

TARGET_LINK_LIBRARIES(Kernel ${STATIC_LIBRARYS})

