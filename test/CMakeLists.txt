project(GameBoyEmuTest C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE TEST_SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")

include_directories(
    ${PROJECT_SOURCE_DIR}/inc
    ${PROJECT_SOURCE_DIR}/lib/doctest
    ${PROJECT_SOURCE_DIR}/../emulator/inc
)

add_executable(GameBoyEmuTest ${TEST_SOURCES})
target_link_libraries(GameBoyEmuTest GameBoyEmuLib)
