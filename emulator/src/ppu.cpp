#include <iostream>
#include <cstring>
#include "ppu/ppu.h"

PPU::PPU(Bus &bus): bus(bus) {
    LCD_data = (LCD_data_t *)(bus.io.data + 0xFF40 - 0xFF00);
    restart();
}

PPU::~PPU() {

}

void PPU::restart() {
    dots_in_current_mode = 0;
    LCD_data->LCD_control.value = 0x91;
    LCD_data->SCY = 0;
    LCD_data->SCX = 0;
    LCD_data->LYC = 0;
    LCD_data->BGP.value = 0x00;
    LCD_data->OBP0.value = 0xFF;
    LCD_data->OBP1.value = 0xFF;
    LCD_data->WY = 0;
    LCD_data->WX = 0;
}

void PPU::tick(unsigned cpu_clocks) {
    // TODO: Enable / disable VRAM access
    // TODO: Implement a proper cycle to dot conversion
    if (LCD_data->LCD_control.bits.LCD_and_PPU_enabled) {
        dots_in_current_mode += cpu_clocks;

        switch(LCD_data->LCD_status.bits.mode_flag) {
            case mode_flag_t::IN_HBLANK:
                if (dots_in_current_mode >= 85-1) { // TODO: 85 or 84
                    // Line 143 is the last line in a frame. After this PPU enters VBlank
                    if (LCD_data->LY <= 143) {
                        // Begin a new line
                        enter_mode_searching_OAM();
                    } else {
                        // Enter VBlank
                        enter_mode_vblank();
                    }
                    dots_in_current_mode -= 85;
                }
                break;
            case mode_flag_t::IN_VBLANK:
                if (dots_in_current_mode >= 4560-1) {
                    // Begin a new line
                    enter_mode_searching_OAM();
                    dots_in_current_mode -= 4560;
                } else if (LCD_data->LY < 153) { // LY should go from 144 to 153 in this mode
                    increment_LY();
                }
                break;
            case mode_flag_t::SEARCHING_OAM:
                if (dots_in_current_mode >= 80-1) {
                    enter_mode_rendering();
                    dots_in_current_mode -= 80;
                }
                break;
            case mode_flag_t::RENDERING:
                if (dots_in_current_mode >= 291-1) {
                    enter_mode_hblank();
                    dots_in_current_mode -= 291;
                }
                break;
        }
    }
}

inline void PPU::enter_mode_searching_OAM() {
    LCD_data->LCD_status.bits.mode_flag = mode_flag_t::SEARCHING_OAM;
    increment_LY();
    if (LCD_data->LCD_status.bits.OAM_STAT_intr_src_enabled) {
        bus.io.interrupts.signal(intr_type_t::LCD_STAT);
    }
}

inline void PPU::enter_mode_rendering() {
    LCD_data->LCD_status.bits.mode_flag = mode_flag_t::RENDERING;
    render_current_screen_line();
}

inline void PPU::enter_mode_hblank() {
    LCD_data->LCD_status.bits.mode_flag = mode_flag_t::IN_HBLANK;
    if (LCD_data->LCD_status.bits.hblank_STAT_intr_src_enabled) {
        bus.io.interrupts.signal(intr_type_t::LCD_STAT);
    }
}

inline void PPU::enter_mode_vblank() {
    LCD_data->LCD_status.bits.mode_flag = mode_flag_t::IN_VBLANK;
    increment_LY();
    bus.io.interrupts.signal(intr_type_t::VBLANK);
    if (LCD_data->LCD_status.bits.vblank_STAT_intr_src_enabled) {
        bus.io.interrupts.signal(intr_type_t::LCD_STAT);
    }
}

inline void PPU::increment_LY() {
    LCD_data->LY = (LCD_data->LY  + 1) % 154;
    if ((LCD_data->LY == LCD_data->LYC) && (LCD_data->LCD_status.bits.LYC_eq_LY_STAT_intr_src_enabled)) {
        bus.io.interrupts.signal(intr_type_t::LCD_STAT);
    }
}

// TODO: Still not perfect as I'm yet to implement Pixel FIFO
void PPU::render_current_screen_line() {
    int tile_row = LCD_data->LY / 8;
    int tile_line_no = LCD_data->LY % 8;

    int high_byte;
    int low_byte;
    int color;
    int col, row;
    uint8_t *tile_map;
    int tile_addr;
    int8_t signed_offset;
    uint8_t *vram_data = bus.vram.get_raw_data();

    uint32_t color_palette[] = {0xFF000000, 0xFF555555, 0xFFAAAAAA, 0xFFFFFFFF}; // TODO: Use the palette from register

    if (LCD_data->LCD_control.bits.BG_tile_map_area == map_area_t::AREA_9800) {
       tile_map = vram_data + 0x1800; // 0x9800 - 0x8000 (VRAM starts at 0x8000 in memory)
    } else {
        tile_map = vram_data + 0x1C00; // 0x9C00 - 0x8000
    }

    for (int tile_col = 0; tile_col < 32; ++tile_col) {
        if (LCD_data->LCD_control.bits.BG_and_window_tile_data_area == data_area_t::AREA_8000) {
            tile_addr = 16 * tile_map[32 * tile_row + tile_col]; // implicit 0x8000 - 0x8000 (VRAM starts at 0x8000 in memory)
        } else {
            // Offset should be interpreted as a signed number
            memcpy(&signed_offset, &tile_map[32 * tile_row + tile_col], 1);
            // This area of tile data starts at 0x800 (from the beginning of VRAM)
            // The first tile has index = -128
            // So the address of tile with index 0 would be 0x800 + 128 * (tile size=16) = 0x800 + 0x800 = 0x1000
            tile_addr =  0x1000 + 16 * signed_offset;
        }

        low_byte = vram_data[tile_addr + 2 * tile_line_no];
        high_byte = vram_data[tile_addr + 2 * tile_line_no + 1];
        for (int bit_no = 7; bit_no >= 0; --bit_no) {
            color = ((high_byte >> bit_no) & 1) << 1 | ((low_byte >> bit_no) & 1);
            row = 8 * tile_row + tile_line_no;
            col = 8 * tile_col + (7 - bit_no);
            screen_pixels[(row * SCREEN_WIDTH) + col] = color_palette[color];
        }
    }
}

uint32_t *PPU::get_screen_pixels() {
    return screen_pixels;
}
