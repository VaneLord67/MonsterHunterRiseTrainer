#include "../include/ProcessManager.h"

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

// 将DWORD转为4个BYTE写入byte_array中，小端形式
void DWORD2BYTE(DWORD dword, BYTE* byte_array) {
    byte_array[3] = (dword >> 24) & 0xFF;
    byte_array[2] = (dword >> 16) & 0xFF;
    byte_array[1] = (dword >> 8) & 0xFF;
    byte_array[0] = dword & 0xFF;
}

// 将QWORD转为8个BYTE写入byte_array中，小端形式
void QWORD2BYTE(DWORD64 qword, BYTE* byte_array) {
    byte_array[7] = (qword >> 56) & 0xFF;
    byte_array[6] = (qword >> 48) & 0xFF;
    byte_array[5] = (qword >> 40) & 0xFF;
    byte_array[4] = (qword >> 32) & 0xFF;
    byte_array[3] = (qword >> 24) & 0xFF;
    byte_array[2] = (qword >> 16) & 0xFF;
    byte_array[1] = (qword >> 8) & 0xFF;
    byte_array[0] = qword & 0xFF;
}

void vecInsertQWORD(DWORD64 qword, std::vector<BYTE>& vec) {
    vec.emplace_back(qword & 0xFF);
    vec.emplace_back((qword >> 8) & 0xFF);
    vec.emplace_back((qword >> 16) & 0xFF);
    vec.emplace_back((qword >> 24) & 0xFF);
    vec.emplace_back((qword >> 32) & 0xFF);
    vec.emplace_back((qword >> 40) & 0xFF);
    vec.emplace_back((qword >> 48) & 0xFF);
    vec.emplace_back((qword >> 56) & 0xFF);
}

bool ProcessManager::init(const wchar_t* processName, std::shared_ptr<ProcessManager> processManager) {
    auto pid = GetProcessIdByName(processName);
    processManager->pid = pid;
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (hProcess == NULL) {
        std::cerr << "Failed to open process. Error code: " << GetLastError() << std::endl;
        return false;
    }
    processManager->hProcess = hProcess;
    uint64_t baseAddress = GetProcessBaseAddress(pid, processName);
    if (baseAddress == 0) {
        std::cerr << "Failed to get base address of " << processName << std::endl;
        return false;
    }
    processManager->baseAddress = baseAddress;
    return true;
}

bool ProcessManager::processRunning() {
    if (this->pid == 0) {
        return false;
    }
    DWORD exitCode;
    if (GetExitCodeProcess(hProcess, &exitCode)) {
        return exitCode == STILL_ACTIVE;
    }
    return false; // 如果无法获取退出代码，则默认返回false
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

bool ProcessManager::codeSimpleInject(uint64_t codeInjectAddr, BYTE* oldCode, BYTE* newCode, size_t codeSize) {
    bool ok = ReadProcessMemory(this->hProcess, reinterpret_cast<LPCVOID>(baseAddress + codeInjectAddr), oldCode, codeSize, nullptr);
    if (!ok) {
        std::cerr << "Read old code error" << std::endl;
        return false;
    }
    ok = WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(this->baseAddress + codeInjectAddr), newCode, codeSize, NULL);
    if (!ok) {
        std::cerr << "Failed to write new code" << std::endl;
        return false;
    }
    return true;
}

bool ProcessManager::codeSimpleRestore(uint64_t codeRestoreAddr, BYTE* oldCode, size_t codeSize) {
    bool ok = WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(this->baseAddress + codeRestoreAddr), oldCode, codeSize, NULL);
    if (!ok) {
        std::cerr << "Failed to restore code" << std::endl;
        return false;
    }
    return true;
}

bool ProcessManager::codeInject(uint64_t codeInjectAddr, BYTE* oldCode, size_t codeSize, std::vector<BYTE> shellCode, LPVOID& virAddr) {
    if (codeSize == 0 || shellCode.size() == 0) {
        std::cerr << "code size error" << std::endl;
        return false;
    }
    std::vector<BYTE> newCode = { 0xFF, 0x25, 0x00, 0x00, 0x00, 0x00 };
    virAddr = VirtualAllocEx(hProcess, NULL, 4096, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (virAddr == NULL) {
        std::cerr << "Failed to alloc virtual memory" << std::endl;
        return false;
    }
    std::cerr << "virAddr = " << std::hex
        << std::setw(16) << std::setfill('0') << (uint64_t)virAddr << std::endl;
    vecInsertQWORD(reinterpret_cast<DWORD64>(virAddr), newCode);
    for (size_t i = 0; i < codeSize - newCode.size(); i++)
    {
        // fill code by nop
        newCode.emplace_back(0x90);
    }
    bool ok = ReadProcessMemory(this->hProcess, reinterpret_cast<LPCVOID>(baseAddress + codeInjectAddr), oldCode, codeSize, nullptr);
    if (!ok) {
        std::cerr << "Read old code error" << std::endl;
        return false;
    }
    ok = WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(this->baseAddress + codeInjectAddr), newCode.data(), codeSize, NULL);
    if (!ok) {
        std::cerr << "Failed to write new code" << std::endl;
        return false;
    }
    shellCode.insert(shellCode.end(), { 0xFF, 0x25, 0x00, 0x00, 0x00, 0x00 });
    uint64_t jmpTarget = this->baseAddress + codeInjectAddr + codeSize;
    vecInsertQWORD(jmpTarget, shellCode);

    ok = WriteProcessMemory(hProcess, virAddr, shellCode.data(), shellCode.size(), NULL);
    if (!ok) {
        std::cerr << "Failed to write shell code" << std::endl;
        return false;
    }
    return true;
}

bool ProcessManager::codeRestore(uint64_t codeRestoreAddr, BYTE* oldCode, size_t codeSize, LPVOID& virAddr) {
    bool ok = VirtualFreeEx(hProcess, virAddr, 0, MEM_RELEASE);
    if (!ok) {
        std::cerr << "Failed to free virtual memory" << std::endl;
        return false;
    }
    ok = WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(this->baseAddress + codeRestoreAddr), oldCode, codeSize, NULL);
    if (!ok) {
        std::cerr << "Failed to restore code" << std::endl;
        return false;
    }
    return true;
}

 bool ProcessManager::codeInjectTest() {
    // PWD 013370
    LPVOID virAddr = VirtualAllocEx(hProcess, NULL, 4096, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (virAddr == 0) {
        std::cerr << "Failed to alloc virtual memory" << std::endl;
        return false;
    }
    std::cerr << "virAddr = " << std::hex
        << std::setw(16) << std::setfill('0') << (uint64_t)virAddr << std::endl;
    this->virAddr = virAddr;
    BYTE oldCode[16] = { 0 };
    bool ok = ReadProcessMemory(this->hProcess, reinterpret_cast<LPCVOID>(baseAddress + 0x2D4F7), oldCode, sizeof(oldCode), nullptr);
    if (!ok) {
        std::cerr << "Read old code error" << std::endl;
        return false;
    }
  
    BYTE newCode[16] = { 0xFF, 0x25, 0x00, 0x00, 0x00, 0x00 };
    QWORD2BYTE((uint64_t)virAddr, newCode + 6);
    newCode[14] = 0x66;
    newCode[15] = 0x90;
    ok = WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(this->baseAddress + 0x2D4F7), newCode, 16, NULL);
    if (!ok) {
        std::cerr << "Failed to write new code" << std::endl;
        return false;
    }
    BYTE shellCode[64] = { 0x83, 0x86, 0xE0, 0x07, 0x00, 0x00, 0x02, 0x48, 0x8D, 0x4D, 0xF8, 0xFF, 0x15, 0x02, 0x00, 0x00, 0x00, 0xEB, 0x08};
    uint64_t callTarget = this->baseAddress + 0x8E20;
    QWORD2BYTE(callTarget, shellCode + 19);
    BYTE jmpFar[6] = { 0xFF, 0x25, 0x00, 0x00, 0x00, 0x00 };
    memcpy(shellCode + 27, jmpFar, 6);
    uint64_t jmpTarget = this->baseAddress + 0x2D4F7 + 16;
    QWORD2BYTE(jmpTarget, shellCode + 33);
    ok = WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(virAddr), shellCode, 41, NULL);
    if (!ok) {
        std::cerr << "Failed to write shell code" << std::endl;
        return false;
    }
    return true;
}

bool ProcessManager::codeRestoreTest() {
    if (this->virAddr == 0) {
        std::cerr << "this->virAddr == 0" << std::endl;
        return true;
    }
    bool ok = VirtualFreeEx(hProcess, this->virAddr, 0, MEM_RELEASE);
    if (!ok) {
        std::cerr << "Failed to free virtual memory" << std::endl;
        return false;
    }
    BYTE oldCode[16] = { 0x83, 0xAE, 0xE0, 0x07, 0x00, 0x00, 0x01, 0x48, 0x8D, 0x4D, 0xF8, 0xE8, 0x19, 0xB9, 0xFD, 0xFF };
    ok = WriteProcessMemory(hProcess, reinterpret_cast<LPVOID>(this->baseAddress + 0x2D4F7), oldCode, 16, NULL);
    if (!ok) {
        std::cerr << "Failed to restore code" << std::endl;
        return false;
    }
    return true;
}
