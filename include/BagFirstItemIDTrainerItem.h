#pragma once
#include "ITrainerItem.h"

class BagFirstItemIDTrainerItem : public ITrainerItem {
private:
    uint64_t valueAddr = 0xF6F31A8;
    std::vector<int64_t> offsets = { 0x20, 0x10 };
    int input = 68157446; // 回复药的ID

public:
    BagFirstItemIDTrainerItem(std::shared_ptr<ProcessManager> pm) : ITrainerItem(pm) {};

    void loop() {
        bool pressButton = ImGui::SmallButton(u8"应用##bag_first_item_id");
        ImGui::SameLine();
        ImGui::Text(u8"道具袋首物品ID（在物品箱里重新放一下首物品后再应用）");
        if (pressButton) {
            input = max(input, 0);
            pm->writeMemory(valueAddr, offsets, &input, 4);
        }
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::InputInt("##bag_first_item_id_input", &input, 0, 0);
        ImGui::PopItemWidth();
    };
};
