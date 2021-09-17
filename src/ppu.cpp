#include "ppu.h"

PPU::PPU(IO &io): io{io} {
    LCD_data = (LCD_data_t *)(io.data + 0xFF40 - 0xFF00);
    // TODO: Remove
    LCD_data->LY = 0x90;
}

PPU::~PPU() {

}

void PPU::tmp_tick() {
    // TODO: Remove
    // LCD_data->LY = (LCD_data->LY + 1) % 154;
}
