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

	// �����������ڴ棬ֱ���ڴ���ע��㴦�޸�
	bool codeSimpleInject(uint64_t codeInjectAddr, BYTE* oldCode, BYTE* newCode, size_t codeSize);
	bool codeSimpleRestore(uint64_t codeRestoreAddr, BYTE* oldCode, size_t codeSize);

	// ���������ڴ棬�޸Ĵ���ע��㲢��ת�������ڴ洦ִ��shellCode
	bool codeInject(uint64_t codeInjectAddr, BYTE* oldCode, size_t codeSize, std::vector<BYTE> shellCode, LPVOID& virAddr);
	bool codeRestore(uint64_t codeRestoreAddr, BYTE* oldCode, size_t codeSize, LPVOID& virAddr);

	bool codeInjectTest();
	bool codeRestoreTest();
};
