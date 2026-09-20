// dllmain.cpp
#include "pch.h"
#include <Windows.h>
#include <cstdint>
#include <cstdio>
#include <thread>
#include <atomic>
#include "derpysLuaTest.h"

// ======================================================
// Lua state (global in Bully)
// ======================================================
#define getLuaState() (*(lua_State**)0xD093B0)

// ======================================================
// Original function type
// ======================================================
typedef void(__thiscall* tAddEffectFunctions)(void* thisptr, void* table);

// Original function address
static tAddEffectFunctions Original_AddEffectFunctions =
(tAddEffectFunctions)0x5DB140;

// ======================================================
// Console debug helpers
// ======================================================
static void InitDebugConsole()
{
    AllocConsole();

    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    freopen_s(&f, "CONOUT$", "w", stderr);

    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, BACKGROUND_RED | BACKGROUND_INTENSITY);

    printf("[+] Debug console initialized\n");
}

// ======================================================
// Helper: draw text via Lua
// ======================================================
static void drawText(const char* str, float duration, int style)
{
    lua_State* L = getLuaState();

    if (!L)
    {
        printf("[!] drawText: Lua state is NULL\n");
        return;
    }

    printf("[+] drawText called: \"%s\"\n", str);

    lua_pushcfunction(L, (lua_CFunction)0x5D57F0);
    lua_pushstring(L, str);
    lua_pushnumber(L, duration);
    lua_pushnumber(L, style);
    lua_call(L, 3, 0);
}

// ======================================================
// Hook replacement for the CALL at 0x5DB5B4
// ======================================================
void __fastcall addEffectFunctions(void* thisptr, void* /*edx*/, void* table)
{
    printf("[+] addEffectFunctions HIT\n");
    printf("    thisptr = %p\n", thisptr);
    printf("    table   = %p\n", table);

    // Call original registration logic
    if (Original_AddEffectFunctions)
    {
        printf("[+] Calling original sub_5DB140...\n");
        Original_AddEffectFunctions(thisptr, table);
        printf("[+] Original sub_5DB140 returned\n");
    }
    else
    {
        printf("[!] Original_AddEffectFunctions is NULL\n");
    }

    // Injected behavior
    drawText("Effect Lua hook active", 3000.0f, 1);
}

// ======================================================
// CALL-site patcher
// ======================================================
static bool replaceFunctionCall(void* callSite, void* replacement)
{
    printf("[+] Attempting CALL patch at %p\n", callSite);

    DWORD oldProtect;
    if (!VirtualProtect(callSite, 5, PAGE_EXECUTE_READWRITE, &oldProtect))
    {
        printf("[!] VirtualProtect failed\n");
        return false;
    }

    *(uint8_t*)callSite = 0xE8; // CALL rel32
    *(int32_t*)((uintptr_t)callSite + 1) =
        (int32_t)((uintptr_t)replacement - ((uintptr_t)callSite + 5));

    VirtualProtect(callSite, 5, oldProtect, &oldProtect);

    printf("[+] CALL patched successfully\n");
    return true;
}

// ======================================================
// Hotkey thread for F6
// ======================================================
static std::atomic<bool> g_Running = true;

DWORD WINAPI HotkeyThread(LPVOID)
{
    printf("[+] Hotkey thread started (F6)\n");

    bool wasDown = false;

    while (g_Running)
    {
        SHORT key = GetAsyncKeyState(VK_F6);
        bool isDown = (key & 0x8000) != 0;

        if (isDown && !wasDown)
        {
            printf("[+] F6 pressed\n");
            drawText("F6 DrawText OK", 3.0f, 1);
        }

        wasDown = isDown;
        Sleep(10); // debounce
    }

    return 0;
}

// ======================================================
// DLL entry
// ======================================================
BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        InitDebugConsole();

        printf("[+] DLL_PROCESS_ATTACH\n");

        // Patch Lua effect registration
        if (!replaceFunctionCall((void*)0x5DB5B4, &addEffectFunctions))
        {
            printf("[!] replaceFunctionCall FAILED\n");
        }

        FlushInstructionCache(GetCurrentProcess(), nullptr, 0);
        printf("[+] Instruction cache flushed\n");

        // Start hotkey thread
        CreateThread(nullptr, 0, HotkeyThread, nullptr, 0, nullptr);
    }
    else if (reason == DLL_PROCESS_DETACH)
    {
        g_Running = false;
        printf("[+] DLL_PROCESS_DETACH\n");
    }

    return TRUE;
}
