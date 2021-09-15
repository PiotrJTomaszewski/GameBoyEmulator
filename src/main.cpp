#include <chrono>
#include <thread>
#include "bus.h"
#include "cpu/cpu.h"
#include "gui/gui.h"
#include "cartridge/cartridge.h"
#include "io/io.h"

int main(int argc, char *argv[]) {
    IO io;
    Bus bus(io);
    CPU cpu(bus, io);
    GUI gui(cpu, bus);
    // bus.insert_cartridge(new Cartridge("/home/pjtom/Documents/GameboyEmulator/roms/test_roms/blargg/cpu_instrs/individual/03-op sp,hl.gb"));
    bool is_cart = bus.get_is_cart_inserted();
    // TODO: Maybe use precalculated cycles in step
    const long step_duration_micros = 16666; // 60 Hz
    const long cpu_cycles_in_one_step = step_duration_micros * (cpu.get_clock_speed_Hz() / 1000000);

    while (!gui.get_should_close()) {
        if (bus.get_is_cart_inserted()) { // TODO: Add CPU execution controller in GUI
            auto start = std::chrono::high_resolution_clock::now();
            for (int cycles = 0; cycles < cpu_cycles_in_one_step;) {
                cycles += cpu.next_cycle();
            }
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = stop - start;
            if (duration < std::chrono::microseconds(step_duration_micros)) {
                std::this_thread::sleep_for(std::chrono::microseconds(step_duration_micros) - duration);
            }
            // TODO: Remove later
            io.interrupts.signal(VBLANK);
        }
        gui.display();
    }

    return 0;
}