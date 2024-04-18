#include "ProcessManager.h"

DWORD GetProcessIdByName(const wchar_t* processName) {
    DWORD pid = 0;
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(entry);
    // 获取系统中所有进程的快照
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to create process snapshot. Error code: " << GetLastError() << std::endl;
        return 0;
    }
    // 遍历进程快照，查找目标进程
    if (Process32First(snapshot, &entry)) {
        do {
            if (_wcsicmp(entry.szExeFile, processName) == 0) {
                pid = entry.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &entry));
    }
    // 关闭进程快照句柄
    CloseHandle(snapshot);
    return pid;
}

uintptr_t GetProcessBaseAddress(DWORD processId, const wchar_t* moduleName) {
    MODULEENTRY32 moduleEntry = { sizeof(moduleEntry) };
    uintptr_t baseAddress = 0;

    // 获取系统中所有模块的快照
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
    if (snapshot != INVALID_HANDLE_VALUE) {
        // 遍历模块快照，查找指定模块
        if (Module32First(snapshot, &moduleEntry)) {
            do {
                if (_wcsicmp(moduleEntry.szModule, moduleName) == 0) {
                    baseAddress = reinterpret_cast<uintptr_t>(moduleEntry.modBaseAddr);
                    break;
                }
            } while (Module32Next(snapshot, &moduleEntry));
        }
        // 关闭模块快照句柄
        CloseHandle(snapshot);
    }

    return baseAddress;
}

std::shared_ptr<ProcessManager> ProcessManager::init(const wchar_t* processName) {
    auto processManager = std::make_shared<ProcessManager>();
    auto pid = GetProcessIdByName(processName);
    processManager->pid = pid;
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProcess == NULL) {
        std::cerr << "Failed to open process. Error code: " << GetLastError() << std::endl;
        return nullptr;
    }
    processManager->hProcess = hProcess;
    uint64_t baseAddress = GetProcessBaseAddress(pid, processName);
    if (baseAddress == 0) {
        std::cerr << "Failed to get base address of " << processName << std::endl;
        return nullptr;
    }
    processManager->baseAddress = baseAddress;
    
    return processManager;
}

uint64_t ProcessManager::getTargetAddress(uint64_t addr, const std::vector<int64_t>& offsets) {
    uint64_t pointer = baseAddress + addr;
    bool ok = true;
    ok = ReadProcessMemory(this->hProcess, reinterpret_cast<LPCVOID>(pointer), &pointer, 8, nullptr);
    if (!ok) {
        std::cerr << "Failed to read addr:" << std::hex
            << std::setw(16) << std::setfill('0') << baseAddress + addr << std::endl;
        return 0;
    }
    for (auto it = offsets.begin(); it != offsets.end(); ++it) {
        const int64_t& offset = *it;
        if (it == std::prev(offsets.end())) {
            return pointer + offset;
        }
        ok = ReadProcessMemory(this->hProcess, reinterpret_cast<LPCVOID>(pointer + offset), &pointer, 8, nullptr);
        if (!ok) {
            std::cerr << "Failed to read addr:" << std::hex
                << std::setw(16) << std::setfill('0') << pointer << std::endl;
            return 0;
        }
    }
    return 0;
}

bool ProcessManager::readMemory(uint64_t addr, const std::vector<int64_t>& offsets, void* value, size_t size) {
    uint64_t targetAddress = getTargetAddress(addr, offsets);
    return ReadProcessMemory(this->hProcess, reinterpret_cast<LPCVOID>(targetAddress), value, size, nullptr);
}

bool ProcessManager::writeMemory(uint64_t addr, const std::vector<int64_t>& offsets, void* value, size_t size) {
    uint64_t targetAddress = getTargetAddress(addr, offsets);
    return WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(targetAddress), value, size, NULL);
}
