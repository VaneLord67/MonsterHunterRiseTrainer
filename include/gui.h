#pragma once

#include "../third_party/imgui/imconfig.h"
#include "../third_party/imgui/imgui.h"
#include "../third_party/imgui/imgui_impl_dx11.h"
#include "../third_party/imgui/imgui_impl_win32.h"
#include "../third_party/imgui/imgui_internal.h"
#include "../third_party/imgui/imstb_rectpack.h"
#include "../third_party/imgui/imstb_textedit.h"
#include "../third_party/imgui/imstb_truetype.h"
#include <windows.h>
#include <functional>

#include "d3d11.h"
#pragma comment(lib, "d3d11.lib")

// Data
extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pd3dDeviceContext;
extern IDXGISwapChain* g_pSwapChain;
extern UINT g_ResizeWidth, g_ResizeHeight;
extern ID3D11RenderTargetView* g_mainRenderTargetView;
extern bool show_main_window;
extern ImGuiWindowFlags main_window_flags;
extern const wchar_t* windowName;


// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool GuiMain(const std::function<void()>& customUI);