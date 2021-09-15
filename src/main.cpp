#include "bus.h"
#include "cpu/cpu.h"
#include "gui/gui.h"
#include "cartridge/cartridge.h"

int main(int argc, char *argv[]) {
    Bus bus;
    CPU cpu(bus);
    GUI gui(cpu, bus);
    bus.insert_cartridge(new Cartridge("/home/pjtom/Documents/GameboyEmulator/roms/test_roms/blargg/cpu_instrs/individual/03-op sp,hl.gb"));
    bool is_cart = bus.get_is_cart_inserted();

    while (!gui.get_should_close()) {
        if (bus.get_is_cart_inserted()) {
            for (int cycles = 0; cycles < 1000;) {
                cycles += cpu.next_cycle();
            }
        }
        gui.display();
    }

    return 0;
}