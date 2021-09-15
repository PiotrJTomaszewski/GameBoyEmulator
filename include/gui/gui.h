#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "cpu/cpu.h"

class GUI {
public:
    GUI(CPU &cpu);
    ~GUI();
    void display();
    bool get_should_close();
private:
    CPU &cpu;
    SDL_Window *window;
    SDL_GLContext gl_context;
    ImGuiIO io;
    const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool should_close;
    void handle_events();
    void display_cpu();
};
