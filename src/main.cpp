#include "bus.h"
#include "cpu/cpu.h"
#include "gui/gui.h"

int main(int argc, char *argv[]) {
    Bus bus;
    CPU cpu(bus);
    GUI gui(cpu);

    while (!gui.get_should_close()) {
        gui.display();
    }

    return 0;
}