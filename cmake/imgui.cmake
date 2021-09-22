cmake_policy(SET CMP0072 NEW)
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

# ImGUI
set(IMGUI_INCLUDE_DIR lib/imgui)
file(GLOB IMGUI_SRC ${IMGUI_INCLUDE_DIR}/*.cpp)
file(GLOB IMGUI_HDRS ${IMGUI_INCLUDE_DIR}/*h)
set(IMGUI_IMPL_DIR ${IMGUI_INCLUDE_DIR}/backends)

add_library(imgui STATIC
    ${IMGUI_SRC}
    ${IMGUI_IMPL_DIR}/imgui_impl_sdl.cpp
    ${IMGUI_IMPL_DIR}/imgui_impl_opengl3.cpp
    lib/ImGuiFileDialog/ImGuiFileDialog.cpp
)

add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD)

include_directories(
    ${IMGUI_INCLUDE_DIR}
    ${IMGUI_IMPL_DIR}
    ${OPENGL_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIRS}
    lib/imgui_club/imgui_memory_editor
    lib/ImGuiFileDialog
    )

target_link_libraries(imgui ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES} dl)
target_compile_options(imgui PRIVATE "-w")

set_target_properties(imgui PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(imgui PROPERTIES FOLDER lib)
set(IMGUI_LIBRARIES imgui)
