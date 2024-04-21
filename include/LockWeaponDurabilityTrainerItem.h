#pragma once
#include "ITrainerItem.h"

class LockWeaponDurabilityTrainerItem : public ITrainerItem {
private:
	uint64_t codeInjectAddr = 0x205D45C;

	bool checkboxValue = false;
	bool active = false;
	LPVOID virAddr = 0;
	std::vector<BYTE> oldCode = {};
	std::vector<BYTE> shellCode = {
		/*
			85 FF                 - test edi,edi
			0F88 05000000         - js 7FF7343F000D
			E9 05000000           - jmp 7FF7343F0012
			BF 00000000           - mov edi,00000000 { 0 }
			01 BB EC010000        - add [rbx+000001EC],edi
		*/
		0x85, 0xFF, // test edi, edi
		0x0F, 0x88, 0x05, 0x00, 0x00, 0x00, // jmp to mov edi,0
		0xE9, 0x05, 0x00, 0x00, 0x00, // jmp to add [rbx+1EC],edi
		0xBF, 0x00, 0x00, 0x00, 0x00, // mov edi,0
		0x01, 0xBB, 0xEC, 0x01, 0x00, 0x00, // add [rbx+1EC],edi

		0x8B, 0x83, 0xEC, 0x01, 0x00, 0x00, // mov eax,[rbx+1EC]
		0x41, 0x3B, 0xC6, // cmp eax,r14d
	};

public:

	void loop() {
		ImGui::Checkbox(u8"ø≥π÷≤ªµÙ’∂Œ∂", &checkboxValue);
		if (checkboxValue && !active) {
			active = true;
			codeInject();
		}
		if (!checkboxValue && active) {
			active = false;
			codeRestore();
		}
	};

	LockWeaponDurabilityTrainerItem(std::shared_ptr<ProcessManager> pm) : ITrainerItem(pm) {};
	~LockWeaponDurabilityTrainerItem() {
		if (active) {
			this->codeRestore();
		}
	}

	bool codeInject() {
		oldCode.clear();
		oldCode.resize(15);
		return pm->codeInject(codeInjectAddr, oldCode.data(), oldCode.size(), shellCode, virAddr);
	};
	bool codeRestore() {
		return pm->codeRestore(codeInjectAddr, oldCode.data(), oldCode.size(), virAddr);
	};
};
