#pragma once
#include "ITrainerItem.h"

class AttackPropertyTrainerItem : public ITrainerItem {
	uint64_t codeInjectAddr = 0x1DD5D98;
	bool checkboxValue = false;
	bool active = false;
	LPVOID virAddr = 0;
	std::vector<BYTE> oldCode = {};
	std::vector<BYTE> shellCode = { 0x48, 0x83, 0xC4, 0x40, 0x41, 0x5E, 0x5F, 0x5D, 0xC3 };
	float inputAttackProperty = 9999;
public:
	void loop() {
		ImGui::Checkbox(u8"¹¥»÷Á¦ÐÞ¸Ä", &checkboxValue);
		if (checkboxValue && !active) {
			active = true;
			codeInject();
		}
		if (!checkboxValue && active) {
			active = false;
			codeRestore();
		}
		if (checkboxValue) {
			pm->writeMemory(0xF6F2E58, { 0x3D0, 0x20, 0x44 }, &inputAttackProperty, 4);
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(150);
		ImGui::InputFloat("##attack_property_input", &inputAttackProperty);
		ImGui::PopItemWidth();
	};

	AttackPropertyTrainerItem(std::shared_ptr<ProcessManager> pm) : ITrainerItem(pm) {};
	~AttackPropertyTrainerItem() {
		if (active) {
			this->codeRestore();
		}
	}

	bool codeInject() {
		oldCode.clear();
		oldCode.resize(14);
		return pm->codeInject(codeInjectAddr, oldCode.data(), 14, shellCode, virAddr);
	};
	bool codeRestore() {
		return pm->codeRestore(codeInjectAddr, oldCode.data(), oldCode.size(), virAddr);
	};
};
