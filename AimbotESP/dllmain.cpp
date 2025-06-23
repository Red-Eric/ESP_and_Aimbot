#include <Windows.h>
#include "func.h"
#include <vector>
#include <limits>
#include <cmath>
#include <iostream>
#include "esp/esp.h"
#include <string>


//==============Variable=======
std::vector<Vector3> AllEnnemiPos = {};
std::vector<Vector2> AllEnnemiPos2D = {};
uintptr_t posXStartAll = 0x007512F0;
uintptr_t nbrPlayerAdd = 0x7512d4;
uintptr_t viewMatrixAddr = 0xCD07680;

uintptr_t myPosXAdrss = 0x794490;
uintptr_t myPosYAdrss = 0x794494;
uintptr_t myPosZAdrss = 0x794498;

int screenWidth = 0xc5afec;
int screenHeight = 0xc5aff0;
bool hacked = true;
HWND hwnd;
float viewMatrix[16];
static float FOVAimbot = 150.0f;
uintptr_t matchTypeAddress = 0xcbb4b08;
//=============================

//=============================

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include "detours.h"
#pragma comment(lib, "detours.lib")

typedef HRESULT(__stdcall* endScene)(IDirect3DDevice9* pDevice);
endScene pEndScene;



void DrawCircleD3D(IDirect3DDevice9* device, float x, float y, float radius, D3DCOLOR color, int numSides = 20, bool write = false, const std::string& text = "") {
    ID3DXLine* line;
    if (FAILED(D3DXCreateLine(device, &line)))
        return;

    std::vector<D3DXVECTOR2> points(numSides + 1);
    for (int i = 0; i <= numSides; ++i) {
        float angle = D3DX_PI * 2 * i / numSides;
        points[i] = D3DXVECTOR2(x + radius * cosf(angle), y + radius * sinf(angle));
    }

    line->SetWidth(1.5f);
    line->Begin();
    line->Draw(points.data(), numSides + 1, color);
    line->End();
    line->Release();

    if (write && !text.empty()) {
        static ID3DXFont* font = nullptr;
        if (!font) {
            D3DXCreateFont(device, 16, 0, FW_NORMAL, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &font);
        }

        RECT rect;
        rect.left = (LONG)(x - radius);
        rect.top = (LONG)(y + radius + 5);
        rect.right = rect.left + 200;
        rect.bottom = rect.top + 50;

        font->DrawTextA(NULL, text.c_str(), -1, &rect, DT_LEFT | DT_TOP, color);
    }
}

static Vector3 myPos{
    0,0,0
};

HRESULT __stdcall hookedEndScene(IDirect3DDevice9* pDevice) {
    AllEnnemiPos.clear();
    AllEnnemiPos2D.clear();
    
    myPos.x = *(float*)myPosXAdrss;
    myPos.y = *(float*)myPosYAdrss;
    myPos.z = *(float*)myPosZAdrss;

    screenWidth = *(int*)0xc5afec;
    screenHeight = *(int*)0xc5aff0;

    // Adding 
    for (int i = 0; i < *(int*)nbrPlayerAdd - 1; i++) {
        Vector3 Temp{
            *(float*)(posXStartAll + (i * 0xf4)),
            *(float*)(posXStartAll + 0x4 + (i * 0xf4)),
            *(float*)(posXStartAll + 0x8 + (i * 0xf4))+53.0f
        };

        int checkAlive = *(int*)((posXStartAll + (i * 0xf4) + 0x18));

        if(checkAlive == 1)
            AllEnnemiPos.push_back(Temp);
    }

    memcpy(viewMatrix, (void*)viewMatrixAddr, sizeof(viewMatrix));

    hwnd = FindWindowA(NULL, "Call of Duty 4");

    //3D to 2D posArray
    for (const auto& ennemiPos : AllEnnemiPos) {
        Vector2 TempVec2;
        if (WorldToScreen(ennemiPos, TempVec2, viewMatrix, screenWidth, screenHeight)) {
            TempVec2.info = GetDistanceInMetersString(myPos, ennemiPos);
            AllEnnemiPos2D.push_back(TempVec2);
        }
    }

    // draw ESP Line 
    for (const auto& pos : AllEnnemiPos2D) {
        /*DrawLineD3D(pDevice, screenWidth / 2.0f, screenHeight, pos.x, pos.y, D3DCOLOR_ARGB(255, 0, 255, 0));*/
        DrawCircleD3D(pDevice, pos.x, pos.y, 10, D3DCOLOR_ARGB(255, 255, 0, 0),20,true,pos.info);
    }

    // draw FOV
    DrawCircleD3D(pDevice, screenWidth / 2.0f, screenHeight / 2.0f, FOVAimbot, D3DCOLOR_ARGB(255, 0, 255, 0));

    return pEndScene(pDevice);
}

void hookEndScene() {
    IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION); // create IDirect3D9 object
    if (!pD3D)
        return;

    D3DPRESENT_PARAMETERS d3dparams = { 0 };
    d3dparams.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dparams.hDeviceWindow = GetForegroundWindow();
    d3dparams.Windowed = true;

    IDirect3DDevice9* pDevice = nullptr;

    HRESULT result = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dparams.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dparams, &pDevice);
    if (FAILED(result) || !pDevice) {
        pD3D->Release();
        return;
    }
    void** vTable = *reinterpret_cast<void***>(pDevice);
    pEndScene = (endScene)DetourFunction((PBYTE)vTable[42], (PBYTE)hookedEndScene);
    pDevice->Release();
    pD3D->Release();
}

//=============================

DWORD WINAPI injected_thread(LPVOID lpParam) {

    HMODULE hModule = (HMODULE)lpParam;
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    hookEndScene();

    while (!(GetAsyncKeyState(VK_RSHIFT) & 0x8000)) {

        if (GetKeyState(VK_RBUTTON) < 0 && hwnd) {
            if (!AllEnnemiPos2D.empty()) {
                Vector2 target;
                if (GetNearestFromCenter(AllEnnemiPos2D, screenWidth, screenHeight, FOVAimbot, target)) {
                    MoveMouseToWindowOrigin(hwnd, target.x, target.y);
                }
            }
        }

        Sleep(50);
    }


    DetourRemove((PBYTE)pEndScene, (PBYTE)hookedEndScene);

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