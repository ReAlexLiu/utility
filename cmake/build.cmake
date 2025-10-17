# CMakeLists.txt
# CMAKE_CURRENT_LIST_DIR 当前cmake所在路径
# CMAKE_CURRENT_SOURCE_DIR 项目根路径

# 获取当前时间
execute_process(
        COMMAND date "+%Y-%m-%d %H:%M:%S"
        OUTPUT_VARIABLE BUILD_TIMESTAMP
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

# 获取 Git 版本信息
execute_process(
        COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
        COMMAND git describe --tags --always
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(STATUS "Git Commit Hash: ${CMAKE_CURRENT_SOURCE_DIR}")
# 将模板文件转换为实际的头文件
configure_file(${CMAKE_CURRENT_LIST_DIR}/../build.h.in ${CMAKE_CURRENT_SOURCE_DIR}/build.h)
