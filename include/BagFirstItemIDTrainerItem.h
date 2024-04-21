#pragma once
#include "ITrainerItem.h"

class BagFirstItemIDTrainerItem : public ITrainerItem {
private:
    uint64_t valueAddr = 0xF6F31A8;
    std::vector<int64_t> offsets = { 0x20, 0x10 };
    int input = 68157446; // �ظ�ҩ��ID

public:
    BagFirstItemIDTrainerItem(std::shared_ptr<ProcessManager> pm) : ITrainerItem(pm) {};

    void loop() {
        bool pressButton = ImGui::SmallButton(u8"Ӧ��##bag_first_item_id");
        ImGui::SameLine();
        ImGui::Text(u8"���ߴ�����ƷID������Ʒ�������·�һ������Ʒ����Ӧ�ã�");
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
