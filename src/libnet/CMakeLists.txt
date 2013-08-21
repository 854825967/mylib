#cmake file for project association                     #表示注释 
#author:大T
#created:2013/4/9

#相关头文件的目录
INCLUDE_DIRECTORIES(
    "../Public"
    "./Interface"
    "./Subunit"
    "./Windows"
    "./Linux"
    "./MacOS"
	"../liblog/Interface"
)

#设置变量，表示所有的源文件
SET(SOURCE_FILES
    "main.cpp"
)

SET(WINDOWS_NET_FILES
    "./Windows/SockData.h"
    "./Windows/SockPlus.h"
    "./Windows/WinNetCore.cpp"
    "./Windows/WinNetCore.h"
)

SET(LINUX_NET_FILES
    "./Linux/LinuxNetCore.cpp"
    "./Linux/LinuxNetCore.h"
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
    "Subunit/CQueue.h"
    "Subunit/CQueue.cpp"
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
ADD_LIBRARY("libnet" SHARED 
    ${SOURCE_FILES} 
    ${WINDOWS_NET_FILES}
    ${LINUX_NET_FILES} 
    ${MAC_OS_NET_FILES}
    ${PUBLIC_FILES}
    ${SUBUNIT_FILES}
    ${LIBNET_INTERFACE_FILES}
)

#依赖的库文件
#TARGET_LINK_LIBRARY(association core)

#表示生成的执行文件所在路径
#SET_TARGET_PROPERTIES(utility PROPERTIES
#RUNTIME_OUTPUT_DIRECTORY> "${OUTPUT_PATH}/libnet")
