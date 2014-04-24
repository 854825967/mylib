#cmake file for project association                     #表示注释
#author:大T
#created:2013/4/9

#设置变量，表示所有的源文件
SET(SOURCE_FILES
    "Bird.h"
    "Bird.cpp"
    "Gate.h"
    "Gate.cpp"
    "main.cpp"
)

SOURCE_GROUP(\\ FILES ${SOURCE_FILES})
SOURCE_GROUP(Interface FILES ${INTERFACE})
SOURCE_GROUP(Public\\tinyxml FILES ${TINYXML_FILES})
SOURCE_GROUP(Public\\json FILES ${JSONCPP_FILES})
SOURCE_GROUP(Public FILES ${PUBLIC_FILES})
SOURCE_GROUP(API FILES ${API})
SOURCE_GROUP(Define FILES ${DEFINE})

SET(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")
ADD_LIBRARY("LibGate" SHARED 
    ${JSONCPP_FILES}
    ${TINYXML_FILES}
    ${SOURCE_FILES} 
    ${PUBLIC_FILES}
    ${INTERFACE}
    ${API}
    ${DEFINE}
)