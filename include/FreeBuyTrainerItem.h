#include "ITrainerItem.h"

class FreeBuyTrainerItem : public ITrainerItem {
	uint64_t codeInjectAddr = 0x1DC7CD2;
	bool checkboxValue = false;
	bool active = false;
	LPVOID virAddr = 0;
	std::vector<BYTE> oldCode = { 0x89, 0x42, 0x18 };
	std::vector<BYTE> newCode = { 0x90, 0x90, 0x90 };
public:
	void loop() {
		ImGui::Checkbox(u8"ÁãÔª¹º", &checkboxValue);
		if (checkboxValue && !active) {
			active = true;
			codeInject();
		}
		if (!checkboxValue && active) {
			active = false;
			codeRestore();
		}
	};
	FreeBuyTrainerItem(std::shared_ptr<ProcessManager> pm) : ITrainerItem(pm) {};
	~FreeBuyTrainerItem() {
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
