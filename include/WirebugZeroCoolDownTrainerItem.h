#pragma once
#include "ITrainerItem.h"

class WirebugZeroCoolDownTrainerItem : public ITrainerItem {
	uint64_t codeInjectAddr = 0x1F5842F;

	// 在狂龙症状态下，只nop上面的指令不能做到无CD，还要把下面四条指令nop掉，分别对应四只翔虫
	uint64_t codeInjectAddr1 = 0x1F5826F;
	uint64_t codeInjectAddr2 = 0x1F582D8;
	uint64_t codeInjectAddr3 = 0x1F58341;
	uint64_t codeInjectAddr4 = 0x1F583AB;
	bool checkboxValue = false;
	bool active = false;
	std::vector<BYTE> oldCode = { 0xF3, 0x0F, 0x11, 0x49, 0x10 };
	std::vector<BYTE> newCode = { 0x90, 0x90, 0x90, 0x90, 0x90 };
public:
	void loop() {
		ImGui::Checkbox(u8"翔虫无CD", &checkboxValue);
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
		pm->codeSimpleInject(this->codeInjectAddr1, this->oldCode.data(), this->newCode.data(), this->oldCode.size());
		pm->codeSimpleInject(this->codeInjectAddr2, this->oldCode.data(), this->newCode.data(), this->oldCode.size());
		pm->codeSimpleInject(this->codeInjectAddr3, this->oldCode.data(), this->newCode.data(), this->oldCode.size());
		pm->codeSimpleInject(this->codeInjectAddr4, this->oldCode.data(), this->newCode.data(), this->oldCode.size());
		return pm->codeSimpleInject(this->codeInjectAddr, this->oldCode.data(), this->newCode.data(), this->oldCode.size());
	};
	bool codeRestore() {
		pm->codeSimpleRestore(this->codeInjectAddr1, this->oldCode.data(), this->oldCode.size());
		pm->codeSimpleRestore(this->codeInjectAddr2, this->oldCode.data(), this->oldCode.size());
		pm->codeSimpleRestore(this->codeInjectAddr3, this->oldCode.data(), this->oldCode.size());
		pm->codeSimpleRestore(this->codeInjectAddr4, this->oldCode.data(), this->oldCode.size());
		return pm->codeSimpleRestore(this->codeInjectAddr, this->oldCode.data(), this->oldCode.size());
	};
};
