#pragma once
#include "ITrainerItem.h"

class MoneyTrainerItem : public ITrainerItem {
private:
    uint64_t valueAddr = 0xF6F02E0;
    std::vector<int64_t> offsets = { 0x58, 0x18 };
    int input = 99999999;

public:
    MoneyTrainerItem(std::shared_ptr<ProcessManager> pm) : ITrainerItem(pm) {};

    void loop() {
        bool pressButton = ImGui::SmallButton(u8"Ó¦ÓÃ##money_button");
        ImGui::SameLine();
        ImGui::Text(u8"±à¼­½ðÇ®");
        if (pressButton) {
            input = max(input, 0);
            pm->writeMemory(valueAddr, offsets, &input, 4);
        }
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::InputInt("##money_input", &input, 0, 0);
        ImGui::PopItemWidth();
    };
};
