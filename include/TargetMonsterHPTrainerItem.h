#pragma once
#include "ITrainerItem.h"

class TargetMonsterHPTrainerItem : public ITrainerItem {
private:
    std::vector<int64_t> hpOffset = { 0x48, 0x10, 0x20, 0x10, 0x20, 0x10, 0x18, 0x18 };

    uint64_t targetMonsterAddr = 0xF6ED340;
    std::vector<int64_t> targetMonsterAddrOffsets = { 0xB8, 0x58, 0x28, 0x78 };
    float input = 1;
    bool show_error_window = false;

public:
    TargetMonsterHPTrainerItem(std::shared_ptr<ProcessManager> pm) : ITrainerItem(pm) {};

    void loop() {
        bool pressButton = ImGui::SmallButton(u8"��������һ����ɱ�����ֹֻ�ת�׶Σ���Ҫ���ʹ�øù��ܣ�##target_monster_hp_button");
        ImGui::SameLine();

        uint64_t readMonsterAddr = 0;
        float targetMonsterHP = 0.0f;

        pm->readMemory(targetMonsterAddr, targetMonsterAddrOffsets, &readMonsterAddr, 8);
        if (readMonsterAddr != NULL) {
            pm->readMemory(readMonsterAddr + 0x308, hpOffset, &targetMonsterHP, 4, true);
        }

        ImGui::Text(u8"��������Ѫ��: %.0f", targetMonsterHP);

        if (ImGui::BeginPopupModal(u8"����##target_monster_hp", &show_error_window, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text(u8"��ǰδ��������");
            ImGui::EndPopup();
        }

        if (pressButton) {
            if (readMonsterAddr == NULL) {
                show_error_window = true;
                ImGui::OpenPopup(u8"����##target_monster_hp");
            }
            else {
                pm->writeMemory(readMonsterAddr + 0x308, hpOffset, &input, 4, true);
            }
        }
    };
};
