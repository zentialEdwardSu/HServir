cmake_minimum_required(VERSION 3.22)
project(HServir)
set(CMAKE_C_STANDARD 11)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

link_directories(${PROJECT_SOURCE_DIR}/lib)

# AUX_SOURCE_DIRECTORY(${PROJECT_SROUCE_DIR}/src DIR_SRCS)

add_executable(${PROJECT_NAME} src/server.c)

target_link_libraries(${PROJECT_NAME} WS2_32.lib)