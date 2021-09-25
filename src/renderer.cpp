#include "renderer.h"

Renderer::Renderer(VideoRAM &vram, PPU &ppu): vram(vram), ppu(ppu) {
    GLuint textures[2];
    glGenTextures(2, textures);

    // TODO: Change to the real size later
    screen_render.width = 256;
    screen_render.height = 256;
    screen_render.texture = textures[0];
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screen_render.texture);

    // There are 384 tiles. We'll display them in 16x24 grid. Each tile is 8x8px
    tile_data_render.width = 16 * 8;
    tile_data_render.height = 24 * 8;
    tile_data_surface = SDL_CreateRGBSurfaceWithFormat(0, tile_data_render.width, tile_data_render.height, 32, SDL_PIXELFORMAT_RGBA32);
    tile_data_render.texture = textures[1];
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tile_data_render.texture);
}

Renderer::~Renderer() {
    SDL_FreeSurface(tile_data_surface);
    glDeleteTextures(1, &tile_data_render.texture);
    glDeleteTextures(1, &screen_render.texture);
}

// TODO: Only render if data is dirty
void Renderer::render_tile_data() {
    int high_byte;
    int low_byte;
    int color;
    int col, row;
    uint8_t *curr_tile_data;

    // There are 384 tiles. We'll display them in 16x24 grid
    const int total_tile_count = 384;

    Uint32 *surface_pixels = static_cast<Uint32 *>(tile_data_surface->pixels);
    Uint32 color_palette[] = {0xFF000000, 0xFF555555, 0xFFAAAAAA, 0xFFFFFFFF};

    curr_tile_data = vram.get_raw_data(); // Tile data starts at the first byte of VRAM
    for (int tile_no = 0; tile_no < total_tile_count; ++tile_no) {
        for (int tile_line_no = 0; tile_line_no < 8; ++tile_line_no) {
            // Each line is represented by 2 bytes
            low_byte = curr_tile_data[2 * tile_line_no];
            high_byte = curr_tile_data[2 * tile_line_no + 1];
            for (int bit_no = 7; bit_no >= 0; --bit_no) {
                color = ((high_byte >> bit_no) & 1) << 1 | ((low_byte >> bit_no) & 1);
                row = 8 * (tile_no / TILE_DATA_TILES_IN_ROW) + tile_line_no;
                col = 8 * (tile_no % TILE_DATA_TILES_IN_ROW) + (7 - bit_no);
                surface_pixels[(row * tile_data_render.width) + col] = color_palette[color];
            }
        }
        // Each tile is 16 bytes
        curr_tile_data += 16;
    }
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tile_data_render.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tile_data_render.width, tile_data_render.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface_pixels);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glDisable(GL_TEXTURE_2D);
}

void Renderer::render_screen() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, screen_render.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_render.width, screen_render.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ppu.get_screen_pixels());
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glDisable(GL_TEXTURE_2D);
}

Renderer::render_t &Renderer::get_tile_data_render(){
    return tile_data_render;
}

Renderer::render_t &Renderer::get_screen_render() {
    return screen_render;
}
