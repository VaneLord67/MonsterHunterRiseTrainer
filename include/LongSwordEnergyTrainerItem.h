#pragma once
#include "ITrainerItem.h"

class LongSwordEnergyTrainerItem : public ITrainerItem {
private:
    uint64_t codeInjectAddr1 = 0x3DEC4E7; // 气刃槽非空时的代码
    uint64_t codeInjectAddr2 = 0x3DEC4F6; // 气刃槽空时写0的代码
    bool checkboxValue = false;
    bool active = false;
    LPVOID virAddr = 0;
    std::vector<BYTE> oldCode1 = {};
    std::vector<BYTE> shellCode1 = {};
    std::vector<BYTE> oldCode2 = { 0xC7, 0x83, 0xF4, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    std::vector<BYTE> newCode = { 0xC7, 0x83, 0xF4, 0x09, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x42 };

public:
    void loop() {
        ImGui::Checkbox(u8"太刀满气刃槽", &checkboxValue);
        if (checkboxValue && !active) {
            active = true;
            codeInject();
        }
        if (!checkboxValue && active) {
            active = false;
            codeRestore();
        }
    };

    LongSwordEnergyTrainerItem(std::shared_ptr<ProcessManager> pm) : ITrainerItem(pm) {
        // 这段代码注入更简单的实现方式是movss [rbx+9F4],xmm0 (代码运行到这里时xmm0固定为100)
        shellCode1 = {
            0xC7, 0x83, 0xF4, 0x09, 0x00, 0x00, 0x00, 0x00, 0xC8, 0x42, // mov [rbx+9F4], 42C80000 (write float 100)
            0x0F, 0x82, 0x17, 0x00, 0x00, 0x00, // jb
            0x0F, 0x2F, 0xD7, // comiss xmm2,xmm7
            0x0F, 0x83, 0x1C, 0x00, 0x00, 0x00, // jae
        };
        pm->jmpFar(shellCode1, 0x3DEC4F6); // jmp
        pm->jmpFar(shellCode1, 0x3DEC510); // jb far jmp
        pm->jmpFar(shellCode1, 0x3DEC51A); // jae far jmp
    };
    ~LongSwordEnergyTrainerItem() {
        if (active) {
            this->codeRestore();
        }
    }

    bool codeInject() {
        oldCode1.clear();
        oldCode1.resize(15);
        pm->codeSimpleInject(codeInjectAddr2, oldCode2.data(), newCode.data(), oldCode2.size());
        return pm->codeInject(codeInjectAddr1, oldCode1.data(), oldCode1.size(), shellCode1, virAddr);
    };
    bool codeRestore() {
        pm->codeSimpleRestore(codeInjectAddr2, oldCode2.data(), oldCode2.size());
        return pm->codeRestore(codeInjectAddr1, oldCode1.data(), oldCode1.size(), virAddr);
    };
};
