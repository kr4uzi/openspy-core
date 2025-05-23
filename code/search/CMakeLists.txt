cmake_minimum_required (VERSION 3.22)

project(SM)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(libuv REQUIRED)
find_package(ZLIB REQUIRED)

file (GLOB MAIN_SRCS "*.cpp")
file (GLOB MAIN_HDRS "*.h")
file (GLOB SERVER_SRCS "server/*.cpp")
file (GLOB SERVER_HDRS "server/*.h")
file (GLOB SERVER_CMDS_SRCS "server/commands/*.cpp")
file (GLOB SERVER_CMDS_HDRS "server/commands/*.h")


set (ALL_SRCS ${MAIN_SRCS} ${MAIN_HDRS} ${SERVER_SRCS} ${SERVER_HDRS} ${SERVER_CMDS_SRCS})

include_directories (${CMAKE_CURRENT_SOURCE_DIR} SharedTasks)

source_group("Sources" FILES ${MAIN_SRCS})
source_group("Sources\\Server" FILES ${SERVER_SRCS})
source_group("Sources\\Commands\\Server" FILES ${SERVER_CMDS_SRCS})

source_group("Headers" FILES ${MAIN_HDRS})
source_group("Headers\\Server" FILES ${SERVER_HDRS})
source_group("Headers\\Commands\\Server" FILES ${SERVER_CMDS_HDRS})

add_executable (SM ${ALL_SRCS})

target_link_libraries(SM openspy SharedTasks)