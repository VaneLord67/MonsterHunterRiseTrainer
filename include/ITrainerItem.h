#pragma once
#include "gui.h"
#include "ProcessManager.h"
#include <stdint.h>

class ITrainerItem {
public:
	std::shared_ptr<ProcessManager> pm;
	ITrainerItem(std::shared_ptr<ProcessManager> pm) : pm(pm) {};
	virtual void loop() = 0;
};