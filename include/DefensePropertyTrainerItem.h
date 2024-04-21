#pragma once
#include "ITrainerItem.h"

class DefensePropertyTrainerItem : public ITrainerItem {
	uint64_t codeInjectAddr = 0x1DD7689;
	bool checkboxValue = false;
	bool active = false;
	std::vector<BYTE> oldCode = { 0xF3, 0x0F, 0x11, 0x48, 0x58 };
	std::vector<BYTE> newCode = { 0x90, 0x90, 0x90, 0x90, 0x90 };
	float inputAttackProperty = 9999;
public:
	void loop() {
		ImGui::Checkbox(u8"·ÀÓùÁ¦ÐÞ¸Ä", &checkboxValue);
		if (checkboxValue && !active) {
			active = true;
			codeInject();
		}
		if (!checkboxValue && active) {
			active = false;
			codeRestore();
		}
		if (checkboxValue) {
			pm->writeMemory(0xF6F2E58, { 0x3D0, 0x20, 0x58 }, &inputAttackProperty, 4);
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(150);
		ImGui::InputFloat("##defense_property_input", &inputAttackProperty);
		ImGui::PopItemWidth();
	};

	DefensePropertyTrainerItem(std::shared_ptr<ProcessManager> pm) : ITrainerItem(pm) {};
	~DefensePropertyTrainerItem() {
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
