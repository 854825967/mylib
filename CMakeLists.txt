#cmake file for project association                     #表示注释
#author:大T
#created:2013/4/9

#cmake 最低版本要求，低于2.6构建过程会被终止
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
IF(UNIX)
    SET(CMAKE_CXX_FLAGS "-g -std=c++0x -Wall -rdynamic -ldl -Dlinux -D_DEBUG")
    SET(CMAKE_CXX_FLAGS_DEBUG "-g -std=c++0x -rdynamic -ldl -Dlinux -D_DEBUG")
    SET(CMAKE_CXX_FLAGS_RELEASE "-g -std=c++0x -rdynamic -ldl -Dlinux")
    MESSAGE(STATUS "UNIX")
ENDIF(UNIX)

IF(WIN32)
    SET(CMAKE_C_FLAGS_DEBUG_INIT "/D_DEBUG /Zm8192000 /MTd /Zi /Ob0 /Od /RTC1") 
    SET(CMAKE_C_FLAGS_MINSIZEREL_INIT "/MT /Zm8192000 /O1 /Ob1 /D NDEBUG") 
    SET(CMAKE_C_FLAGS_RELEASE_INIT "/MT /Zm8192000 /O2 /Ob2 /D NDEBUG") 
    SET(CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "/MT /Zm8192000 /Zi /O2 /Ob1 /D NDEBUG")
    MESSAGE(STATUS "WIN32")
ENDIF(WIN32)

IF(WIN64)
    SET(CMAKE_C_FLAGS_DEBUG_INIT "/D_DEBUG /Zm8192000 /MTd /Zi /Ob0 /Od /RTC1") 
    SET(CMAKE_C_FLAGS_MINSIZEREL_INIT "/MT /Zm8192000 /O1 /Ob1 /D NDEBUG") 
    SET(CMAKE_C_FLAGS_RELEASE_INIT "/MT /Zm8192000 /O2 /Ob2 /D NDEBUG") 
    SET(CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "/MT /Zm8192000 /Zi /O2 /Ob1 /D NDEBUG")
    MESSAGE(STATUS "WIN64")
ENDIF(WIN64)

IF(APPLE)
    MESSAGE(STATUS "APPLE")
ENDIF(APPLE)

#设置公共库代码目录
SET(JSONCPP_FILES
    "${PROJECT_SOURCE_DIR}/Source/Public/json/autolink.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/json/config.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/json/features.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/json/forwards.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/json/json.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/json/json_batchallocator.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/json/json_internalarray.inl"
    "${PROJECT_SOURCE_DIR}/Source/Public/json/json_internalmap.inl"
    "${PROJECT_SOURCE_DIR}/Source/Public/json/json_reader.cpp"
    "${PROJECT_SOURCE_DIR}/Source/Public/json/json_value.cpp"
    "${PROJECT_SOURCE_DIR}/Source/Public/json/json_valueiterator.inl"
    "${PROJECT_SOURCE_DIR}/Source/Public/json/json_writer.cpp"
    "${PROJECT_SOURCE_DIR}/Source/Public/json/reader.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/json/value.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/json/writer.h"
)

SET(TINYXML_FILES
    "${PROJECT_SOURCE_DIR}/Source/Public/tinyxml/tinystr.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/tinyxml/tinystr.CPP"
    "${PROJECT_SOURCE_DIR}/Source/Public/tinyxml/tinyxml.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/tinyxml/tinyxml.cpp"
    "${PROJECT_SOURCE_DIR}/Source/Public/tinyxml/tinyxmlparser.cpp"
    "${PROJECT_SOURCE_DIR}/Source/Public/tinyxml/tinyxmlerror.cpp"
)

SET(PUBLIC_FILES
    "${PROJECT_SOURCE_DIR}/Source/Public/MultiSys.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/MultiSys.cpp"
    "${PROJECT_SOURCE_DIR}/Source/Public/LinuxSys.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/MacSys.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/WinSys.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/CLock.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/CLockUnit.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/TPool.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/TQueue.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/Tools.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/CDumper.cpp"
    "${PROJECT_SOURCE_DIR}/Source/Public/MacTools.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/LinuxTools.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/WinTools.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/Console.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/WinConsole.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/WinConsole.cpp"
    "${PROJECT_SOURCE_DIR}/Source/Public/TStream.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/TAttribute.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/CDumper.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/CDumper.cpp"
    "${PROJECT_SOURCE_DIR}/Source/Public/TLogger.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/TCallBack.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/Logfile.cpp"
    "${PROJECT_SOURCE_DIR}/Source/Public/IOSSys.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/CHashMap.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/IOSTools.h"
    "${PROJECT_SOURCE_DIR}/Source/Public/Logfile.cpp"
)

#添加子目录
#ADD_SUBDIRECTORY(./Source/Net_library)
#ADD_SUBDIRECTORY(./Source/Log_library)
ADD_SUBDIRECTORY(./Source/Kernel)
ADD_SUBDIRECTORY(./Source/Logic)
