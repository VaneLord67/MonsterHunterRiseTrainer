#pragma once
#include "ITrainerItem.h"

class TransportTrainerItem : public ITrainerItem {
private:
	float input[3] = { 0.0f, 0.0f, 0.0f };
	// 游戏中位置由两个地址共同决定，其中两个地址的垂直方向的值不同，含义未知，可能一个是相机位置一个是人物位置，
	// 暂停游戏后同时修改这两个地址的值，恢复游戏即可实现瞬移
	uint64_t posAddr1 = 0;
	uint64_t posAddr2 = 0;

	uint64_t imageAddr = 0xF963DEC; // 静态地址，时刻对应玩家的位置

	// 弹框控制
	bool showErrorWindow = false;
	bool showHelpWindow = false;
	bool showSuccessReadPosWindow = false;
	bool showFailReadPosWindow = false;

	void getPosAddr() {
		posAddr1 = 0;
		posAddr2 = 0;
		// 先读取静态地址中的x/y值，这个永远对应玩家的位置，后续以此为基准遍历数组去找地址
		float x = 0.0f;
		float y = 0.0f;
		pm->readMemory(imageAddr, {}, &x, 4);
		x = -x; // 这里不知道为什么只有x与实际值是相反数
		pm->readMemory(imageAddr + 8, {}, &y, 4);
		printf("player x = %.2f, y = %.2f\n", x, y);
		// 在游戏中，位置信息存储于数组当中，因此需要遍历数组，查找与人物坐标相等的值对应的地址posAdd1和posAddr2
		getPosAddr1(x, y);
		if (posAddr1 == 0) {
			return;
		}
		getPosAddr2(x, y);
		if (posAddr2 == 0) {
			return;
		}
	};

	void getPosAddr1(float playerX, float playerY) {
		const static int maxFindCnt = 50000;
		int findCnt = 0;
		// 遍历查找posAddr1
		while (findCnt++ < maxFindCnt) {
			uint64_t posAddr = 0;
			// 由于posAddr1高频出现在数组的0x0处，因此只在这个地方找
			pm->readMemory(0xF621620, { 0x90, 0x1804D8, 0x0 }, &posAddr, 8);
			float tempX = 0.0f;
			float tempY = 0.0f;
			pm->readMemory(posAddr + 0x30, &tempX, 4);
			pm->readMemory(posAddr + 0x38, &tempY, 4);
			if (floatEqual(playerX, tempX) && floatEqual(playerY, tempY)) {
				posAddr1 = posAddr;
				printf("posAddr1 = 0x%016llX\n", posAddr1);
				return;
			}
		}
		showFailReadPosWindow = true;
		ImGui::OpenPopup(u8"读取失败##transport");
	};

	void getPosAddr2(float playerX, float playerY) {
		const static int maxFindCnt = 10000;
		int findCnt = 0;
		while (findCnt < maxFindCnt) {
			uint64_t arrayAddr = 0;
			uint64_t posAddr = 0;
			pm->readMemory(0xF621358, { 0x8498 }, &arrayAddr, 8);
			for (uint64_t i = 0; i < 1000; i++) {
				if (findCnt++ >= maxFindCnt) {
					break;
				}
				// posAddr2出现在数组中的位置随机，因此遍历寻找，每次偏移 i * 8
				pm->readMemory(arrayAddr + i * 8, &posAddr, 8);
				float tempX = 0.0f;
				float tempY = 0.0f;
				pm->readMemory(posAddr + 0x180, &tempX, 4);
				pm->readMemory(posAddr + 0x188, &tempY, 4);
				if (floatEqual(playerX, tempX) && floatEqual(playerY, tempY)) {
					posAddr2 = posAddr;
					printf("posAddr2 = 0x%016llX\n", posAddr2);
					// 填写坐标值，显示到UI上
					input[0] = tempX;
					pm->readMemory(posAddr + 0x184, &input[1], 4);
					input[2] = tempY;
					showSuccessReadPosWindow = true;
					ImGui::OpenPopup(u8"读取成功##transport");
					return;
				}
			}
		}
		showFailReadPosWindow = true;
		ImGui::OpenPopup(u8"读取失败##transport");
	}

	void transport() {
		if (posAddr1 == 0 || posAddr2 == 0) {
			popErrorWindow();
			return;
		}
		// 写入两个地址中的xyz值
		pm->writeMemory(posAddr1 + 0x30, {}, &input[0], 12, true);
		pm->writeMemory(posAddr2 + 0x180, {}, &input[0], 12, true);
	};

	void popErrorWindow() {
		showErrorWindow = true;
		ImGui::OpenPopup(u8"错误##transport");
	}

	static bool floatEqual(float a, float b) {
		// 在游戏中这些位置的浮点数是严格相等的，因此直接使用==运算符即可
		return a == b;
	};

public:
	void loop() {
		bool transportPress = ImGui::SmallButton(u8"应用坐标");
		if (transportPress) {
			transport();
		}
		ImGui::SameLine();
		bool readPosPress = ImGui::SmallButton(u8"读取坐标");
		if (readPosPress) {
			getPosAddr();
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(150);
		ImGui::InputFloat3(u8"##transport_input", input, "%.0f");
		ImGui::PopItemWidth();
		ImGui::SameLine();
		bool helpPress = ImGui::SmallButton(u8"使用帮助？##transport");
		if (helpPress) {
			showHelpWindow = true;
			ImGui::OpenPopup(u8"瞬移说明##transport");
		}

		if (ImGui::BeginPopupModal(u8"读取成功##transport", &showSuccessReadPosWindow, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text(u8"读取坐标成功");
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopupModal(u8"读取失败##transport", &showFailReadPosWindow, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text(u8"读取坐标失败，请重试并在游戏非暂停情况下保持人物静止");
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopupModal(u8"瞬移说明##transport", &showHelpWindow, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text(u8"非暂停状态人物静止时读取坐标->读取成功后填写坐标值->暂停游戏填写坐标值->应用坐标");
			ImGui::Text(u8"同一次任务中你可以只读取一次坐标");
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopupModal(u8"错误##transport", &showErrorWindow, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text(u8"请先读取坐标后再应用");
			ImGui::EndPopup();
		}
	};

	TransportTrainerItem(std::shared_ptr<ProcessManager> pm) : ITrainerItem(pm) {};

};
