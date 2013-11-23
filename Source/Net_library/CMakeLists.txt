#cmake file for project association                     #表示注释
#author:大T
#created:2013/4/9

#相关头文件的目录
IF(UNIX)
    INCLUDE_DIRECTORIES(
        "../Public"
        "./Interface"
        "./Subunit"
        "./Linux"
    )
ENDIF(UNIX)

IF(WIN32)
    INCLUDE_DIRECTORIES(
        "../Public"
        "./Interface"
        "./Subunit"
        "./Windows"
    )
ENDIF(WIN32)

IF(WIN64)
    INCLUDE_DIRECTORIES(
        "../Public"
        "./Interface"
        "./Subunit"
        "./Windows"
    )
ENDIF(WIN64)

IF(APPLE)
    INCLUDE_DIRECTORIES(
        "../Public"
        "./Interface"
        "./Subunit"
        "./MacOS"
    )
ENDIF(APPLE)

#设置变量，表示所有的源文件
SET(SOURCE_FILES
    "main.cpp"
)

SET(WINDOWS_NET_FILES
    "./Windows/CConnection.h"
    "./Windows/CNet.h"
    "./Windows/CNet.cpp"
    "./Windows/CQueue.h"
    "./Windows/event_queue.h"
    "./Windows/iocp.h"
    "./Windows/iocp.c"
    "./Windows/iocp_event.h"
    "./Windows/iocp_event.c"
)

SET(LINUX_NET_FILES
    "./Linux/CConnection.h"
    "./Linux/CNet.cpp"
    "./Linux/CNet.h"
    "./Linux/CQueue.h"
    "./Linux/cepoll.h"
    "./Linux/cepoll.c"
    "./Linux/cepoll_event.h"
    "./Linux/cepoll_event.c"
)

SET(MAC_OS_NET_FILES
    "./MacOS/MacNetCore.cpp"
    "./MacOS/MacNetCore.h"
)

SET(LIBNET_INTERFACE_FILES
    "Interface/INet.h"
)

SET(SUBUNIT_FILES
    "Subunit/CInetAddress.h"
    "Subunit/CInetAddress.cpp"
)

SOURCE_GROUP(\\ FILES ${SOURCE_FILES})
SOURCE_GROUP(Subunit FILES ${SUBUNIT_FILES})
SOURCE_GROUP(Interface FILES ${LIBNET_INTERFACE_FILES})
SOURCE_GROUP(Public FILES ${PUBLIC_FILES})
SOURCE_GROUP(Windows FILES ${WINDOWS_NET_FILES})
SOURCE_GROUP(Linux FILES ${LINUX_NET_FILES})
SOURCE_GROUP(Mac FILES ${MAC_OS_NET_FILES})

MESSAGE(STATUS "Public : ${PUBLIC_FILES}")

#相关库文件的目录
#LINK_DIRECTORIES(
#/usr/local/lib
#)

#设置输出路径,并且生成动态库
SET(LIBRARY_OUTPUT_PATH "${OUTPUT_PATH}/libnet")

IF(UNIX)
ADD_LIBRARY("net" SHARED
    ${SOURCE_FILES}
    ${LINUX_NET_FILES}
    ${PUBLIC_FILES}
    ${SUBUNIT_FILES}
    ${LIBNET_INTERFACE_FILES}
)
ENDIF(UNIX)

IF(WIN32)
ADD_LIBRARY("libnet" SHARED
    ${SOURCE_FILES}
    ${WINDOWS_NET_FILES}
    ${PUBLIC_FILES}
    ${SUBUNIT_FILES}
    ${LIBNET_INTERFACE_FILES}
)
ENDIF(WIN32)

IF(WIN64)
ADD_LIBRARY("libnet" SHARED
    ${SOURCE_FILES}
    ${WINDOWS_NET_FILES}
    ${PUBLIC_FILES}
    ${SUBUNIT_FILES}
    ${LIBNET_INTERFACE_FILES}
)
ENDIF(WIN64)

IF(APPLE)
ADD_LIBRARY("libnet" SHARED
    ${SOURCE_FILES}
    ${MAC_OS_NET_FILES}
    ${PUBLIC_FILES}
    ${SUBUNIT_FILES}
    ${LIBNET_INTERFACE_FILES}
)
ENDIF(APPLE)

#依赖的库文件
#TARGET_LINK_LIBRARY(association core)

#表示生成的执行文件所在路径
#SET_TARGET_PROPERTIES(utility PROPERTIES
#RUNTIME_OUTPUT_DIRECTORY> "${OUTPUT_PATH}/libnet")
