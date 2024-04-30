#include "../include/gui.h"
#include <iostream>
#include "../include/ProcessManager.h"
#include "../include/ITrainerItem.h"
#include "../include/TrainerItems.h"
#include <chrono>
#include <thread>

void initTrainerItems(std::vector<std::shared_ptr<ITrainerItem>>& trainerItems, std::shared_ptr<ProcessManager> pm) {
    std::cout << "initTrainerItems" << std::endl;
    trainerItems = {
        // checkbox
        std::make_shared<HPTrainerItem>(pm),
        std::make_shared<StaminaTrainerItem>(pm),
        std::make_shared<AttackPropertyTrainerItem>(pm),
        std::make_shared<DefensePropertyTrainerItem>(pm),
        std::make_shared<LockWeaponDurabilityTrainerItem>(pm),
        std::make_shared<LongSwordEnergyTrainerItem>(pm),
        std::make_shared<LongSwordLevelTrainerItem>(pm),
        std::make_shared<FreeBuyTrainerItem>(pm),
        std::make_shared<ForeverUseItemTrainerItem>(pm),
        std::make_shared<WirebugZeroCoolDownTrainerItem>(pm),
        std::make_shared<ForeverEnvWirebugTrainerItem>(pm),
        //std::make_shared<UnlimitGrabMaterialTrainerItem>(pm), // 该功能会导致结算后游戏崩溃，故不开放
        std::make_shared<SteelHeartTrainerItem>(pm),
        
        // button
        std::make_shared<MoneyTrainerItem>(pm),
        std::make_shared<PointTrainerItem>(pm),
        std::make_shared<BagFirstItemCountTrainerItem>(pm),
        std::make_shared<BagFirstItemIDTrainerItem>(pm),
        std::make_shared<TargetMonsterHPTrainerItem>(pm),
        std::make_shared<TransportTrainerItem>(pm),
        // raw button
        std::make_shared<ZeroHourglassTrainerItem>(pm),
        std::make_shared<ResetTaskTimeTrainerItem>(pm),
    };
}

auto pm = std::make_shared<ProcessManager>();
auto processName = L"MonsterHunterRise.exe";
const wchar_t* windowName = L"怪物猎人崛起：曙光 修改器";
std::vector<std::shared_ptr<ITrainerItem>> trainerItems = {};

std::atomic<bool> processAvailable(false);
std::atomic<bool> initThreadStopSignal(false);

void initThreadFunc() {
    auto sleepSec = 0;
    while (!initThreadStopSignal) {
        if (!pm->processRunning()) {
            processAvailable = false;
            if (ProcessManager::init(processName, pm)) {
                initTrainerItems(trainerItems, pm);
                processAvailable = true;
            }
            sleepSec = 1;
        } else {
            processAvailable = true;
            sleepSec = 5;
        }
        std::this_thread::sleep_for(std::chrono::seconds(sleepSec));
        std::cout << "init thread sleep for " << sleepSec << " s" << std::endl;
    }
}

int WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
//int main(void)
{
    std::thread initThread(initThreadFunc);

    bool ok = GuiMain([]() {
        ImGui::Begin(u8"##main_window", &show_main_window, main_window_flags);

        if (!processAvailable) {
            ImGui::Text(u8"未检测到进程运行: %ls", processName);
        }
        else {
            ImGui::Text(u8"进程已运行：%ls", processName);
            for (auto& trainerItem : trainerItems) {
                trainerItem->loop();
            }
        }
        ImGui::SetCursorPosY(ImGui::GetWindowHeight() - ImGui::GetTextLineHeightWithSpacing());
        ImGui::Text("Author Github: https://github.com/VaneLord67");

        ImGui::End();
    });
    initThreadStopSignal = true;
    initThread.join();
    if (!ok) {
        return 1;
    }

    return 0;
}
