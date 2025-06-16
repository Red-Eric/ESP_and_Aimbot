#include <Windows.h>
#include "func.h"
#include <vector>
#include <limits>
#include <cmath>
#include <iostream>
#include "esp/esp.h"
#include <d3d9.h>


DWORD WINAPI injected_thread(LPVOID lpParam) {
    HMODULE hModule = (HMODULE)lpParam;
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    std::vector<Vector3> AllEnnemiPos = {};
    std::vector<Vector2> AllEnnemiPos2D = {};

    uintptr_t posXStartAll = 0x007512F0;
    uintptr_t nbrPlayerAdd = 0x7512d4;
    uintptr_t viewMatrixAddr = 0xCD07680;
    int screenWidth = 0xc5afec;
    int screenHeight = 0xc5aff0;

    bool hacked = true;

    POINT cursorPos;

    while (!(GetAsyncKeyState(VK_RSHIFT) & 0x8000)) {
        AllEnnemiPos.clear();
        AllEnnemiPos2D.clear();
        system("cls");


        for (int i = 0; i < *(int*)nbrPlayerAdd - 1; i++) {
            Vector3 Temp{
                *(float*)(posXStartAll + (i * 0xf4)),
                *(float*)(posXStartAll + 0x4 + (i * 0xf4)),
                *(float*)(posXStartAll + 0x8 + (i * 0xf4)) + 50.0f
            };
            
            AllEnnemiPos.push_back(Temp);
        }

        screenWidth = *(int*)0xc5afec;
        screenHeight = *(int*)0xc5aff0;

        float viewMatrix[16];
        memcpy(viewMatrix, (void*)viewMatrixAddr, sizeof(viewMatrix));

        HWND hwnd = FindWindowA(NULL, "Call of Duty 4");
        HDC hdc = GetDC(hwnd);

        if (hwnd) {
            if (GetCursorPos(&cursorPos)) {
                ScreenToClient(hwnd, &cursorPos);

                std::cout << "-------------------MOUSE-----------------" << std::endl;
                std::cout << "X :" << cursorPos.x << std::endl;
                std::cout << "Y :" << cursorPos.y << std::endl;
                std::cout << "-----------------------------------------" << std::endl;
            }
            else {
                std::cout << "[!] Erreur GetCursorPos" << std::endl;
            }
        }

        for (const auto& ennemiPos : AllEnnemiPos) {
            Vector2 TempVec2;
            if (WorldToScreen(ennemiPos, TempVec2, viewMatrix, screenWidth, screenHeight)) {
                DrawLine(hdc, screenWidth / 2, screenHeight, (int)TempVec2.x, (int)TempVec2.y, RGB(255, 0, 0));
                AllEnnemiPos2D.push_back(TempVec2);
            }
        }

        ReleaseDC(hwnd, hdc);

        if (GetKeyState(VK_RBUTTON) < 0 && hwnd) {
            if (!AllEnnemiPos2D.empty()) {
                Vector2 target = GetNearestFromCenter(AllEnnemiPos2D, screenWidth, screenHeight);
                MoveMouseToWindowOrigin(hwnd,target.x, target.y);
            }
            
        }

        Sleep(10);
    }
    std::cout << "exit" << std::endl;
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}



BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hinstDLL);
        CreateThread(NULL, 0, injected_thread, hinstDLL, 0, NULL);
    }
    return TRUE;
}