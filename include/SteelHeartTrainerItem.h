#pragma once
#include "ITrainerItem.h"

class SteelHeartTrainerItem : public ITrainerItem {
private:
	uint64_t codeInjectAddr = 0x2061044;
	uint64_t nopCodeAddr = 0x20717E5;

	bool checkboxValue = false;
	bool active = false;
	LPVOID virAddr = 0;
	std::vector<BYTE> nopOldCode = { 0x44, 0x89, 0xB8, 0xB0, 0x01, 0x00, 0x00 };
	std::vector<BYTE> nopCode = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };

	std::vector<BYTE> oldCode = {};
	std::vector<BYTE> shellCode = {
		// 这里xmm8固定为10000,因此借用来赋值
		// 刚心为float类型，范围为0-400
		0xF3, 0x44, 0x0F, 0x11, 0x80, 0xB0, 0x01, 0x00, 0x00, // movss [rax+1B0],xmm8
		0x48, 0x8B, 0x86, 0x20, 0x01, 0x00, 0x00, // mov rax,[rsi+120]
	};

public:

	void loop() {
		ImGui::Checkbox(u8"无限刚心（需装备刚心技能）", &checkboxValue);
		if (checkboxValue && !active) {
			active = true;
			codeInject();
		}
		if (!checkboxValue && active) {
			active = false;
			codeRestore();
		}
	};

	SteelHeartTrainerItem(std::shared_ptr<ProcessManager> pm) : ITrainerItem(pm) {};
	~SteelHeartTrainerItem() {
		if (active) {
			this->codeRestore();
		}
	}

	bool codeInject() {
		oldCode.clear();
		oldCode.resize(15);
		pm->codeSimpleInject(nopCodeAddr, nopOldCode.data(), nopCode.data(), nopCode.size());
		return pm->codeInject(codeInjectAddr, oldCode.data(), oldCode.size(), shellCode, virAddr);
	};
	bool codeRestore() {
		pm->codeSimpleRestore(nopCodeAddr, nopOldCode.data(), nopOldCode.size());
		return pm->codeRestore(codeInjectAddr, oldCode.data(), oldCode.size(), virAddr);
	};
};
