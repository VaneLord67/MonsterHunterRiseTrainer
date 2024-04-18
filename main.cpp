#include <iostream>
#include "ProcessManager.h"

int main(void) {
    auto processManager = ProcessManager::init(L"Tutorial-x86_64.exe");
    if (!processManager) {
        std::cerr << "Failed to init ProcessManager" << std::endl;
        return -1;
    }
    int32_t readValue = 0;
    bool ok = true;
    ok = processManager->readMemory(0x306A70, { 0x7F0 }, &readValue, sizeof(readValue));
    if (!ok) {
        std::cerr << "Failed to read value" << std::endl;
        return -1;
    }
    std::cout << "readValue = " << readValue << std::endl;
    int32_t newValue = 1000;
    ok = processManager->writeMemory(0x306A70, { 0x7F0 }, &newValue, sizeof(newValue));
    if (!ok) {
        std::cerr << "Failed to write value" << std::endl;
        return -1;
    }
    return 0;
}

