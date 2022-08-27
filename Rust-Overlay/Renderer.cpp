#include "Renderer.h"
#include "AimBot.h"
#include "global.h"
static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;
extern LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))  return true;
    switch (msg)
    {
    case WM_CREATE:
    {
        MARGINS Margin = { -1 };
        DwmExtendFrameIntoClientArea(hWnd, &Margin);
        break;
    }
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            if (g_mainRenderTargetView) g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL;
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            ID3D11Texture2D* pBackBuffer;
            g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
            g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
            pBackBuffer->Release();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
BOOL Draw::Install(LPCWSTR WindowName, LPCWSTR WindowClassName, PVOID FontBuffer, ULONG BufferSize, FLOAT FontSize)
{
    this->GameHwnd = FindWindow(WindowClassName, WindowName);
    this->Buffer = FontBuffer;
    this->Size = FontSize;
    RECT rect{ 0 };
    WNDCLASSEX wClass = { 0 };
    if (GetWindowRect(this->GameHwnd, &rect)) {
        this->WindowWidth = rect.right - rect.left;
        this->WindowHeight = rect.bottom - rect.top;
    }
    wClass.cbSize = sizeof(WNDCLASSEX);
    wClass.style = CS_HREDRAW | CS_VREDRAW;
    wClass.lpfnWndProc = WndProc;
    wClass.cbClsExtra = 0;
    wClass.cbWndExtra = 0;
    wClass.hInstance = GetModuleHandle(NULL);
    wClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wClass.hbrBackground = (HBRUSH)RGB(0, 0, 0);
    wClass.lpszMenuName = 0;
    wClass.lpszClassName = L"afawobo";
    wClass.hIconSm = LoadIcon(0, IDI_APPLICATION);
    if (!RegisterClassEx(&wClass)) return FALSE;
    this->TopHwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW, wClass.lpszClassName, L"kugiybaf", WS_POPUP, rect.left, rect.top, this->WindowWidth, this->WindowHeight, NULL, NULL, wClass.hInstance, NULL);
    if (!this->TopHwnd) return FALSE;
    UpdateWindow(this->TopHwnd);
    ShowWindow(this->TopHwnd, SW_SHOW);
    SetLayeredWindowAttributes(this->TopHwnd, 0, 255, LWA_ALPHA);
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = this->TopHwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK) return false;
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;
    ImGui_ImplWin32_Init(this->TopHwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
    this->Font = io.Fonts->AddFontFromMemoryTTF(this->Buffer, BufferSize, 12.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    this->MenuFont = io.Fonts->AddFontFromMemoryTTF(this->Buffer, BufferSize,15.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    return  TRUE;
}
VOID Draw::Create(_DrawFunc Drawfunc) {
    MSG uMSG = { 0 };
    DWORD lastTime = NULL;
    RECT rect{ 0 };
    ImVec4 clear_color = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    while (TRUE) {
        if (PeekMessage(&uMSG, NULL, 0, 0, PM_REMOVE))
        {
            if (uMSG.message == WM_QUIT) break;
            TranslateMessage(&uMSG);
            DispatchMessage(&uMSG);
        }
        if (timeGetTime() - lastTime > 50)
        {
            GetWindowRect(this->GameHwnd, &rect);
            this->WindowWidth = rect.right - rect.left;
            this->WindowHeight = rect.bottom - rect.top;
            SetWindowPos(this->TopHwnd, HWND_TOPMOST, rect.left, rect.top, this->WindowWidth, this->WindowHeight, SWP_SHOWWINDOW);
            lastTime = timeGetTime();
        }
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        {
            Drawfunc();
        }
        ImGui::Render();
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        g_pSwapChain->Present(0, 0);
        Sleep(this->SleepTime);
    }
}
VOID Draw::DrawString(FLOAT x, FLOAT y, LPCSTR String, ImVec4 Colour,BOOL OutLine)
{
    std::string utf8 = miscs::get().AnisToUTF8(String);
    if (OutLine) {
        ImGui::GetForegroundDrawList()->AddText(ImVec2(x + 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), utf8.c_str());
        ImGui::GetForegroundDrawList()->AddText(ImVec2(x - 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), utf8.c_str());
        ImGui::GetForegroundDrawList()->AddText(ImVec2(x, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), utf8.c_str());
        ImGui::GetForegroundDrawList()->AddText(ImVec2(x, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), utf8.c_str());
    }
    ImGui::GetForegroundDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(Colour), utf8.c_str());
}
VOID Draw::DrawCircleFilled(FLOAT x, FLOAT y, FLOAT Radius, ImVec4 Colour)
{
    if (Radius > 0.f) ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(x, y), Radius, ImGui::ColorConvertFloat4ToU32(ImVec4(Colour)), 360);
}
VOID Draw::DrawCircle(FLOAT x, FLOAT y, FLOAT Radius, ImVec4 Colour, FLOAT thickness)
{
    if (Radius > 0.f) ImGui::GetForegroundDrawList()->AddCircle(ImVec2(x, y), Radius, ImGui::ColorConvertFloat4ToU32(ImVec4(Colour)),360, thickness);
}
VOID Draw::DrawRect(FLOAT x, FLOAT y, FLOAT w, FLOAT h, ImVec4 Colour, FLOAT thickness)
{
    ImGui::GetForegroundDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(Colour)), 0, 0, thickness);
}
VOID Draw::DrawRectFilled(FLOAT x, FLOAT y, FLOAT w, FLOAT h, ImVec4 Colour)
{
    ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(Colour)), 0, 0);
}
VOID Draw::DrawLine(FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2, ImVec4 Colour, FLOAT thickness)
{
    ImGui::GetForegroundDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), ImGui::ColorConvertFloat4ToU32(ImVec4(Colour)), thickness);
}
ImVec4 RGBA(FLOAT R, FLOAT G, FLOAT B, FLOAT A) {
    ImVec4 Color;
    Color.x = R / 255.0f;
    Color.y = G / 255.0f;
    Color.z = B / 255.0f;
    Color.w = A / 255.0f;
    return Color;
}
VOID Draw::DrawWindows(BOOL v, LPCSTR Title) {
    FLOAT x = WindowX;
    FLOAT y = WindowY;
    this->DrawRect(x - 1, y - 3, 259.f, 18.f, RGBA(255, 255, 255, 255), 1);
    this->DrawRectFilled(x, y - 2, 257.f, 16.f, RGBA(0, 0, 0, 60));
    this->DrawString(x, y, Title, RGBA(0, 255, 0, 255), TRUE);
    ImGui::PushFont(this->MenuFont);
    if (!v) {
        this->DrawString(x + 242.f, y - 1, "-", RGBA(255, 255, 255, 255), FALSE);
        this->DrawRectFilled(x, y + 15.f, 258.f, 20.f, RGBA(0, 0, 0, 60));
        this->DrawRectFilled(x, y + 35.f, 258.f, 108.f, RGBA(0, 0, 0, 60));
        this->DrawRect(x - 1, y + 14, 259.f, 129.f, RGBA(255, 255, 255, 255), 1);
    }
    else this->DrawString(x + 242.f, y - 1, "+", RGBA(255, 255, 255, 255), FALSE);
    ImGui::PopFont();
}
VOID Draw::DrawButton(FLOAT x, FLOAT y, LPCSTR String, BOOL v) {
    x = x + WindowX;
    y = y + WindowY;
    this->DrawString(x + 22.f, y + 2, String, RGBA(255, 255, 255, 255), FALSE);
    if (v) this->DrawRectFilled(x + 5.f, y + 3.f, 12.f, 12.f, RGBA(0, 255, 0, 255));
    else this->DrawRectFilled(x + 5.f, y + 3.f, 12.f, 12.f, RGBA(255, 0, 0, 100));
}
VOID Draw::DrawChoose(FLOAT x, FLOAT y,LPCSTR String, BOOL v) {
    x = x + WindowX;
    y = y + WindowY;
    if (v) this->DrawRectFilled(x, y, 50,15, RGBA(255, 20, 0, 120));
    else this->DrawRectFilled(x, y, 50,15, RGBA(28, 28, 28, 120));
    this->DrawString(x + 13, y + 1, String, RGBA(255, 255, 255, 255), FALSE);
}
VOID Draw::RangeFloat(FLOAT x, FLOAT y, LPCSTR String) {
    x = x + WindowX;
    y = y + WindowY;
    FLOAT X = _x + WindowX, Y = _y + WindowY;
    this->DrawString(x, y + 1, String, RGBA(255, 255, 255, 255), FALSE);
    this->DrawRectFilled(x + 55, y + 8, 100, 1, RGBA(255, 255, 255, 180));
    this->DrawRectFilled(X, Y, 5, 11, RGBA(255, 255, 0, 255));
    this->DrawString(x + 160, y + 1, std::to_string((int)_x + 35).c_str(), RGBA(255, 255, 255, 255), FALSE);
}
VOID Draw::Key() {
    if (GetAsyncKeyState(VK_END) & 1) {
        IsExit = FALSE;
        miscs::get().SendNotify("Bye", "欢迎下次使用~", TRUE);
        miscs::get().DeleteNotify();
        ExitProcess(1);
    }
    if (GetAsyncKeyState(VK_F2) & 1) {
        Setting::get().ReadRendererSettings();
        Setting::get().ReadColorSettings();
        Setting::get().ReadDistanceSettings();
    }
    FLOAT x = WindowX, y = WindowY;
    Vec2 Pos{};
    BOOL Move = FALSE, IsDown = FALSE, MouseMove = FALSE;
    if (miscs::get().CheckFullscreen(this->GameHwnd)) Pos.Set(miscs::get().GetMousePos(this->GameHwnd).x, miscs::get().GetMousePos(this->GameHwnd).y);
    else Pos.Set(miscs::get().GetMousePos(this->GameHwnd).x + 7.f, miscs::get().GetMousePos(this->GameHwnd).y + 30.f);
    if (!miscs::get().KeyState(1)) {
        Move = FALSE;
        IsDown = FALSE;
    }
    if (IsDown == FALSE && Pos.x > x + _x - 10 && Pos.x < x  + _x + 16.f && Pos.y > y + _y - 1 && Pos.y < y + _y + 12.f) {
        if (!miscs::get().KeyState(1)) mousepos = Pos; else MouseMove = TRUE;
    }
    if (miscs::get().KeyState(1) == TRUE && IsDown == FALSE) {
        if (Pos.x > x + 242.f && Pos.x < x + 252.f && Pos.y > y && Pos.y < y + 15.f) {
            IsDown = TRUE;
            if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) {
            HideMenu = !HideMenu;
            }
        }
        if (!HideMenu) {
            if (Pos.x > x + 13.f && Pos.x < x + 63.f && Pos.y > y + 20 && Pos.y < y + 35.f)
            {
                IsDown = TRUE;
                if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) {
                    this->ESP = TRUE;
                    this->_Aimbot = FALSE;
                    this->Memory = FALSE;
                    this->Rander = FALSE;
                }
            }
            if (Pos.x > x + 73.f && Pos.x < x + 123.f && Pos.y > y + 20 && Pos.y < y + 35.f)
            {
                IsDown = TRUE;
                if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) {
                    this->ESP = FALSE;
                    this->_Aimbot = TRUE;
                    this->Memory = FALSE;
                    this->Rander = FALSE;
                }
            }
            if (Pos.x > x + 133.f && Pos.x < x + 183.f && Pos.y > y + 20 && Pos.y < y + 35.f)
            {
                IsDown = TRUE;
                if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) {
                    this->ESP = FALSE;
                    this->_Aimbot = FALSE;
                    this->Memory = TRUE;
                    this->Rander = FALSE;
                }
            }
            if (Pos.x > x + 193.f && Pos.x < x + 243.f && Pos.y > y + 20 && Pos.y < y + 35.f)
            {
                IsDown = TRUE;
                if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) {
                    this->ESP = FALSE;
                    this->_Aimbot = FALSE;
                    this->Memory = FALSE;
                    this->Rander = TRUE;
                }
            }
            if (this->ESP) {
                if (Pos.x > x + 5.f + 5.f && Pos.x < x + 5.f + 5.f + 12.f && Pos.y > y + 40.f + 2.f && Pos.y < y + 40.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) Box = !Box;
                }
                if (Pos.x > x + 50.f + 5.f && Pos.x < x + 50.f + 5.f + 12.f && Pos.y > y + 40.f + 2.f && Pos.y < y + 40.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) Name = !Name;
                }
                if (Pos.x > x + 95.f + 5.f && Pos.x < x + 95.f + 5.f + 12.f && Pos.y > y + 40.f + 2.f && Pos.y < y + 40.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) HeldWeapon = !HeldWeapon;
                }
                if (Pos.x > x + 140.f + 5.f && Pos.x < x + 140.f + 5.f + 12.f && Pos.y > y + 40.f + 2.f && Pos.y < y + 40.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) Ray = !Ray;
                }
                if (Pos.x > x + 185.f + 5.f && Pos.x < x + 185.f + 5.f + 12.f && Pos.y > y + 40.f + 2.f && Pos.y < y + 40.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) SupplyBox = !SupplyBox;
                }

                if (Pos.x > x + 5.f + 5.f && Pos.x < x + 5.f + 5.f + 12.f && Pos.y > y + 60.f + 2.f && Pos.y < y + 60.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) Corpse = !Corpse;
                }
                if (Pos.x > x + 50.f + 5.f && Pos.x < x + 50.f + 5.f + 12.f && Pos.y > y + 60.f + 2.f && Pos.y < y + 60.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) Npc = !Npc;
                }
                if (Pos.x > x + 95.f + 5.f && Pos.x < x + 95.f + 5.f + 12.f && Pos.y > y + 60.f + 2.f && Pos.y < y + 60.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) Offline = !Offline;
                }
                if (Pos.x > x + 140.f + 5.f && Pos.x < x + 140.f + 5.f + 12.f && Pos.y > y + 60.f + 2.f && Pos.y < y + 60.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) HideBox = !HideBox;
                }
                if (Pos.x > x + 185.f + 5.f && Pos.x < x + 185.f + 5.f + 12.f && Pos.y > y + 60.f + 2.f && Pos.y < y + 60.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) MilitaryBox = !MilitaryBox;
                }

                if (Pos.x > x + 5.f + 5.f && Pos.x < x + 5.f + 5.f + 12.f && Pos.y > y + 80.f + 2.f && Pos.y < y + 80.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) Stone = !Stone;
                }
                if (Pos.x > x + 50.f + 5.f && Pos.x < x + 50.f + 5.f + 12.f && Pos.y > y + 80.f + 2.f && Pos.y < y + 80.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) Sulfur = !Sulfur;
                }
                if (Pos.x > x + 95.f + 5.f && Pos.x < x + 95.f + 5.f + 12.f && Pos.y > y + 80.f + 2.f && Pos.y < y + 80.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) Metal = !Metal;
                }
                if (Pos.x > x + 140.f + 5.f && Pos.x < x + 140.f + 5.f + 12.f && Pos.y > y + 80.f + 2.f && Pos.y < y + 80.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) Hemp = !Hemp;
                }
                if (Pos.x > x + 185.f + 5.f && Pos.x < x + 185.f + 5.f + 12.f && Pos.y > y + 80.f + 2.f && Pos.y < y + 80.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) Superbox = !Superbox;
                }

                if (Pos.x > x + 5.f + 5.f && Pos.x < x + 5.f + 5.f + 12.f && Pos.y > y + 100.f + 2.f && Pos.y < y + 100.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) Food = !Food;
                }
                if (Pos.x > x + 50.f + 5.f && Pos.x < x + 50.f + 5.f + 12.f && Pos.y > y + 100.f + 2.f && Pos.y < y + 100.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) Animal = !Animal;
                }
                if (Pos.x > x + 95.f + 5.f && Pos.x < x + 95.f + 5.f + 12.f && Pos.y > y + 100.f + 2.f && Pos.y < y + 100.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) Tank = !Tank;
                }
                if (Pos.x > x + 140.f + 5.f && Pos.x < x + 140.f + 5.f + 12.f && Pos.y > y + 100.f + 2.f && Pos.y < y + 100.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) ArmedHelicopter = !ArmedHelicopter;
                }
                if (Pos.x > x + 185.f + 5.f && Pos.x < x + 185.f + 5.f + 12.f && Pos.y > y + 100.f + 2.f && Pos.y < y + 100.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) TerritoryBox = !TerritoryBox;
                }
                if (Pos.x > x + 5.f + 5.f && Pos.x < x + 5.f + 5.f + 12.f && Pos.y > y + 120.f + 2.f && Pos.y < y + 120.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) Turret = !Turret;
                }
                if (Pos.x > x + 50.f + 5.f && Pos.x < x + 50.f + 5.f + 12.f && Pos.y > y + 120.f + 2.f && Pos.y < y + 120.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) AropBox = !AropBox;
                }
                if (Pos.x > x + 95.f + 5.f && Pos.x < x + 95.f + 5.f + 12.f && Pos.y > y + 120.f + 2.f && Pos.y < y + 120.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) Car = !Car;
                }
            }
            if (this->_Aimbot) {
                if (IsDown == FALSE && Pos.x > x + _x - 1 - MouseMove * 10 && Pos.x < x + _x + 6.f + MouseMove * 10 && Pos.y > y + _y - 2 - MouseMove * 10 && Pos.y < y + _y + 12.f + MouseMove * 10) {
                    if (miscs::get().KeyState(1) == TRUE && MouseMove == TRUE) {
                        if (_x < 65)_x = 65;
                        if (_x > 165)_x = 165;
                        _x = _x + (Pos - mousepos).x;
                        mousepos = Pos;
                    }
                }
                if (Pos.x > x + 5.f + 5.f && Pos.x < x + 5.f + 5.f + 12.f && Pos.y > y + 40.f + 2.f && Pos.y < y + 40.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) AimBot = !AimBot;
                }
                if (Pos.x > x + 50.f + 5.f && Pos.x < x + 50.f + 5.f + 12.f && Pos.y > y + 40.f + 2.f && Pos.y < y + 40.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) Position = !Position;
                }
                if (Pos.x > x + 95.f + 5.f && Pos.x < x + 95.f + 5.f + 12.f && Pos.y > y + 40.f + 2.f && Pos.y < y + 40.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) Position = !Position;
                }
                if (Pos.x > x + 140.f + 5.f && Pos.x < x + 140.f + 5.f + 12.f && Pos.y > y + 40.f + 2.f && Pos.y < y + 40.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) AimBotType = !AimBotType;
                }
                if (Pos.x > x + 185.f + 5.f && Pos.x < x + 185.f + 5.f + 12.f && Pos.y > y + 40.f + 2.f && Pos.y < y + 40.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) AimBotType = !AimBotType;
                }
            
            }
            if (this->Memory) {
                if (Pos.x > x + 5.f + 5.f && Pos.x < x + 5.f + 5.f + 12.f && Pos.y > y + 40.f + 2.f && Pos.y < y + 40.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) Recoil = !Recoil;
                }
                if (Pos.x > x + 90.f + 5.f && Pos.x < x + 90.f + 5.f + 12.f && Pos.y > y + 40.f + 2.f && Pos.y < y + 40.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) swag = !swag;
                }
                if (Pos.x > x + 175.f + 5.f && Pos.x < x + 175.f + 5.f + 12.f && Pos.y > y + 40.f + 2.f && Pos.y < y + 40.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) {
                        Nofall = !Nofall;
                        Aimbot::get().NoFall(Nofall);
                    }
                }
                if (Pos.x > x + 5.f + 5.f && Pos.x < x + 5.f + 5.f + 12.f && Pos.y > y + 40.f + 2.f && Pos.y < y + 40.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) Wall = !Wall;
                }
                if (Pos.x > x + 90.f + 5.f && Pos.x < x + 90.f + 5.f + 12.f && Pos.y > y + 60.f + 2.f && Pos.y < y + 60.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) Melee = !Melee;
                }
                if (Pos.x > x + 175.f + 5.f && Pos.x < x + 175.f + 5.f + 12.f && Pos.y > y + 60.f + 2.f && Pos.y < y + 60.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) {
                        fly = !fly;
                        Aimbot::get().Fly(fly);
                    }
                }
                if (Pos.x > x + 5.f + 5.f && Pos.x < x + 5.f + 5.f + 12.f && Pos.y > y + 60.f + 2.f && Pos.y < y + 60.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) {
                        Admin = !Admin;
                        if (!Admin) Aimbot::get().open = 1;
                    }
                }
            }
            if (this->Rander) {
                if (Pos.x > x + 5.f + 5.f && Pos.x < x + 5.f + 5.f + 12.f && Pos.y > y + 40.f + 2.f && Pos.y < y + 40.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) RanderSwith = !RanderSwith;
                }
                if (Pos.x > x + 50.f + 5.f && Pos.x < x + 50.f + 5.f + 12.f && Pos.y > y + 40.f + 2.f && Pos.y < y + 40.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) __Box = !__Box;
                }
                if (Pos.x > x + 95.f + 5.f && Pos.x < x + 95.f + 5.f + 12.f && Pos.y > y + 40.f + 2.f && Pos.y < y + 40.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) __Corpse = !__Corpse;
                }
                if (Pos.x > x + 140.f + 5.f && Pos.x < x + 140.f + 5.f + 12.f && Pos.y > y + 40.f + 2.f && Pos.y < y + 40.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) __npc = !__npc;
                }
                if (Pos.x > x + 185.f + 5.f && Pos.x < x + 185.f + 5.f + 12.f && Pos.y > y + 40.f + 2.f && Pos.y < y + 40.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) __MilitaryBox = !__MilitaryBox;
                }

                if (Pos.x > x + 5.f + 5.f && Pos.x < x + 5.f + 5.f + 12.f && Pos.y > y + 60.f + 2.f && Pos.y < y + 60.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) __Stone = !__Stone;
                }
                if (Pos.x > x + 50.f + 5.f && Pos.x < x + 50.f + 5.f + 12.f && Pos.y > y + 60.f + 2.f && Pos.y < y + 60.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) __Sulfur = !__Sulfur;
                }
                if (Pos.x > x + 95.f + 5.f && Pos.x < x + 95.f + 5.f + 12.f && Pos.y > y + 60.f + 2.f && Pos.y < y + 60.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) __Metal = !__Metal;
                }
                if (Pos.x > x + 140.f + 5.f && Pos.x < x + 140.f + 5.f + 12.f && Pos.y > y + 60.f + 2.f && Pos.y < y + 60.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) __Hemp = !__Hemp;
                }
                if (Pos.x > x + 185.f + 5.f && Pos.x < x + 185.f + 5.f + 12.f && Pos.y > y + 60.f + 2.f && Pos.y < y + 60.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) __Superbox = !__Superbox;
                }

                if (Pos.x > x + 5.f + 5.f && Pos.x < x + 5.f + 5.f + 12.f && Pos.y > y + 80.f + 2.f && Pos.y < y + 80.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) __Food = !__Food;
                }
                if (Pos.x > x + 50.f + 5.f && Pos.x < x + 50.f + 5.f + 12.f && Pos.y > y + 80.f + 2.f && Pos.y < y + 80.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) __Animal = !__Animal;
                }
                if (Pos.x > x + 95.f + 5.f && Pos.x < x + 95.f + 5.f + 12.f && Pos.y > y + 80.f + 2.f && Pos.y < y + 80.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) __Tank = !__Tank;
                }
                if (Pos.x > x + 140.f + 5.f && Pos.x < x + 140.f + 5.f + 12.f && Pos.y > y + 80.f + 2.f && Pos.y < y + 80.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) __Turret = !__Turret;
                }
                if (Pos.x > x + 185.f + 5.f && Pos.x < x + 185.f + 5.f + 12.f && Pos.y > y + 80.f + 2.f && Pos.y < y + 80.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) __TerritoryBox = !__TerritoryBox;
                }

                if (Pos.x > x + 5.f + 5.f && Pos.x < x + 5.f + 5.f + 12.f && Pos.y > y + 100.f + 2.f && Pos.y < y + 100.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) __HideBox = !__HideBox;
                }
                if (Pos.x > x + 50.f + 5.f && Pos.x < x + 50.f + 5.f + 12.f && Pos.y > y + 100.f + 2.f && Pos.y < y + 100.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) __Offline = !__Offline;
                }
                if (Pos.x > x + 95.f + 5.f && Pos.x < x + 95.f + 5.f + 12.f && Pos.y > y + 100.f + 2.f && Pos.y < y + 100.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) __AropBox = !__AropBox;
                }
                if (Pos.x > x + 140.f + 5.f && Pos.x < x + 140.f + 5.f + 12.f && Pos.y > y + 100.f + 2.f && Pos.y < y + 100.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) __Car = !__Car;
                }
                if (Pos.x > x + 185.f + 5.f && Pos.x < x + 185.f + 5.f + 12.f && Pos.y > y + 100.f + 2.f && Pos.y < y + 100.f + 3.f + 12.f) {
                    IsDown = TRUE;
                    if (GetAsyncKeyState(1) & 1 && IsDown == TRUE) __SupplyBox = !__SupplyBox;
                }
            }
        }
    }
    if (IsDown == FALSE) {
        if (Pos.x > x && Pos.x < x + 258.f) {
            if (Pos.y > y - 3 && Pos.y < y + 17.f) {
                if (!miscs::get().KeyState(1)) Local = Pos; else Move = TRUE;
            }
        }
    }
    if (miscs::get().KeyState(1) == TRUE && Move == TRUE) {
        WindowX = WindowX + (Pos - Local).x;
        WindowY = WindowY + (Pos - Local).y;
        Local = Pos;
    }
}
VOID Draw::DrawNewMenu() {
    Draw::get().Key();
    Draw::get().DrawWindows(HideMenu, ("      TL_Rust [END]安全退出 FPS:" + std::to_string((int)ImGui::GetIO().Framerate)).c_str());
    if (!HideMenu) {
        this->DrawChoose(13, 20, "透视", this->ESP);
        this->DrawChoose(73, 20, "自瞄", this->_Aimbot);
        this->DrawChoose(133, 20, "功能", this->Memory);
        this->DrawChoose(193, 20, "雷达", this->Rander);
        if (this->ESP) {
            this->DrawButton(5, 40, "玩家", Box);
            this->DrawButton(50, 40, "名字", Name);
            this->DrawButton(95, 40, "手持", HeldWeapon);
            this->DrawButton(140, 40, "射线", Ray);
            this->DrawButton(185, 40, "补给箱", SupplyBox);

            this->DrawButton(5, 60, "尸体", Corpse);
            this->DrawButton(50, 60, "NPC", Npc);
            this->DrawButton(95, 60, "离线", Offline);
            this->DrawButton(140, 60, "藏匿", HideBox);
            this->DrawButton(185, 60, "军工箱", MilitaryBox);

            this->DrawButton(5, 80, "石头", Stone);
            this->DrawButton(50, 80, "硫磺", Sulfur);
            this->DrawButton(95, 80, "金属", Metal);
            this->DrawButton(140, 80, "大麻", Hemp);
            this->DrawButton(185, 80, "精英箱", Superbox);

            this->DrawButton(5, 100, "食物", Food);
            this->DrawButton(50, 100, "动物", Animal);
            this->DrawButton(95, 100, "坦克", Tank);
            this->DrawButton(140, 100, "武直", ArmedHelicopter);
            this->DrawButton(185, 100, "领地柜", TerritoryBox);

            this->DrawButton(5, 120, "炮塔", Turret);
            this->DrawButton(50, 120, "空投", AropBox);
            this->DrawButton(95, 120, "船具", Car);
        }
        if (this->_Aimbot) {
            this->DrawButton(5, 40, "自瞄", AimBot);
            this->DrawButton(50, 40, "胸部", !Position);
            this->DrawButton(95, 40, "头部", Position);
            this->DrawButton(140, 40, "磁性", AimBotType);
            this->DrawButton(185, 40, "追踪", !AimBotType);
            this->RangeFloat(10,60,"瞄准范围:");
        }
        if (this->Memory) {
            this->DrawButton(5, 40, "无后坐力", Recoil);
            this->DrawButton(90, 40, "精准打击", swag);
            this->DrawButton(175, 40, "坠落无伤", Nofall);
            this->DrawButton(5, 60, "爬墙无摔", Wall);
            this->DrawButton(90, 60, "近战加强", Melee);
            this->DrawButton(175, 60, "游泳飞天", fly);
            this->DrawButton(5, 80, "管理权限", Admin);
        }
        if (this->Rander) {
            this->DrawButton(5, 40, "显示", RanderSwith);
            this->DrawButton(50, 40, "玩家", __Box);
            this->DrawButton(95, 40, "尸体", __Corpse);
            this->DrawButton(140, 40, "NPC", __npc);
            this->DrawButton(185, 40, "军工箱", __MilitaryBox);

            this->DrawButton(5, 60, "石头", __Stone);
            this->DrawButton(50, 60, "硫磺", __Sulfur);
            this->DrawButton(95, 60, "金属", __Metal);
            this->DrawButton(140, 60, "大麻", __Hemp);
            this->DrawButton(185, 60, "精英箱", __Superbox);

            this->DrawButton(5, 80, "食物", __Food);
            this->DrawButton(50, 80, "动物", __Animal);
            this->DrawButton(95, 80, "坦克", __Tank);
            this->DrawButton(140, 80, "炮塔", __Turret);
            this->DrawButton(185, 80, "领地柜", __TerritoryBox);

            this->DrawButton(5, 100, "藏匿", __HideBox);
            this->DrawButton(50, 100, "离线", __Offline);
            this->DrawButton(95, 100, "空投", __AropBox);
            this->DrawButton(140, 100, "船具", __Car);
            this->DrawButton(185, 100, "补给箱", __SupplyBox);
        }
    }
    
}