#pragma once
#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <vector>
#include <iomanip>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <mutex>

class ProcessManager {
private:
	DWORD pid;
	uint64_t baseAddress;
	HANDLE hProcess;
	std::mutex loopWriteSetMutex;
	std::unordered_set<uint64_t> loopWriteSet;

	uint64_t getTargetAddress(uint64_t addr, const std::vector<int64_t>& offsets);
public:
	ProcessManager() : pid(0), baseAddress(0), hProcess(0) {};

	static std::shared_ptr<ProcessManager> init(const wchar_t* processName);
	
	bool readMemory(uint64_t addr, const std::vector<int64_t>& offsets, void* value, size_t size);
	bool writeMemory(uint64_t addr, const std::vector<int64_t>& offsets, void* value, size_t size);
};
