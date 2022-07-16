#include "gui_logger.h"
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
    // ImGui::BeginChild("");
    for (auto const &msg: messages) {
        ImGui::Text(msg.c_str());
    }
    // ImGui::EndChild();
    ImGui::End();
}
