cmake_minimum_required (VERSION 3.2)
SET (PROJECT_NAME "xq_svr_zigbee")

project (${PROJECT_NAME})

IF(CROSS_COMPILER_PREFX)
    set (CMAKE_C_COMPILER "${CROSS_COMPILER_PREFX}gcc")
    set (CMAKE_CXX_COMPILER "${CROSS_COMPILER_PREFX}g++")
    set (CMAKE_STRIP "${CROSS_COMPILER_PREFX}strip")
ENDIF(CROSS_COMPILER_PREFX)

IF(HOST_PATH)
    SET(CMAKE_INSTALL_RPATH "${HOST_PATH}/lib/")
    include_directories("${HOST_PATH}/include/")
    include_directories("${HOST_PATH}/include/libxq")
    link_directories("${HOST_PATH}/lib")
    SET(CMAKE_INSTALL_PREFIX "${HOST_PATH}")
ENDIF(HOST_PATH)

aux_source_directory(${PROJECT_SOURCE_DIR} DIR_SRCS)

add_executable(${PROJECT_NAME}  ${DIR_SRCS})
target_link_libraries(${PROJECT_NAME} xq sqlite3 dl pthread)

install(TARGETS ${PROJECT_NAME}
        EXPORT xqTargets 
        RUNTIME DESTINATION bin  # 可执行文件安装路径
        )