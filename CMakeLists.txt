#cmake file for project association                     #表示注释
#author:大T
#created:2013/4/9

#cmake 最低版本要求，低于2.6 构建过程会被终止
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

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
#SET(CMAKE_C_FLAGS_DEBUG "-g -std=c++0x -Wall -D_DEBUG")
IF(UNIX)
    SET(CMAKE_CXX_FLAGS "-g -std=c++0x -Wall -rdynamic -ldl -Dlinux -D_DEBUG")
    SET(CMAKE_CXX_FLAGS_DEBUG "-g -std=c++0x -rdynamic -ldl -Dlinux -D_DEBUG")
    SET(CMAKE_CXX_FLAGS_RELEASE "-g -std=c++0x -rdynamic -ldl -Dlinux")
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
    "${PROJECT_SOURCE_DIR}/Source/Public/MultiSys.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/MultiSys.cpp"
    "${PROJECT_SOURCE_DIR}/Source/Public/LinuxSys.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/MacSys.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/WinSys.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/CLock.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/CLockUnit.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/Pool.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/Tools.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/MacTools.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/LinuxTools.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/WinTools.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/Console.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/WinConsole.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/WinConsole.cpp"
    "${PROJECT_SOURCE_DIR}/Source/Public/CStream.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/CStream.cpp"
    "${PROJECT_SOURCE_DIR}/Source/Public/CDumper.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/CDumper.cpp"
)

MESSAGE(STATUS "Public files : ${PUBLIC_FILES}")

#添加子目录
ADD_SUBDIRECTORY(./Source/Net_library)
#ADD_SUBDIRECTORY(./Source/Log_library)
ADD_SUBDIRECTORY(./Source/Kernel)
