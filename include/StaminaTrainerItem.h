#pragma once
#include "ITrainerItem.h"

class StaminaTrainerItem : public ITrainerItem {
private:
    bool checkboxValue = false;
    int inputStamina = 6600;

public:
    StaminaTrainerItem(std::shared_ptr<ProcessManager> pm) : ITrainerItem(pm) {};

    void loop() {
        ImGui::Checkbox(u8"ÎÞÏÞÄÍÁ¦", &checkboxValue);
        if (checkboxValue) {
            inputStamina = min(inputStamina, 6600);
            inputStamina = max(inputStamina, 0);
            float floatStamina = static_cast<float>(inputStamina);
            pm->writeMemory(0xF6F2E58, { 0x3D0, 0x20, 0x28 }, &floatStamina, sizeof(floatStamina));
            pm->writeMemory(0xF6F2E58, { 0x3D0, 0x20, 0x2C }, &floatStamina, sizeof(floatStamina));
        }
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::InputInt("##stamina_input", &inputStamina, 0, 0);
        ImGui::PopItemWidth();
    };
};
