#pragma once
#include <stdint.h>

enum class TrainerItemType {
	CODE_INJECT,
	CODE_EXECUTE,
	VALUE_WRITE,
	VALUE_LOCK,
};

class ITrainerItem {
	TrainerItemType type;
	uint64_t addr;
	
	virtual bool codeInject() = 0;
	virtual bool execute() = 0;
	virtual bool valueLock() = 0;
	virtual bool valueWrite() = 0;
};