#cmake file for project association                     #表示注释
#author:大T
#created:2013/4/9

#相关头文件的目录
INCLUDE_DIRECTORIES(
    "./Public"
    "./libnet/Interface"
    "./liblog/Interface"
)

#设置变量，表示所有的源文件
SET(SOURCE_FILES
    "./libnet/Linux/cepoll_event.c"
    "./libnet/Linux/cepoll.c"
    "main.cpp"
)


SET(LIBNET_INTERFACE_FILES
    "./libnet/Interface/INet.h"
    "./liblog/Interface/ILog.h"
)


SOURCE_GROUP(\\ FILES ${SOURCE_FILES})
SOURCE_GROUP(Interface FILES ${LIBNET_INTERFACE_FILES})
SOURCE_GROUP(Public FILES ${PUBLIC_FILES})

#相关库文件的目录
#LINK_DIRECTORIES(
#/usr/local/lib
#)

#设置输出路径,并且生成动态库
SET(EXECUTABLE_OUTPUT_PATH "${OUTPUT_PATH}/main")
ADD_EXECUTABLE(main
    ${SOURCE_FILES}
    ${PUBLIC_FILES}
    ${LIBNET_INTERFACE_FILES}
)

#依赖的库文件
#TARGET_LINK_LIBRARY(association core)

#表示生成的执行文件所在路径
#SET_TARGET_PROPERTIES(utility PROPERTIES
#RUNTIME_OUTPUT_DIRECTORY> "${OUTPUT_PATH}/main")
