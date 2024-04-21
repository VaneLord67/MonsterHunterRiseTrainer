#pragma once
#include "ITrainerItem.h"

class ForeverEnvWirebugTrainerItem : public ITrainerItem {
	uint64_t codeInjectAddr = 0x2060346;
	bool checkboxValue = false;
	bool active = false;
	std::vector<BYTE> oldCode = { 0x41, 0xB8, 0xFF, 0xFF, 0xFF, 0xFF };
	std::vector<BYTE> newCode = { 0x41, 0xB8, 0x00, 0x00, 0x00, 0x00 };
public:
	void loop() {
		ImGui::Checkbox(u8"环境翔虫不消失", &checkboxValue);
		if (checkboxValue && !active) {
			active = true;
			codeInject();
		}
		if (!checkboxValue && active) {
			active = false;
			codeRestore();
		}
	};

	ForeverEnvWirebugTrainerItem(std::shared_ptr<ProcessManager> pm) : ITrainerItem(pm) {};
	~ForeverEnvWirebugTrainerItem() {
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
