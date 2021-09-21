project(GameBoyEmuTest C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE TEST_SOURCES "./src/*.cpp")

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/lib/doctest
    ${PROJECT_SOURCE_DIR}/../include
    ${SDL2_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
    )

file(GLOB_RECURSE EMU_SOURCES "../src/*.cpp")
list(FILTER EMU_SOURCES EXCLUDE REGEX ".*/main.cpp$")
list(FILTER EMU_SOURCES EXCLUDE REGEX ".*/gui.cpp$")

add_executable(GameBoyEmuTest ${TEST_SOURCES} ${EMU_SOURCES})
target_link_libraries(GameBoyEmuTest ${IMGUI_LIBRARIES})
