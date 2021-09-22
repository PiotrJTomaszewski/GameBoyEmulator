#include <chrono>
#include <thread>
#include <iostream>
#include "bus.h"
#include "cpu/cpu.h"
#include "gui.h"
#include "cartridge/cartridge.h"
#include "io/io.h"

Bus bus;
CPU cpu(bus);
GUI gui(cpu, bus);

int main(int argc, char *argv[]) {
    // bus.insert_cartridge(new Cartridge("/home/pjtom/Documents/GameBoyEmulatorCpp/roms/helloworld/dmg/picture.gb"));
    // TODO: Maybe use precalculated cycles in step
    const long step_duration_micros = 16666; // 60 Hz
    const long cpu_cycles_in_one_step = step_duration_micros * (cpu.get_clock_speed_Hz() / 1000000);
    long cycles_left_in_step = cpu_cycles_in_one_step;
    int cpu_cycles;

    while (!gui.get_should_close()) {
        if (bus.get_is_cart_inserted()) { // TODO: Add CPU execution controller in GUI
            auto start = std::chrono::high_resolution_clock::now();
            while (cycles_left_in_step > 0) {
                cpu_cycles = cpu.exec_next_instr();
                bus.io.timer.tick(cpu_cycles);
                // ppu.tmp_tick(cpu_cycles);
                cycles_left_in_step -= cpu_cycles;
            }
            cycles_left_in_step = cpu_cycles_in_one_step;
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = stop - start;
            if (duration < std::chrono::microseconds(step_duration_micros)) {
                std::this_thread::sleep_for(std::chrono::microseconds(step_duration_micros) - duration);
            }
        }
        // ppu.render_tile_data();
        // ppu.render_screen();
        gui.display();
    }

    return 0;
}