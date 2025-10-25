// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "Easyhook.h"

typedef BOOL(WINAPI* GetCursorInfo_t)(PCURSORINFO);
GetCursorInfo_t OriginalGetCursorInfo = nullptr;
HOOK_TRACE_INFO hHook = { nullptr };
void** bypass = nullptr;


BOOL WINAPI HookedGetCursorInfo(PCURSORINFO pci) {
    BOOL result = OriginalGetCursorInfo(pci);
    if (result == true)
    {
        POINT nypt;
        GetCursorPos(&nypt);
        pci->ptScreenPos.x = nypt.x;
        pci->ptScreenPos.y = nypt.y;
        return true;
    }
    else return false;
}

void Setuphook() {

    HMODULE hUser32 = GetModuleHandle(L"user32.dll");
    if (!hUser32) MessageBoxA(NULL, "No User32? hooked too early maybe", "Error", MB_OK);

    void* target = GetProcAddress(hUser32, "GetCursorInfo");
    if (!target) MessageBoxA(NULL, "GetcursorInfo not found", "Error", MB_OK);

    // Install the hook
    NTSTATUS result = LhInstallHook(
        target,
        HookedGetCursorInfo,
        nullptr,
        &hHook
    );

    if (FAILED(result)) {
        MessageBoxA(NULL, "Failed to install EasyHook", "Error", MB_OK);
        return;
    }

    
    
    if (LhGetHookBypassAddress(&hHook, &bypass) == 0) {
        OriginalGetCursorInfo = (GetCursorInfo_t)bypass;
    }
    else {
        MessageBoxA(NULL, "Failed to get original function pointer", "Error", MB_OK);
    }

    // Enable the hook for all threads
    ULONG ACLEntries[1] = { 0 };
    result = LhSetInclusiveACL(ACLEntries, 1, &hHook);

    if (FAILED(result)) {
        MessageBoxA(NULL, "Failed to set ACL", "Error", MB_OK);
        return;
    }

}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        Setuphook();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

