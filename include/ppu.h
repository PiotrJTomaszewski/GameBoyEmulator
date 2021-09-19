#pragma once
#include <cstdint>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "io/io.h"
#include "bus.h"

class PPU {
public:
    struct render_t {
        SDL_Surface *surface;
        GLuint texture;
        int width;
        int height;
    };
    PPU(IO &io, Bus &bus);
    ~PPU();
    void restart();
    void tmp_tick(unsigned cpu_clocks);
    void render_screen();
    void render_tile_data();
    void render_current_screen_line();
    render_t &get_tile_data_render();
    render_t &get_screen_render();

private:
    enum lcd_en_dis_t {
        DISABLE = 0,
        ENABLE = 1
    };

    enum map_area_t {
        AREA_9800 = 0,
        AREA_9C00 = 1
    };

    enum data_area_t {
        AREA_8800 = 0,
        AREA_8000 = 1
    };

    enum OBJ_size_t {
        SIZE_8x8 = 0,
        SIZE_16x16 = 1
    };

    enum diff_eq_t {
        DIFFERENT = 0,
        EQUAL = 1
    };

    enum mode_flag_t {
        IN_HBLANK = 0,
        IN_VBLANK = 1,
        SEARCHING_OAM = 2,
        RENDERING = 3
    };

        struct __attribute__((packed)) LLCDC_t {
        lcd_en_dis_t BG_and_window_enable: 1; // 0 - Off, 1 - On
        lcd_en_dis_t OBJ_enable: 1; // 0 - Off, 1 - On
        OBJ_size_t OBJ_size: 1; // 0 - 8x8, 1 - 8x16
        map_area_t BG_tile_map_area: 1; // 0 - 9800-9BFF, 1 - 9C00-9FFF
        data_area_t BG_and_window_tile_data_area: 1; // 0 - 8800-97FF, 1 - 8000-8FFF
        lcd_en_dis_t window_enable: 1; // 0 - Off, 1 - On
        map_area_t window_tile_map_area: 1; // 0 - 9800-9BFF, 1 - 9C00-9FFF
        lcd_en_dis_t LCD_and_PPU_enable: 1; // 0 - Off, 1 - On
    };

    struct __attribute__((packed)) STAT_t {
        mode_flag_t mode_flag: 2; // 0 - In HBlank, 1 - In VBlank, 2 - Searching OAM, 3 - Transferring data to LCD controller
        diff_eq_t LYC_eq_LY: 1; // 0 - different, 1 - equal
        lcd_en_dis_t hblank_STAT_intr_src: 1; // 0 - disable, 1 - enable
        lcd_en_dis_t vblank_STAT_intr_src: 1; // 0 - disable, 1 - enable
        lcd_en_dis_t OAM_STAT_intr_src: 1; // 0 - disable, 1 - enable
        lcd_en_dis_t LYC_eq_LY_STAT_intr_src: 1; // 0 - disable, 1 - enable
        int _unused: 1;
    };

    enum color_t {
        WHITE = 0,
        LIGHT_GRAY = 1,
        DARK_GRAY = 2,
        BLACK = 3
    };

    union __attribute__((packed)) palette_data_t {
        uint8_t value;
        struct INNER {
            color_t index0: 2;
            color_t index1: 2;
            color_t index2: 2;
            color_t index3: 2;
        } colors;
    };

    struct __attribute__((packed)) LCD_data_t {
        LLCDC_t LCD_control; // 0xFF40
        STAT_t LCD_status; // 0xFF41
        uint8_t SCY; // Scroll Y - 0xFF42
        uint8_t SCX; // Scroll X - 0xFF43
        uint8_t LY; // LCD Y Coordinate - 0xFF44
        uint8_t LYC; // LY Compare - 0xFF45
        uint8_t _nothing; // ? - 0xFF46
        palette_data_t BGP; // BG Palette data - 0xFF47
        palette_data_t OBP0; // Object palette 0 data - 0xFF48
        palette_data_t OBP1; // Object palette 1 data - 0xFF49
        uint8_t WY; // Window Y Position - 0xFF4A
        uint8_t WX; // Window X Position + 7 - 0xFF4B
    };

    IO &io;
    Bus &bus;
    LCD_data_t *LCD_data;
    render_t tile_data_render, screen_render;
    const int TILE_DATA_TILES_IN_ROW = 16;
    int dots_in_current_mode;

    inline void enter_mode_searching_OAM();
    inline void enter_mode_rendering();
    inline void enter_mode_hblank();
    inline void enter_mode_vblank();
};
