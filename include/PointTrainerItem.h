#pragma once
#include "ITrainerItem.h"

class PointTrainerItem : public ITrainerItem {
private:
    uint64_t valueAddr = 0xF6F02E0;
    std::vector<int64_t> offsets = { 0x60, 0x10 };
    int input = 99999999;

public:
    PointTrainerItem(std::shared_ptr<ProcessManager> pm) : ITrainerItem(pm) {};

    void loop() {
        bool pressButton = ImGui::SmallButton(u8"应用##point_button");
        ImGui::SameLine();
        ImGui::Text(u8"编辑点数");
        if (pressButton) {
            input = max(input, 0);
            pm->writeMemory(valueAddr, offsets, &input, 4);
        }
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::InputInt("##point_input", &input, 0, 0);
        ImGui::PopItemWidth();
    };
};
