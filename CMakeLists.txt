cmake_minimum_required(VERSION 3.10)

project(GameBoyEmu VERSION 0.1)

include(cmake/imgui.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(
    include
    ${OPENGL_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIR}
    ${IMGUI_INCLUDE_DIR}
    )

file(GLOB SOURCES "src/*.cpp")
add_executable(GameBoyEmu ${SOURCES})
target_link_libraries(GameBoyEmu
    ${OPENGL_LIBRARIES}
    ${SDL2_LIBRARIES}
    ${IMGUI_LIBRARIES}
    dl
    )