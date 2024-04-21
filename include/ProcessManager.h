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
	LPVOID virAddr;

	uint64_t getTargetAddress(uint64_t addr, const std::vector<int64_t>& offsets);
public:
	ProcessManager() : pid(0), baseAddress(0), hProcess(0), virAddr(0) {};
	~ProcessManager() {
		if (hProcess) {
			CloseHandle(hProcess);
		}
	}

	static bool init(const wchar_t* processName, std::shared_ptr<ProcessManager> pm);
	bool processRunning();
	
	bool readMemory(uint64_t addr, const std::vector<int64_t>& offsets, void* value, size_t size);
	bool writeMemory(uint64_t addr, const std::vector<int64_t>& offsets, void* value, size_t size);

	// 不分配虚拟内存，直接在代码注入点处修改
	bool codeSimpleInject(uint64_t codeInjectAddr, BYTE* oldCode, BYTE* newCode, size_t codeSize);
	bool codeSimpleRestore(uint64_t codeRestoreAddr, BYTE* oldCode, size_t codeSize);

	// 分配虚拟内存，修改代码注入点并跳转到虚拟内存处执行shellCode
	bool codeInject(uint64_t codeInjectAddr, BYTE* oldCode, size_t codeSize, std::vector<BYTE> shellCode, LPVOID& virAddr);
	bool codeRestore(uint64_t codeRestoreAddr, BYTE* oldCode, size_t codeSize, LPVOID& virAddr);

	bool codeInjectTest();
	bool codeRestoreTest();
};
