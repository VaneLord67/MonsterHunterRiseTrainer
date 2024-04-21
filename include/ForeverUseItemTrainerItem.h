#pragma once
#include "ITrainerItem.h"

class ForeverUseItemTrainerItem : public ITrainerItem {
	uint64_t codeInjectAddr = 0x335BB8A;
	bool checkboxValue = false;
	bool active = false;
	std::vector<BYTE> oldCode = { 0x29, 0x72, 0x14 };
	std::vector<BYTE> newCode = { 0x90, 0x90, 0x90 };
public:
	void loop() {
		ImGui::Checkbox(u8"无限次使用道具", &checkboxValue);
		if (checkboxValue && !active) {
			active = true;
			codeInject();
		}
		if (!checkboxValue && active) {
			active = false;
			codeRestore();
		}
	};

	ForeverUseItemTrainerItem(std::shared_ptr<ProcessManager> pm) : ITrainerItem(pm) {};
	~ForeverUseItemTrainerItem() {
		if (active) {
			this->codeRestore();
		}
	}

	bool codeInject() {
		return pm->codeSimpleInject(this->codeInjectAddr, this->oldCode.data(), this->newCode.data(), this->oldCode.size());
	};
	bool codeRestore() {
		return pm->codeSimpleRestore(this->codeInjectAddr, this->oldCode.data(), this->oldCode.size());
	};
};
