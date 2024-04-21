#pragma once
#include "ITrainerItem.h"

class HPTrainerItem : public ITrainerItem {
private:
    bool checkboxValue = false;
    int inputHP = 200;

public:
    HPTrainerItem(std::shared_ptr<ProcessManager> pm) : ITrainerItem(pm) {};

	void loop() {
        ImGui::Checkbox(u8"无限体力", &checkboxValue);
        if (checkboxValue) {
            inputHP = min(inputHP, 200);
            inputHP = max(inputHP, 1);
            float floatHP = static_cast<float>(inputHP);
            pm->writeMemory(0xF6F2E58, { 0x3D0, 0x20, 0x10, 0x18, 0x18 }, &floatHP, 4);
            pm->writeMemory(0xF6F2E58, { 0x3D0, 0x20, 0x18 }, &inputHP, 4);
        }
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::InputInt("##hp_input", &inputHP, 0, 0);
        ImGui::PopItemWidth();
	};
};
