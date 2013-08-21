#cmake file for project association                     #表示注释 
#author:大T
#created:2013/4/9

#cmake 最低版本要求，低于2.6 构建过程会被终止
CMAKE_MINIMUM_REQUIRED(VERSION 2.8) 

#定义工程名称
PROJECT(mylib)

#打印相关消息消息
MESSAGE(STATUS "Project: mylib")
MESSAGE(STATUS "Project Directory: ${PROJECT_SOURCE_DIR}")

#指定编译类型
#SET(CMAKE_BUILE_TYPE DEBUG)

#设置输出路径变量
SET(OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")

#指定编译器 
#SET(CMAKE_C_FLAGS_DEBUG "-g -Wall")          

IF(UNIX)
	MESSAGE(STATUS "UNIX")
ENDIF(UNIX)

IF(WIN32)
	MESSAGE(STATUS "WIN32")
ENDIF(WIN32)

IF(WIN64)
	MESSAGE(STATUS "WIN64")
ENDIF(WIN64)

IF(APPLE)
	MESSAGE(STATUS "APPLE")
ENDIF(APPLE)

#设置公共库代码目录
SET(PUBLIC_FILES
    "${PROJECT_SOURCE_DIR}/src/Public/MultiSys.h"
    "${PROJECT_SOURCE_DIR}/src/Public/LinuxSys.h"
    "${PROJECT_SOURCE_DIR}/src/Public/MacSys.h"
    "${PROJECT_SOURCE_DIR}/src/Public/WinSys.h"
    "${PROJECT_SOURCE_DIR}/src/Public/CLock.h"
    "${PROJECT_SOURCE_DIR}/src/Public/CLockUnit.h"
    "${PROJECT_SOURCE_DIR}/src/Public/Pool.h"
    "${PROJECT_SOURCE_DIR}/src/Public/Tools.h"
    "${PROJECT_SOURCE_DIR}/src/Public/MacTools.h"
    "${PROJECT_SOURCE_DIR}/src/Public/LinuxTools.h"
    "${PROJECT_SOURCE_DIR}/src/Public/WinTools.h"
    "${PROJECT_SOURCE_DIR}/src/Public/Console.h"
    "${PROJECT_SOURCE_DIR}/src/Public/WinConsole.h"
    "${PROJECT_SOURCE_DIR}/src/Public/WinConsole.cpp"
    "${PROJECT_SOURCE_DIR}/src/Public/CStream.h"
    "${PROJECT_SOURCE_DIR}/src/Public/CStream.cpp"
    "${PROJECT_SOURCE_DIR}/src/Public/CDumper.h"
    "${PROJECT_SOURCE_DIR}/src/Public/CDumper.cpp"
)

MESSAGE(STATUS "Public files : ${PUBLIC_FILES}")

#添加子目录
ADD_SUBDIRECTORY(./src/libnet)
ADD_SUBDIRECTORY(./src/liblog)
ADD_SUBDIRECTORY(./src)
