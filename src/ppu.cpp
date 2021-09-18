#include <iostream>
#include "ppu.h"

PPU::PPU(IO &io, Bus &bus): io{io}, bus{bus} {
    LCD_data = (LCD_data_t *)(io.data + 0xFF40 - 0xFF00);
    // TODO: Remove
    LCD_data->LY = 0x90;

    GLuint textures[2];
    glGenTextures(2, textures);

    // TODO: Change to the real size later
    screen_render.width = 256;
    screen_render.height = 256;
    screen_render.surface = SDL_CreateRGBSurfaceWithFormat(0, screen_render.width, screen_render.height, 32, SDL_PIXELFORMAT_RGBA32);
    screen_render.texture = textures[0];
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screen_render.texture);

    // There are 384 tiles. We'll display them in 16x24 grid. Each tile is 8x8px
    tile_data_render.width = 16 * 8;
    tile_data_render.height = 24 * 8;
    tile_data_render.surface = SDL_CreateRGBSurfaceWithFormat(0, tile_data_render.width, tile_data_render.height, 32, SDL_PIXELFORMAT_RGBA32);
    tile_data_render.texture = textures[1];
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tile_data_render.texture);
}

PPU::~PPU() {
    SDL_FreeSurface(tile_data_render.surface);
    glDeleteTextures(1, &tile_data_render.texture);
    SDL_FreeSurface(screen_render.surface);
    glDeleteTextures(1, &screen_render.texture);
}

void PPU::tmp_tick() {
    // TODO: Remove
    // LCD_data->LY = (LCD_data->LY + 1) % 154;
}

void PPU::render_screen() {
    int tile_id;
    uint8_t *tile_data;
    SDL_Rect src_rect;
    src_rect.w = 8;
    src_rect.h = 8;
    SDL_Rect dst_rect;
    dst_rect.w = 8;
    dst_rect.h = 8;

    if (LCD_data->LCD_control.BG_tile_map_area == map_area_t::AREA_9800) {
       tile_data = bus.tmp_mem + 0x9800;
    } else {
        tile_data = bus.tmp_mem + 0x9C00;
    }

    for (int tile_row = 0; tile_row < 32; ++tile_row) {
        for (int tile_col = 0; tile_col < 32; ++tile_col) {
            // TODO: Add comment
            if (LCD_data->LCD_control.BG_and_window_tile_data_area == data_area_t::AREA_8000) {
                tile_id = tile_data[32 * tile_row + tile_col];
            } else {
                tile_id = 128 + static_cast<int8_t>(tile_data[32 * tile_row + tile_col]);
            }
            // std::cout << tile_id << " ";
            src_rect.x = (tile_id % tile_data_tiles_in_row) * 8;
            src_rect.y = (tile_id / tile_data_tiles_in_row) * 8;
            dst_rect.x = 8 * tile_col;
            dst_rect.y = 8 * tile_row;
            SDL_BlitSurface(tile_data_render.surface, &src_rect, screen_render.surface, &dst_rect);
        }
    }
    // std::cout << std::endl;

    glEnable(GL_TEXTURE_2D);
    // glActiveTexture(screen_render.texture);
    glBindTexture(GL_TEXTURE_2D, screen_render.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_render.width, screen_render.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, screen_render.surface->pixels);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glDisable(GL_TEXTURE_2D);
}


void PPU::render_tile_data() {
    int high_byte;
    int low_byte;
    int color;
    int col, row;
    uint8_t *curr_tile_data;

    // There are 384 tiles. We'll display them in 16x24 grid
    const int total_tile_count = 384;
    const int tile_data_start_addr = 0x8000;

    Uint32 *surface_pixels = static_cast<Uint32 *>(tile_data_render.surface->pixels);
    // Uint32 color_palette[] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000};
    Uint32 color_palette[] = {0xFF000000, 0xFF555555, 0xFFAAAAAA, 0xFFFFFFFF};

    curr_tile_data = bus.tmp_mem + tile_data_start_addr;
    for (int tile_no = 0; tile_no < total_tile_count; ++tile_no) {
        for (int tile_line_no = 0; tile_line_no < 8; ++tile_line_no) {
            // Each line is represented by 2 bytes
            low_byte = curr_tile_data[2 * tile_line_no];
            high_byte = curr_tile_data[2 * tile_line_no + 1];
            for (int bit_no = 7; bit_no >= 0; --bit_no) {
                color = ((high_byte >> bit_no) & 1) << 1 | ((low_byte >> bit_no) & 1);
                row = 8 * (tile_no / tile_data_tiles_in_row) + tile_line_no;
                col = 8 * (tile_no % tile_data_tiles_in_row) + (7 - bit_no);
                surface_pixels[(row * tile_data_render.width) + col] = color_palette[color];
            }
        }
        // Each tile is 16 bytes
        curr_tile_data += 16;
    }
    glEnable(GL_TEXTURE_2D);
    // glActiveTexture(tile_data_render.texture);
    glBindTexture(GL_TEXTURE_2D, tile_data_render.texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tile_data_render.width, tile_data_render.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface_pixels);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glDisable(GL_TEXTURE_2D);
}

PPU::render_t &PPU::get_tile_data_render() {
    return tile_data_render;
}

PPU::render_t &PPU::get_screen_render() {
    return screen_render;
}
