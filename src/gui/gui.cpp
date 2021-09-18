#include <string>
#include <iostream>
#include <cstdlib>
#include "ImGuiFileDialog.h"
#include "gui/gui.h"

GUI::GUI(CPU &cpu, Bus &bus, IO &io): cpu{cpu}, bus{bus}, io{io} {
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
    display_tile_map();
    mem_edit.DrawWindow("Memory", &(bus.tmp_mem), 0xFFFF+1);
    mem_edit.DrawWindow("IO", &(io.data), 0x80);
    
    if (ImGuiFileDialog::Instance()->Display("ChCartKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) {
            filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            bus.insert_cartridge(new Cartridge(filePathName));
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

void GUI::display_tile_map() {
    int high_byte;
    int low_byte;
    int color;
    int col, row;
    uint8_t *curr_tile_data;

    // There are 384 tiles. We'll display them in 16x24 grid
    const int total_tile_count = 384;
    const int tiles_in_row = 16;
    const int tiles_rows = 24;
    // Each tile is 8x8 pixels
    const int surf_width = tiles_in_row * 8;
    const int surf_height = tiles_rows * 8;
    const int tile_data_start_addr = 0x8000;

    SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, surf_width, surf_height, 32, SDL_PIXELFORMAT_RGBA32);
    Uint32 *surface_pixels = static_cast<Uint32 *>(surface->pixels);
    Uint32 color_palette[] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};
    GLuint texture;

    curr_tile_data = bus.tmp_mem + tile_data_start_addr;
    for (int tile_no = 0; tile_no < total_tile_count; ++tile_no) {
        for (int tile_line_no = 0; tile_line_no < 8; ++tile_line_no) {
            // Each line is represented by 2 bytes
            low_byte = curr_tile_data[2 * tile_line_no];
            high_byte = curr_tile_data[2 * tile_line_no + 1];
            for (int bit_no = 7; bit_no >= 0; --bit_no) {
                color = ((high_byte >> bit_no) & 1) << 1 | ((low_byte >> bit_no) & 1);
                row = 8 * (tile_no / tiles_in_row) + tile_line_no;
                col = 8 * (tile_no % tiles_in_row) + (7 - bit_no);
                surface_pixels[(row * surf_width) + col] = color_palette[color];
            }
        }
        // Each tile is 16 bytes
        curr_tile_data += 16;
    }

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surf_width, surf_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
 
    ImGui::Begin("Tile Data", NULL);
    ImGui::Image(reinterpret_cast<ImTextureID>(texture), ImVec2(2 * surf_width, 2 * surf_height));
    SDL_FreeSurface(surface);
    ImGui::End();
    glDisable(GL_TEXTURE_2D);
}
