// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "MinHook.h"

typedef BOOL(WINAPI* GetCursorInfo_t)(PCURSORINFO);
GetCursorInfo_t OriginalGetCursorInfo = nullptr;



BOOL WINAPI HookedGetCursorInfo(PCURSORINFO pci) {
    if (pci && pci->cbSize == sizeof(CURSORINFO)) {
        // BOOL result = OriginalGetCursorInfo(pci);
        POINT nypt;
        GetCursorPos(&nypt);
        pci->ptScreenPos.x = nypt.x;
        pci->ptScreenPos.y = nypt.y;
        return true;
    }
    return FALSE;
}

void Setuphook() {

    if (MH_Initialize() != MH_OK) {
        MessageBoxA(NULL, "Failed to initialize MinHook", "Error", MB_OK);
	}
    HMODULE hUser32 = GetModuleHandle(L"user32.dll");
    if (!hUser32) MessageBoxA(NULL, "No User32? hooked too early maybe", "Error", MB_OK);

    void* target = GetProcAddress(hUser32, "GetCursorInfo");
    if (!target) MessageBoxA(NULL, "GetcursorInfo not found", "Error", MB_OK);

    if (MH_CreateHook(target, &HookedGetCursorInfo, reinterpret_cast<void**>(&OriginalGetCursorInfo)) != MH_OK) {
        MessageBoxA(NULL, "Failed to create hook", "Error", MB_OK);
    }

    if (MH_EnableHook(target) != MH_OK) {
        MessageBoxA(NULL, "Failed to enable hook", "Error", MB_OK);
    }


}
DWORD WINAPI ThreadFunction(LPVOID lpParam)
{
	Sleep(2000);
    Setuphook();
    return 0;// Wait for 2 seconds to ensure the target process is ready
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // Load dll
        Setuphook();
       // CreateThread(nullptr, 0,
       //     (LPTHREAD_START_ROUTINE)ThreadFunction, GetModuleHandle(0), 0, 0); //GetModuleHandle(0)
        
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

