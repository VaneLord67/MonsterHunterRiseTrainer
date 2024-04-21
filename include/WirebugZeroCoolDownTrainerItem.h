#pragma once
#include "ITrainerItem.h"

class WirebugZeroCoolDownTrainerItem : public ITrainerItem {
	uint64_t codeInjectAddr = 0x1F5842F;
	bool checkboxValue = false;
	bool active = false;
	std::vector<BYTE> oldCode = { 0xF3, 0x0F, 0x11, 0x49, 0x10 };
	std::vector<BYTE> newCode = { 0x90, 0x90, 0x90, 0x90, 0x90 };
public:
	void loop() {
		ImGui::Checkbox(u8"Ïè³æÎÞCD", &checkboxValue);
		if (checkboxValue && !active) {
			active = true;
			codeInject();
		}
		if (!checkboxValue && active) {
			active = false;
			codeRestore();
		}
	};

	WirebugZeroCoolDownTrainerItem(std::shared_ptr<ProcessManager> pm) : ITrainerItem(pm) {};
	~WirebugZeroCoolDownTrainerItem() {
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
