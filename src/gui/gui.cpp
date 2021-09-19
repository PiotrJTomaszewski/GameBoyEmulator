#include <string>
#include <iostream>
#include <cstdlib>
#include "ImGuiFileDialog.h"
#include "gui/gui.h"

GUI::GUI(CPU &cpu, Bus &bus, IO &io, PPU &ppu): cpu{cpu}, bus{bus}, io{io}, ppu{ppu} {
    // From https://github.com/ocornut/imgui/blob/master/examples/example_sdl_opengl3/main.cpp
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        // TODO: Throw exception
    }

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    window = SDL_CreateWindow("GameBoy Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
    gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    imgui_io = ImGui::GetIO(); (void)imgui_io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    // ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);
    
    should_close = false;
    mem_edit.ReadOnly = true;
}

GUI::~GUI() {
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void GUI::display() {
    std::string filePathName;

    handle_events();
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    display_main_menu();
    display_cpu();
    display_tile_data();
    display_screen();
    display_timer();
    mem_edit.DrawWindow("Memory", &(bus.tmp_mem), 0xFFFF+1);
    mem_edit.DrawWindow("IO", &(io.data), 0x80);
    
    if (ImGuiFileDialog::Instance()->Display("ChCartKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            bus.insert_cartridge(new Cartridge(filePathName));
            cpu.restart();
        }
        ImGuiFileDialog::Instance()->Close();
    }

    // Rendering
    ImGui::Render();
    glViewport(0, 0, (int)imgui_io.DisplaySize.x, (int)imgui_io.DisplaySize.y);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(window);
}

bool GUI::get_should_close() {
    return should_close;
}

void GUI::handle_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
            should_close = true;
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
            should_close = true;
    }
}

void GUI::display_main_menu() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Cartridge")) {
            if (ImGui::MenuItem("Open..", "Ctrl+O")) {
                ImGuiFileDialog::Instance()->OpenDialog("ChCartKey", "Choose a Cartridge file", ".gb", ".");
            }
            if (ImGui::MenuItem("Dump")) {
                bus.tmp_dump();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void GUI::display_cpu() {
    flags_reg_t cpu_flags_reg = cpu.get_flags_reg();

    ImGui::Begin("CPU", NULL);
    // Registers
    ImGui::TextColored(ImVec4(1,1,0,1), "Registers");
    ImGui::BeginTable("#cpu_registers_table", 3);
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("A: 0x%02X", cpu.get_regA());
    ImGui::TableNextColumn();
    ImGui::Text("F: 0x%02X", cpu_flags_reg.value);
    ImGui::TableNextColumn();
    ImGui::Text("AF: 0x%04X", (cpu.get_regA() << 8) | cpu_flags_reg.value);

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("B: 0x%02X", cpu.get_regB());
    ImGui::TableNextColumn();
    ImGui::Text("C: 0x%02X", cpu.get_regC());
    ImGui::TableNextColumn();
    ImGui::Text("BC: 0x%04X", cpu.get_regBC());

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("D: 0x%02X", cpu.get_regD());
    ImGui::TableNextColumn();
    ImGui::Text("E: 0x%02X", cpu.get_regE());
    ImGui::TableNextColumn();
    ImGui::Text("DE: 0x%04X", cpu.get_regDE());

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("H: 0x%02X", cpu.get_regH());
    ImGui::TableNextColumn();
    ImGui::Text("L: 0x%02X", cpu.get_regL());
    ImGui::TableNextColumn();
    ImGui::Text("HL: 0x%04X", cpu.get_regHL());

    ImGui::EndTable();

    // Flags
    ImGui::BeginTable("#flags_and_regs_table", 2);
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("PC: 0x%04X", cpu.get_regPC());
    ImGui::TableNextColumn();
    ImGui::Text("SP: 0x%04X", cpu.get_regSP());
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::TextColored(ImVec4(1,1,0,1), "Flags");
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("Z: %d", cpu_flags_reg.flags.Z);
    ImGui::TableNextColumn();
    ImGui::Text("N: %d", cpu_flags_reg.flags.N);
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("H: %d", cpu_flags_reg.flags.H);
    ImGui::TableNextColumn();
    ImGui::Text("C: %d", cpu_flags_reg.flags.C);
    ImGui::EndTable();
    ImGui::End();
}

void GUI::display_tile_data() {
    ImGui::Begin("Tile Data", NULL);
    PPU::render_t &render = ppu.get_tile_data_render();
    ImGui::Image(reinterpret_cast<ImTextureID>(render.texture), ImVec2(2 * render.width, 2 * render.height));
    ImGui::End();
}

void GUI::display_screen() {
    ImGui::Begin("Screen", NULL);
    PPU::render_t &render = ppu.get_screen_render();
    ImGui::Image(reinterpret_cast<ImTextureID>(render.texture), ImVec2(2 * render.width, 2 * render.height));
    ImGui::End();
}

void GUI::display_timer() {
    ImGui::Begin("Timer", NULL);
    ImGui::Text("DIV: 0x%02X", io.timer.get_DIV());
    ImGui::Text("TIMA: 0x%02X", io.timer.get_TIMA());
    ImGui::Text("TMA: %02X", io.timer.get_TMA());
    ImGui::Text("Enabled: %d", io.timer.get_TAC_is_enabled());
    ImGui::Text("Divider: %d", io.timer.get_TAC_clock_divider());
    ImGui::End();
}
