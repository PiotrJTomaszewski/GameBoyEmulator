#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "imgui_memory_editor.h"

#include "cpu/cpu.h"
#include "bus.h"
#include "io/io.h"
#include "ppu.h"

class GUI {
public:
    GUI(CPU &cpu, Bus &bus, IO &io, PPU &ppu);
    ~GUI();
    void display();
    bool get_should_close();
private:
    CPU &cpu;
    Bus &bus;
    IO &io;
    PPU &ppu;
    SDL_Window *window;
    SDL_GLContext gl_context;
    ImGuiIO imgui_io;
    MemoryEditor mem_edit;
    const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool should_close;
    void handle_events();
    void display_main_menu();
    void display_cpu();
    void display_tile_data();
    void display_screen();
    void display_timer();
};
