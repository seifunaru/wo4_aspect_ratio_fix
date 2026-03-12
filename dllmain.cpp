#include <Windows.h>
#include <cstdint>
#include <string>
#include <cstring>

/*
    Aspect Ratio Fix for Warriors Orochi 4
    Inject as dinput8.dll.

    Can optionally load zmod too by renaming
    zmod's dinput to "zmod.dll".
*/

typedef HRESULT(WINAPI* DirectInput8Create_t)(
    HINSTANCE,
    DWORD,
    REFIID,
    LPVOID*,
    LPUNKNOWN);

DirectInput8Create_t g_OriginalDirectInput8Create = nullptr;
HMODULE g_OriginalDll = nullptr;

//////////////////////////////////////////////////////////////////
// Memory writing helper
//////////////////////////////////////////////////////////////////

void WriteMemory(void* address, const void* data, size_t size)
{
    DWORD oldProtect;

    VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(address, data, size);
    VirtualProtect(address, size, oldProtect, &oldProtect);
}

//////////////////////////////////////////////////////////////////
// Simple pattern scanner
//////////////////////////////////////////////////////////////////

uintptr_t FindPattern(uint8_t* base, size_t size, const uint8_t* pattern, const char* mask)
{
    size_t patternLength = strlen(mask);

    for (size_t i = 0; i < size - patternLength; i++)
    {
        bool found = true;

        for (size_t j = 0; j < patternLength; j++)
        {
            if (mask[j] != '?' && pattern[j] != *(uint8_t*)(base + i + j))
            {
                found = false;
                break;
            }
        }

        if (found)
            return (uintptr_t)(base + i);
    }

    return 0;
}

//////////////////////////////////////////////////////////////////
// Retrieve current monitor aspect ratio
//////////////////////////////////////////////////////////////////

float GetAspectRatio()
{
    DEVMODEA devMode = {};
    devMode.dmSize = sizeof(devMode);

    if (EnumDisplaySettingsA(nullptr, ENUM_CURRENT_SETTINGS, &devMode))
    {
        return (float)devMode.dmPelsWidth /
            (float)devMode.dmPelsHeight;
    }

    // Fallback if detection fails
    return 16.0f / 9.0f;
}

//////////////////////////////////////////////////////////////////
// Apply the aspect ratio patch
//////////////////////////////////////////////////////////////////

void PatchAspectRatio()
{
    // Wait until the game module is loaded
    while (!GetModuleHandleA("WO4U.dll"))
        Sleep(100);

    HMODULE module = GetModuleHandleA("WO4U.dll");
    uint8_t* base = (uint8_t*)module;

    // Instruction pattern to replace
    uint8_t pattern[] =
    {
        0xF3,0x0F,0x11,0xAB,0x4C,0x01,0x00,0x00
    };

    uintptr_t address = FindPattern(base, 0x2000000, pattern, "xxxxxxxx");

    if (!address)
        return;

    float aspect = GetAspectRatio();

    uint32_t aspectBytes;
    memcpy(&aspectBytes, &aspect, sizeof(aspect));

    /*
        Replace original instruction with:

        mov dword ptr [rbx+14C], aspect_ratio
    */

    uint8_t patch[10] =
    {
        0xC7,0x83,0x4C,0x01,0x00,0x00,
        (uint8_t)(aspectBytes),
        (uint8_t)(aspectBytes >> 8),
        (uint8_t)(aspectBytes >> 16),
        (uint8_t)(aspectBytes >> 24)
    };

    WriteMemory((void*)address, patch, sizeof(patch));
}

//////////////////////////////////////////////////////////////////
// Load original dinput8.dll from system directory
//////////////////////////////////////////////////////////////////

void LoadOriginalDll()
{
    if (g_OriginalDll)
        return;

    char systemPath[MAX_PATH];
    GetSystemDirectoryA(systemPath, MAX_PATH);

    std::string path = std::string(systemPath) + "\\dinput8.dll";

    g_OriginalDll = LoadLibraryA(path.c_str());

    if (g_OriginalDll)
    {
        g_OriginalDirectInput8Create =
            (DirectInput8Create_t)GetProcAddress(
                g_OriginalDll,
                "DirectInput8Create");
    }
}

//////////////////////////////////////////////////////////////////
// Optional compatibility mod loader
//////////////////////////////////////////////////////////////////

void LoadOptionalZmod()
{
    if (GetFileAttributesA("zmod.dll") != INVALID_FILE_ATTRIBUTES)
    {
        LoadLibraryA("zmod.dll");
    }
}

//////////////////////////////////////////////////////////////////
// Main mod thread
//////////////////////////////////////////////////////////////////

DWORD WINAPI ModThread(LPVOID)
{
    LoadOptionalZmod();
    PatchAspectRatio();

    return 0;
}

//////////////////////////////////////////////////////////////////
// Forwarded DirectInput8Create
//////////////////////////////////////////////////////////////////

extern "C" __declspec(dllexport)
HRESULT WINAPI DirectInput8Create(
    HINSTANCE hinst,
    DWORD dwVersion,
    REFIID riidltf,
    LPVOID* ppvOut,
    LPUNKNOWN punkOuter)
{
    LoadOriginalDll();

    if (!g_OriginalDirectInput8Create)
        return E_FAIL;

    return g_OriginalDirectInput8Create(
        hinst,
        dwVersion,
        riidltf,
        ppvOut,
        punkOuter);
}

//////////////////////////////////////////////////////////////////
// DLL entry point
//////////////////////////////////////////////////////////////////

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hinstDLL);

        // Load the real system dinput8.dll
        LoadOriginalDll();

        // Start the mod thread
        CreateThread(nullptr, 0, ModThread, nullptr, 0, nullptr);
    }

    return TRUE;
}