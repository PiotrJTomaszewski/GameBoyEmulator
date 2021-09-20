project(GameBoyEmuTest)

file(GLOB_RECURSE TEST_SOURCES "./src/*.cpp")

include_directories(
    ${PROJECT_SOURCE_DIR}/lib/doctest
    ${PROJECT_SOURCE_DIR}/../include
    )

add_executable(GameBoyEmuTest ${TEST_SOURCES})
