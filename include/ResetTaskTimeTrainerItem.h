#pragma once
#include "ITrainerItem.h"

class ResetTaskTimeTrainerItem : public ITrainerItem {
	uint64_t taskCurrentTimeAddr = 0xF6EC0F0;
	uint64_t taskTotalTimeAddr = 0xF6EC0F0;
	std::vector<int64_t> taskCurrentTimeOffsets = { 0x16C };
	std::vector<int64_t> taskTotalTimeOffsets = { 0x178 };
public:
	void loop() {
		bool pressButton = ImGui::SmallButton(u8"重置任务时间");
		if (pressButton) {
			float taskTotalTime = 0;
			pm->readMemory(taskTotalTimeAddr, taskTotalTimeOffsets, &taskTotalTime, 4);
			pm->writeMemory(taskCurrentTimeAddr, taskCurrentTimeOffsets, &taskTotalTime, 4);
		}
	};

	ResetTaskTimeTrainerItem(std::shared_ptr<ProcessManager> pm) : ITrainerItem(pm) {};
	~ResetTaskTimeTrainerItem() {}

};
