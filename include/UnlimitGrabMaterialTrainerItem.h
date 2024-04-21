#pragma once
#include "ITrainerItem.h"

class UnlimitGrabMaterialTrainerItem : public ITrainerItem {
	uint64_t codeInjectAddr = 0x12305EE;
	bool checkboxValue = false;
	bool active = false;
	std::vector<BYTE> oldCode = { 0x41, 0xFF, 0x86, 0x94, 0x00, 0x00, 0x00 };
	std::vector<BYTE> newCode = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
public:
	void loop() {
		ImGui::Checkbox(u8"无限挖取素材（怪物尸体仍然会消失）", &checkboxValue);
		if (checkboxValue && !active) {
			active = true;
			codeInject();
		}
		if (!checkboxValue && active) {
			active = false;
			codeRestore();
		}
	};

	UnlimitGrabMaterialTrainerItem(std::shared_ptr<ProcessManager> pm) : ITrainerItem(pm) {};
	~UnlimitGrabMaterialTrainerItem() {
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
