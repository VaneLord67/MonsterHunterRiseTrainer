#pragma once
#include "ITrainerItem.h"

class TransportTrainerItem : public ITrainerItem {
private:
	float input[3] = { 0.0f, 0.0f, 0.0f };
	// ��Ϸ��λ����������ַ��ͬ����������������ַ�Ĵ�ֱ�����ֵ��ͬ������δ֪������һ�������λ��һ��������λ�ã�
	// ��ͣ��Ϸ��ͬʱ�޸���������ַ��ֵ���ָ���Ϸ����ʵ��˲��
	uint64_t posAddr1 = 0;
	uint64_t posAddr2 = 0;

	uint64_t imageAddr = 0xF963DEC; // ��̬��ַ��ʱ�̶�Ӧ��ҵ�λ��

	// �������
	bool showErrorWindow = false;
	bool showHelpWindow = false;
	bool showSuccessReadPosWindow = false;
	bool showFailReadPosWindow = false;

	void getPosAddr() {
		posAddr1 = 0;
		posAddr2 = 0;
		// �ȶ�ȡ��̬��ַ�е�x/yֵ�������Զ��Ӧ��ҵ�λ�ã������Դ�Ϊ��׼��������ȥ�ҵ�ַ
		float x = 0.0f;
		float y = 0.0f;
		pm->readMemory(imageAddr, {}, &x, 4);
		x = -x; // ���ﲻ֪��Ϊʲôֻ��x��ʵ��ֵ���෴��
		pm->readMemory(imageAddr + 8, {}, &y, 4);
		printf("player x = %.2f, y = %.2f\n", x, y);
		// ����Ϸ�У�λ����Ϣ�洢�����鵱�У������Ҫ�������飬����������������ȵ�ֵ��Ӧ�ĵ�ַposAdd1��posAddr2
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
		// ��������posAddr1
		while (findCnt++ < maxFindCnt) {
			uint64_t posAddr = 0;
			// ����posAddr1��Ƶ�����������0x0�������ֻ������ط���
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
		ImGui::OpenPopup(u8"��ȡʧ��##transport");
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
				// posAddr2�����������е�λ���������˱���Ѱ�ң�ÿ��ƫ�� i * 8
				pm->readMemory(arrayAddr + i * 8, &posAddr, 8);
				float tempX = 0.0f;
				float tempY = 0.0f;
				pm->readMemory(posAddr + 0x180, &tempX, 4);
				pm->readMemory(posAddr + 0x188, &tempY, 4);
				if (floatEqual(playerX, tempX) && floatEqual(playerY, tempY)) {
					posAddr2 = posAddr;
					printf("posAddr2 = 0x%016llX\n", posAddr2);
					// ��д����ֵ����ʾ��UI��
					input[0] = tempX;
					pm->readMemory(posAddr + 0x184, &input[1], 4);
					input[2] = tempY;
					showSuccessReadPosWindow = true;
					ImGui::OpenPopup(u8"��ȡ�ɹ�##transport");
					return;
				}
			}
		}
		showFailReadPosWindow = true;
		ImGui::OpenPopup(u8"��ȡʧ��##transport");
	}

	void transport() {
		if (posAddr1 == 0 || posAddr2 == 0) {
			popErrorWindow();
			return;
		}
		// д��������ַ�е�xyzֵ
		pm->writeMemory(posAddr1 + 0x30, {}, &input[0], 12, true);
		pm->writeMemory(posAddr2 + 0x180, {}, &input[0], 12, true);
	};

	void popErrorWindow() {
		showErrorWindow = true;
		ImGui::OpenPopup(u8"����##transport");
	}

	static bool floatEqual(float a, float b) {
		// ����Ϸ����Щλ�õĸ��������ϸ���ȵģ����ֱ��ʹ��==���������
		return a == b;
	};

public:
	void loop() {
		bool transportPress = ImGui::SmallButton(u8"Ӧ������");
		if (transportPress) {
			transport();
		}
		ImGui::SameLine();
		bool readPosPress = ImGui::SmallButton(u8"��ȡ����");
		if (readPosPress) {
			getPosAddr();
		}
		ImGui::SameLine();
		ImGui::PushItemWidth(150);
		ImGui::InputFloat3(u8"##transport_input", input, "%.0f");
		ImGui::PopItemWidth();
		ImGui::SameLine();
		bool helpPress = ImGui::SmallButton(u8"ʹ�ð�����##transport");
		if (helpPress) {
			showHelpWindow = true;
			ImGui::OpenPopup(u8"˲��˵��##transport");
		}

		if (ImGui::BeginPopupModal(u8"��ȡ�ɹ�##transport", &showSuccessReadPosWindow, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text(u8"��ȡ����ɹ�");
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopupModal(u8"��ȡʧ��##transport", &showFailReadPosWindow, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text(u8"��ȡ����ʧ�ܣ������Բ�����Ϸ����ͣ����±������ﾲֹ");
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopupModal(u8"˲��˵��##transport", &showHelpWindow, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text(u8"����ͣ״̬���ﾲֹʱ��ȡ����->��ȡ�ɹ�����д����ֵ->��ͣ��Ϸ��д����ֵ->Ӧ������");
			ImGui::Text(u8"ͬһ�������������ֻ��ȡһ������");
			ImGui::EndPopup();
		}
		if (ImGui::BeginPopupModal(u8"����##transport", &showErrorWindow, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text(u8"���ȶ�ȡ�������Ӧ��");
			ImGui::EndPopup();
		}
	};

	TransportTrainerItem(std::shared_ptr<ProcessManager> pm) : ITrainerItem(pm) {};

};
