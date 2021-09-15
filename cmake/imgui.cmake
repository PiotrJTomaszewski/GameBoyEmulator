# Based on https://github.com/shxy0/SDLImGui
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(X11 REQUIRED)

# ImGUI
set(IMGUI_INCLUDE_DIR lib/imgui)
file(GLOB IMGUI_SRC ${IMGUI_INCLUDE_DIR}/*.cpp)
file(GLOB IMGUI_HDRS ${IMGUI_INCLUDE_DIR}/*h)
set(IMGUI_IMPL_DIR ${IMGUI_INCLUDE_DIR}/backends)

add_library(imgui STATIC ${IMGUI_SRC} ${IMGUI_IMPL_DIR}/imgui_impl_sdl.cpp ${IMGUI_IMPL_DIR}/imgui_impl_opengl3.cpp)

add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD)

include_directories(
    ${IMGUI_INCLUDE_DIR}
    ${IMGUI_IMPL_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIRS}
    )

target_link_libraries(imgui ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES})

set_target_properties(imgui PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(imgui PROPERTIES FOLDER lib)
set(IMGUI_LIBRARIES imgui)
