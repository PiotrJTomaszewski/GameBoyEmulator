cmake_minimum_required(VERSION 3.10)

project(GameBoyEmu)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

add_subdirectory(emulator)
add_subdirectory(test)

if (NOT BUILD_ONLY_TESTS)
    include(cmake/imgui.cmake)
    
    include_directories(
        ${PROJECT_SOURCE_DIR}/inc
        ${PROJECT_SOURCE_DIR}/emulator/inc
    )

    file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
    add_executable(GameBoyEmu ${SOURCES})
    target_link_libraries(GameBoyEmu
        GameBoyEmuLib
        ${IMGUI_LIBRARIES}
    )
endif()
