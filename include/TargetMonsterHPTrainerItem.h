#pragma once
#include "ITrainerItem.h"

class TargetMonsterHPTrainerItem : public ITrainerItem {
private:
    uint64_t targetMonsterIndexAddr = 0xF6E9348;
    std::vector<int64_t> targetMonsterIndexOffsets = { 0x410, 0xDC };
    uint64_t valueAddr = 0xF6ED1B8;
    std::vector<int64_t> offsets = { 0x80, 0x10, 0x20, 0x308, 0x48, 0x10, 0x20, 0x10, 0x20, 0x10, 0x18, 0x18 };
    float input = 1;

public:
    TargetMonsterHPTrainerItem(std::shared_ptr<ProcessManager> pm) : ITrainerItem(pm) {};

    void loop() {
        bool pressButton = ImGui::SmallButton(u8"锁定怪物一击必杀（部分怪会转阶段，需要多次使用该功能）##target_monster_hp_button");
        ImGui::SameLine();

        uint32_t targetMonsterIndex = UINT32_MAX;
        pm->readMemory(targetMonsterIndexAddr, targetMonsterIndexOffsets, &targetMonsterIndex, 4);
        if (targetMonsterIndex <= 5) {
            offsets.at(2) = 0x20 + (int64_t)targetMonsterIndex * 8;
        }
        float targetMonsterHP = 0.0f;
        pm->readMemory(valueAddr, offsets, &targetMonsterHP, 4);
        ImGui::Text(u8"锁定怪物血量: %.0f", targetMonsterHP);

        if (pressButton) {
            if (targetMonsterIndex <= 5) {
                pm->writeMemory(valueAddr, offsets, &input, 4);
            }
        }
    };
};
