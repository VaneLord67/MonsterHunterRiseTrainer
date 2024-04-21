#pragma once
#include "ITrainerItem.h"

class LongSwordEnergyTrainerItem : public ITrainerItem {
private:
    uint64_t valueAddr = 0xF6EEA18;
    std::vector<int64_t> offsets = { 0xE0, 0x9F4 };
    bool checkboxValue = false;
    float input = 100.0f;

public:
    LongSwordEnergyTrainerItem(std::shared_ptr<ProcessManager> pm) : ITrainerItem(pm) {};

    void loop() {
        ImGui::Checkbox(u8"Ì«µ¶ÆøÈÐ²Û", &checkboxValue);
        if (checkboxValue) {
            input = min(input, 100.0f);
            input = max(input, 0.0f);
            pm->writeMemory(valueAddr, offsets, &input, 4);
        }
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::InputFloat("##long_sword_energy_input", &input);
        ImGui::PopItemWidth();
    };
};
