#cmake file for project association                     #表示注释
#author:大T
#created:2013/4/9

#相关头文件的目录
IF(UNIX)
    INCLUDE_DIRECTORIES(
        "../Public"
        "./API"
    )
ENDIF(UNIX)

IF(WIN32)
    INCLUDE_DIRECTORIES(
        "../Public"
        "./API"
    )
ENDIF(WIN32)

IF(WIN64)
    INCLUDE_DIRECTORIES(
        "../Public"
        "./API"
    )
ENDIF(WIN64)

IF(APPLE)
    INCLUDE_DIRECTORIES(
        "../Public"
        "./API"
    )
ENDIF(APPLE)

#设置变量，表示所有的源文件
SET(SOURCE_FILES
    "main.cpp"
)

SET(API
	"./API/IKernel.h"
	"./API/IModule.h"
)


SOURCE_GROUP(\\ FILES ${SOURCE_FILES})
SOURCE_GROUP(Public FILES ${PUBLIC_FILES})
SOURCE_GROUP(API FILES ${API})

MESSAGE(STATUS "Public : ${PUBLIC_FILES}")
MESSAGE(STATUS "API : ${API}")

#相关库文件的目录
#LINK_DIRECTORIES(
#/usr/local/lib
#)

#设置输出路径,并且生成可执行引擎文件
SET(EXECUTABLE_OUTPUT_PATH "${OUTPUT_PATH}/main")
ADD_EXECUTABLE(main
    ${SOURCE_FILES}
    ${PUBLIC_FILES}
    ${API}
)

#依赖的库文件
#TARGET_LINK_LIBRARY(association core)

#表示生成的执行文件所在路径
#SET_TARGET_PROPERTIES(utility PROPERTIES
#RUNTIME_OUTPUT_DIRECTORY> "${OUTPUT_PATH}/libnet")
