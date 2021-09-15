#include "bus.h"
#include "cpu/cpu.h"
#include "gui/gui.h"
#include "cartridge/cartridge.h"

int main(int argc, char *argv[]) {
    Bus bus;
    CPU cpu(bus);
    GUI gui(cpu, bus);
    bool is_cart = bus.get_is_cart_inserted();

    while (!gui.get_should_close()) {
        if (bus.get_is_cart_inserted()) {
            cpu.next_cycle();
        }
        gui.display();
    }

    return 0;
}