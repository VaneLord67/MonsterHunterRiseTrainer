#pragma once
#include "ITrainerItem.h"

class ZeroHourglassTrainerItem : public ITrainerItem {
	uint64_t valueAddr = 0xF6EC0F0;
	std::vector<int64_t> offsets = { 0x1B4 };
	float zeroHourglassValue = 0;
public:
	void loop() {
		bool pressButton = ImGui::SmallButton(u8"清空任务返回倒计时");
		if (pressButton) {
			pm->writeMemory(valueAddr, offsets, &zeroHourglassValue, 4);
		}
	};

	ZeroHourglassTrainerItem(std::shared_ptr<ProcessManager> pm) : ITrainerItem(pm) {};
	~ZeroHourglassTrainerItem() {}

};
