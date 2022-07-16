#include "gui_logger.h"
#include <fstream>
#include "imgui.h"
#include "disassembler.h"

void GuiLogger::log(std::string message) {
    messages.push_back(message);
}

void GuiLogger::log_instruction(instruction_t const& instruction) {
    char buffer[50];
    memset(buffer, 0, 50);
    Disassembler::disassemble_instr(instruction, buffer);
    messages.push_back(buffer);
}

void GuiLogger::display() const {
    ImGui::Begin("Log", NULL);
    if (ImGui::Button("Save to file")) {
        std::fstream fs;
        fs.open("log.txt", std::ios_base::out);
        for (auto const &msg: messages) {
            fs << msg << std::endl;
        }
        fs.close();
    }
    ImGui::BeginChild("");
    for (auto const &msg: messages) {
        ImGui::Text(msg.c_str());
    }
    ImGui::EndChild();
    ImGui::End();
}
