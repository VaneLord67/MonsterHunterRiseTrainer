#pragma once
#include "ITrainerItem.h"

class LongSwordLevelTrainerItem : public ITrainerItem {
private:
    uint64_t codeInjectAddr = 0x3DEC555;
    bool checkboxValue = false;
    bool active = false;
    LPVOID virAddr = 0;
    std::vector<BYTE> oldCode = {};
    std::vector<BYTE> shellCode = { 
		0xC7, 0x82, 0xF8, 0x09, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, // move [rdx+9F8], 3
		0x8B, 0x82, 0xF8, 0x09, 0x00, 0x00, // mov eax,[rdx+9F8]
		0x83, 0xF8, 0x03, // cmp eax,03
	};

public:

    void loop() {
        ImGui::Checkbox(u8"Ì«µ¶ËøºìÈÐ", &checkboxValue);
		if (checkboxValue && !active) {
			active = true;
			codeInject();
		}
		if (!checkboxValue && active) {
			active = false;
			codeRestore();
		}
    };

	LongSwordLevelTrainerItem(std::shared_ptr<ProcessManager> pm) : ITrainerItem(pm) {};
	~LongSwordLevelTrainerItem() {
		if (active) {
			this->codeRestore();
		}
	}

	bool codeInject() {
		oldCode.clear();
		oldCode.resize(16);
		return pm->codeInject(codeInjectAddr, oldCode.data(), oldCode.size(), shellCode, virAddr);
	};
	bool codeRestore() {
		return pm->codeRestore(codeInjectAddr, oldCode.data(), oldCode.size(), virAddr);
	};
};
