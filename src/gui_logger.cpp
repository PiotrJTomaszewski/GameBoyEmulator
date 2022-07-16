#include "gui_logger.h"
#include "imgui.h"

void GuiLogger::log(std::string message) {
    messages.push_back(message);
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
