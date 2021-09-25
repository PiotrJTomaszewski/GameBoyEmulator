#pragma once
#include <cstdint>
#include "ppu/ppu_types.h"
#include "bus.h"

class PPU {
public:
    PPU(Bus &bus);
    ~PPU();
    void attach_interrupts(Interrupts *interrupts);
    void restart();
    void tick(unsigned cpu_clocks);
    void render_current_screen_line();
    uint32_t *get_screen_pixels();

private:
    // TODO: Set to the real values once scrolling is implmented
    const static unsigned SCREEN_WIDTH = 256; 
    const static unsigned SCREEN_HEIGHT = 256;
    const static unsigned VRAM_SIZE = 0x2000;

    Bus &bus;
    LCD_data_t *LCD_data;
    unsigned dots_in_current_mode;
    // Each pixel is represented as 32 bit number (RGBA). This should be easily converted to OpenGL texture
    uint32_t screen_pixels[SCREEN_WIDTH * SCREEN_HEIGHT];

private:
    inline void enter_mode_searching_OAM();
    inline void enter_mode_rendering();
    inline void enter_mode_hblank();
    inline void enter_mode_vblank();
    inline void increment_LY();
};
