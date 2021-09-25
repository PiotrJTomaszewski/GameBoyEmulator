#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "memory/video_ram.h"
#include "ppu/ppu.h"

class Renderer {
public:
    struct render_t {
        GLuint texture;
        int width;
        int height;
    };

public:
    Renderer(VideoRAM &vram, PPU &ppu);
    ~Renderer();
    void render_tile_data();
    void render_screen();
    render_t &get_tile_data_render();
    render_t &get_screen_render();

private:
    const static int TILE_DATA_TILES_IN_ROW = 16;
    VideoRAM &vram;
    PPU &ppu;
    render_t tile_data_render, screen_render;
    SDL_Surface *tile_data_surface;
};
