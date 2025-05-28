// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "detours.h"
#include "packets.h"
#include "vietsub.h" // Thêm include cho file vietsub
#include "NDGM.h"

#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <future>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <unordered_map>

#pragma comment(lib, "Ws2_32.lib")

// Bỏ comment dòng dưới đây để bật chế độ debug console
#define DEBUG

// Handle của game process
HANDLE handlGame = nullptr;

// Định nghĩa các con trỏ hàm gốc
typedef void (*_LoadFastMap_t)();
typedef void (*_LoadMapsFast_t)();
typedef void (*_AddressPassGG_t)();

_LoadFastMap_t original_LoadFastMap = nullptr;
_LoadMapsFast_t original_LoadMapsFast = nullptr;
_AddressPassGG_t original_AddressPassGG = nullptr;

// Định nghĩa địa chỉ các hàm cần hook
DWORD FastLoadMapAddr = 0x005A0E26;
DWORD LoadMapsFastAddr = 0x5A0A9C;
DWORD AddressPassGGAdr = 0x00839B65;
DWORD AddressPassGG1 = 0x007BBAFB;
DWORD AddressPassGG2 = 0x00413B96;
DWORD AddressPassGG3 = 0x004263AA;
DWORD AddressPassGG4 = 0x005A5616;
DWORD AddressPassGG5 = 0x006A98B3;
DWORD AddressPassGG7 = 0x006A9C2D;

// File logging
FILE* fp;

// Khai báo các hàm việt hóa
// Hàm FastLoadMap thay thế
__declspec(naked) void LoadFastMap()
{
    __asm
    {
        push 0x005A0E2D
        ret
    }
}

// Hàm LoadMapsFast thay thế
__declspec(naked) void HookedLoadMapsFast()
{
    __asm
    {
        push eax
        mov eax, 0x00000001
        mov[ebp + 0x5054], eax
        mov[ebp + 0x5058], eax
        mov[ebp + 0x505C], eax
        pop eax
        push 0x005A0AAE
        ret
    }
}

// Hàm PassGG cho GameGuard bypass
__declspec(naked)void AddressPassGG()  //00574C20
{
    __asm
    {
        nop
        nop
        nop
        nop
        nop
        push eax
        mov eax, 0x0085C28C
        call[eax]
        test eax, eax
        jz NotRun
        push 0x00839BB3
        ret
        NotRun :
        mov eax, 0x00404B50
            call eax
    }
}

// Cấu trúc cho việc hook
struct extHook {
    BYTE originalBytes[64];
    DWORD originalLength;
    DWORD hookAddress;
    HANDLE processHandle;

    extHook(HANDLE hProc, DWORD sourceFnc, DWORD targetFnc, DWORD length) {
        processHandle = hProc;
        hookAddress = targetFnc;
        originalLength = length;

        // Lưu các byte gốc
        ReadProcessMemory(hProc, (LPCVOID)targetFnc, originalBytes, length, NULL);

        // Tạo và ghi JMP hook
        BYTE jmpCode[5] = { 0xE9, 0x00, 0x00, 0x00, 0x00 };
        *(DWORD*)(jmpCode + 1) = sourceFnc - targetFnc - 5;

        DWORD oldProtect;
        VirtualProtectEx(hProc, (LPVOID)targetFnc, length, PAGE_EXECUTE_READWRITE, &oldProtect);
        WriteProcessMemory(hProc, (LPVOID)targetFnc, jmpCode, 5, NULL);

        // Nếu hook size > 5, fill NOP cho các byte còn lại
        if (length > 5) {
            BYTE nopBytes[64];
            memset(nopBytes, 0x90, 64);
            WriteProcessMemory(hProc, (LPVOID)(targetFnc + 5), nopBytes, length - 5, NULL);
        }

        VirtualProtectEx(hProc, (LPVOID)targetFnc, length, oldProtect, &oldProtect);
    }

    // Hàm để unhook khi cần
    void Unhook() {
        DWORD oldProtect;
        VirtualProtectEx(processHandle, (LPVOID)hookAddress, originalLength, PAGE_EXECUTE_READWRITE, &oldProtect);
        WriteProcessMemory(processHandle, (LPVOID)hookAddress, originalBytes, originalLength, NULL);
        VirtualProtectEx(processHandle, (LPVOID)hookAddress, originalLength, oldProtect, &oldProtect);
    }
};

// Hàm PatchEx để ghi các byte vào memory
void PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess)
{
    DWORD oldprotect;
    VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
    WriteProcessMemory(hProcess, dst, src, size, nullptr);
    VirtualProtectEx(hProcess, dst, size, oldprotect, &oldprotect);
}

// Hàm bổ sung để check exception
LONG WINAPI MyUnhandledExceptionFilter(EXCEPTION_POINTERS* pExceptionPointers)
{
#ifdef DEBUG
    std::cout << "Exception detected in NDCLIENT" << std::endl;
#endif
    return EXCEPTION_CONTINUE_SEARCH;
}

// Hàm để áp dụng các bypass patches
void ApplyBypassPatches()
{
    if (!handlGame) {
#ifdef DEBUG
        std::cout << "ApplyBypassPatches: Error - handlGame is NULL" << std::endl;
#endif
        return;
    }

    try {
#ifdef DEBUG
        std::cout << "ApplyBypassPatches: Applying bypass patches" << std::endl;
#endif

        // Bo Qua Yeu Cap Nhap 3 Ky Tu
        DWORD ByPassRequestCharacter = 0x0042770A;
        PatchEx((BYTE*)ByPassRequestCharacter, (BYTE*)"\xEB\x47", 2, handlGame);
        DWORD ByPassRequestCharacter2 = 0x00410E6F;
        PatchEx((BYTE*)ByPassRequestCharacter2, (BYTE*)"\xEB\x4D", 2, handlGame);

#ifdef DEBUG
        std::cout << "ApplyBypassPatches: Bypass patches applied successfully" << std::endl;
#endif
    }
    catch (const std::exception& e) {
#ifdef DEBUG
        std::cout << "ApplyBypassPatches: Exception caught: " << e.what() << std::endl;
#endif
    }
    catch (...) {
#ifdef DEBUG
        std::cout << "ApplyBypassPatches: Unknown exception caught" << std::endl;
#endif
    }
}

// Hàm tạo bypass game guard
void ApplyGameGuardBypass(DWORD ProIDNineDragons)
{
    try {
#ifdef DEBUG
        std::cout << "ApplyGameGuardBypass: Applying Game Guard bypass patches" << std::endl;
        std::cout << "DEBUG: Checking for GG address conflicts with FastLoadMap (0x"
            << std::hex << std::uppercase << FastLoadMapAddr << ")" << std::endl;
#endif

        // Mở process với quyền đầy đủ
        HANDLE handlGameTemp = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProIDNineDragons);

        if (!handlGameTemp) {
#ifdef DEBUG
            std::cout << "ApplyGameGuardBypass: Failed to open process, error "
                << GetLastError() << std::endl;
#endif
            return;
        }

        BYTE AddressGG2[6] = { 0xEB, 0x4C, 0x90, 0x90, 0x90, 0x90 };
        BYTE AddressGG3[6] = { 0xE9, 0xF0, 0x01, 0x00, 0x00, 0x90 };

        // Áp dụng các patch để bypass Game Guard
        PatchEx((BYTE*)AddressPassGGAdr, (BYTE*)AddressGG2, 6, handlGameTemp);
        PatchEx((BYTE*)AddressPassGG1, (BYTE*)AddressGG3, 6, handlGameTemp);
        PatchEx((BYTE*)AddressPassGG2, (BYTE*)"\xEB\x17", 2, handlGameTemp);
        PatchEx((BYTE*)AddressPassGG3, (BYTE*)"\xEB\x1C", 2, handlGameTemp);
        PatchEx((BYTE*)AddressPassGG4, (BYTE*)"\xEB\x16", 2, handlGameTemp);
        PatchEx((BYTE*)AddressPassGG5, (BYTE*)"\xEB\x16", 2, handlGameTemp);
        PatchEx((BYTE*)AddressPassGG7, (BYTE*)"\xEB\x0E", 2, handlGameTemp);

        // Đóng handle khi đã sử dụng xong
        CloseHandle(handlGameTemp);

#ifdef DEBUG
        std::cout << "ApplyGameGuardBypass: Game Guard bypass patches applied successfully" << std::endl;
#endif
    }
    catch (const std::exception& e) {
#ifdef DEBUG
        std::cout << "ApplyGameGuardBypass: Exception caught: " << e.what() << std::endl;
#endif
    }
    catch (...) {
#ifdef DEBUG
        std::cout << "ApplyGameGuardBypass: Unknown exception caught" << std::endl;
#endif
    }
}

// Hàm để áp dụng patch trực tiếp cho FastLoadMap
void ApplyFastLoadMapPatch()
{
#ifdef DEBUG
    std::cout << "ApplyFastLoadMapPatch: Directly patching FastLoadMap function" << std::endl;
#endif

    try {
        if (!handlGame) {
#ifdef DEBUG
            std::cout << "ApplyFastLoadMapPatch: Error - handlGame is NULL, using GetCurrentProcess()" << std::endl;
#endif
            handlGame = GetCurrentProcess();
        }

        // Đọc và hiển thị byte trước khi patch
        BYTE beforePatch[8] = { 0 };
        SIZE_T bytesRead = 0;
        if (ReadProcessMemory(handlGame, (LPCVOID)FastLoadMapAddr, beforePatch, sizeof(beforePatch), &bytesRead)) {
            std::stringstream ss;
            for (size_t i = 0; i < bytesRead; i++) {
                ss << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
                    << static_cast<int>(beforePatch[i]) << " ";
            }
#ifdef DEBUG
            std::cout << "ApplyFastLoadMapPatch: Before patch at 0x" << std::hex << std::uppercase
                << FastLoadMapAddr << ": " << ss.str() << std::endl;
#endif
        }

        // Detour cho FastLoadMap thay vì patch trực tiếp
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        // Detour FastLoadMap function
        original_LoadFastMap = (_LoadFastMap_t)FastLoadMapAddr;
        DetourAttach(&(PVOID&)original_LoadFastMap, LoadFastMap);

        LONG result = DetourTransactionCommit();
        if (result != NO_ERROR) {
#ifdef DEBUG
            std::cout << "ApplyFastLoadMapPatch: Error applying Detour to FastLoadMap: "
                << result << std::endl;
#endif
        }
        else {
#ifdef DEBUG
            std::cout << "ApplyFastLoadMapPatch: FastLoadMap detoured successfully" << std::endl;
#endif
        }

        // Đọc và hiển thị byte sau khi detour để xác nhận
        BYTE afterPatch[8] = { 0 };
        if (ReadProcessMemory(handlGame, (LPCVOID)FastLoadMapAddr, afterPatch, sizeof(afterPatch), &bytesRead)) {
            std::stringstream ss2;
            for (size_t i = 0; i < bytesRead; i++) {
                ss2 << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
                    << static_cast<int>(afterPatch[i]) << " ";
            }
#ifdef DEBUG
            std::cout << "ApplyFastLoadMapPatch: After detour at 0x" << std::hex << std::uppercase
                << FastLoadMapAddr << ": " << ss2.str() << std::endl;
#endif
        }
    }
    catch (const std::exception& e) {
#ifdef DEBUG
        std::cout << "ApplyFastLoadMapPatch: Exception caught: " << e.what() << std::endl;
#endif
    }
    catch (...) {
#ifdef DEBUG
        std::cout << "ApplyFastLoadMapPatch: Unknown exception caught" << std::endl;
#endif
    }
}

std::string GetLineFromCin()
{
    std::string line;
    std::getline(std::cin, line);
    return line;
}

// Biến để kiểm soát quá trình chạy
volatile bool isRunning = true;

// Hàm để khởi tạo tất cả các hooks
void InitializeAllHooks() {
#ifdef DEBUG
    std::cout << "InitializeAllHooks: Setting up all hooks and patches" << std::endl;
#endif

    // Bắt đầu transaction Detours
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    // Hook FastLoadMap function
    original_LoadFastMap = (_LoadFastMap_t)FastLoadMapAddr;
    DetourAttach(&(PVOID&)original_LoadFastMap, LoadFastMap);

    // Hook LoadMapsFast function
    original_LoadMapsFast = (_LoadMapsFast_t)LoadMapsFastAddr;
    DetourAttach(&(PVOID&)original_LoadMapsFast, HookedLoadMapsFast);

    // Hook AddressPassGG function
    original_AddressPassGG = (_AddressPassGG_t)AddressPassGGAdr;
    DetourAttach(&(PVOID&)original_AddressPassGG, AddressPassGG);

    // Kết thúc transaction
    LONG result = DetourTransactionCommit();
    if (result != NO_ERROR) {
#ifdef DEBUG
        std::cout << "InitializeAllHooks: Error setting up hooks: " << result << std::endl;
#endif
    }
    else {
#ifdef DEBUG
        std::cout << "InitializeAllHooks: All hooks set up successfully" << std::endl;
#endif

        // Áp dụng các patches sau khi hooks đã được thiết lập
        ApplyBypassPatches();

        // Lấy Process ID của game (ở đây là process hiện tại)
        DWORD currentProcessID = GetCurrentProcessId();

        // Áp dụng bypass GameGuard
        ApplyGameGuardBypass(currentProcessID);

        // Khởi tạo các hook việt hóa
        InitializeVietHoaHooks();

        InitializeGMHooks();
    }
}

// Thread chính
int MainThread(HMODULE hModule)
{
#ifdef DEBUG
    // Mở console để debug
    AllocConsole();
    SetConsoleTitle(L"[debug] NDCLIENT Hook Test");
    FILE* consoleOut = nullptr;
    FILE* consoleErr = nullptr;
    FILE* consoleIn = nullptr;

    freopen_s(&consoleOut, "CONOUT$", "w", stdout);
    freopen_s(&consoleErr, "CONOUT$", "w", stderr);
    freopen_s(&consoleIn, "CONIN$", "r", stdin);

    std::cout << "MainThread: Starting NDCLIENT hook test version" << std::endl;
#endif

    // Đặt handler cho unhandled exception
    SetUnhandledExceptionFilter(MyUnhandledExceptionFilter);

    try {
        // Lấy handle của game process (hiện tại đang là handle của process hiện tại)
        handlGame = GetCurrentProcess();
        if (handlGame) {
#ifdef DEBUG
            std::cout << "MainThread: Got game process handle" << std::endl;
#endif

            // Khởi tạo tất cả các hooks và patches
            InitializeAllHooks();
        }
        else {
#ifdef DEBUG
            std::cout << "MainThread: Failed to get game process handle" << std::endl;
#endif
        }

#ifdef DEBUG
        // Khởi tạo đọc input từ console
        auto future = std::async(std::launch::async, GetLineFromCin);

        std::cout << "MainThread: Entering main loop" << std::endl;
#endif

        // Khởi tạo đếm thời gian
        auto startTime = std::chrono::steady_clock::now();

        while (isRunning)
        {
#ifdef DEBUG
            // Kiểm tra xem có input từ console không
            if (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                auto line = future.get();
                future = std::async(std::launch::async, GetLineFromCin);

                // Log lệnh nhận được
                std::cout << "MainThread: Command received: " << line << std::endl;

                // Xử lý các lệnh từ console
                if (line == "exit") {
                    std::cout << "MainThread: Exit command received" << std::endl;
                    isRunning = false;
                }
                else if (line == "msgbox") {
                    std::cout << "MainThread: MessageBox command received" << std::endl;
                    MessageBoxA(NULL, "This is a test from console", "Manual Test", MB_OK);
                }
                else if (line == "ticks") {
                    DWORD ticks = GetTickCount();
                    std::cout << "MainThread: GetTickCount returned " << ticks << std::endl;
                }
                else if (line == "bypass") {
                    std::cout << "MainThread: Bypass command received" << std::endl;
                    ApplyBypassPatches();
                }
                else if (line == "fastload") {
                    std::cout << "MainThread: FastLoadMap patch command received" << std::endl;
                    ApplyFastLoadMapPatch();
                }
                else if (line == "memory") {
                    std::cout << "MainThread: Memory info command received" << std::endl;
                    // Lệnh memory đã bị loại bỏ
                }
                else if (line == "viethoa") {
                    std::cout << "MainThread: Toggling Vietnamese localization" << std::endl;
                    static bool vietHoaEnabled = false;
                    vietHoaEnabled = !vietHoaEnabled;

                    ToggleVietHoa(vietHoaEnabled); // Gọi hàm từ vietsub.cpp

                    if (vietHoaEnabled) {
                        printf("Vietnamese localization enabled\n");
                    }
                    else {
                        printf("Vietnamese localization disabled\n");
                    }
                }
                else if (line == "help") {
                    printf("Type a command to execute an action.\n");
                }
                else {
                    printf("Unknown command: %s\n", line.c_str());
                }
            }
#endif
            // Giảm tần suất kiểm tra để không tốn CPU
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    catch (const std::exception& e) {
#ifdef DEBUG
        std::cout << "MainThread: Exception caught: " << e.what() << std::endl;
#endif
    }
    catch (...) {
#ifdef DEBUG
        std::cout << "MainThread: Unknown exception caught" << std::endl;
#endif
    }

#ifdef DEBUG
    std::cout << "MainThread: Exiting main thread" << std::endl;
#endif

    // Dọn dẹp các hooks
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    if (original_LoadFastMap)
        DetourDetach(&(PVOID&)original_LoadFastMap, LoadFastMap);
    if (original_LoadMapsFast)
        DetourDetach(&(PVOID&)original_LoadMapsFast, HookedLoadMapsFast);
    if (original_AddressPassGG)
        DetourDetach(&(PVOID&)original_AddressPassGG, AddressPassGG);
    DetourTransactionCommit();

    // Dọn dẹp các hooks việc hóa
    CleanupVietHoaHooks(); // Gọi từ vietsub.cpp

    CleanupGMHooks();

#ifdef DEBUG
    if (consoleOut) fclose(consoleOut);
    if (consoleErr) fclose(consoleErr);
    if (consoleIn) fclose(consoleIn);

    FreeConsole();
#endif
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

extern "C" void __declspec(dllexport) ndclient()
{
    try {
        // Không làm gì đặc biệt trong export function, chỉ trả về
    }
    catch (...) {
        // Nothing to do
    }
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD ul_reason_for_call,
    LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // Tạo thread mới để xử lý logic
        CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(&MainThread), hModule, NULL, nullptr);
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        isRunning = false;
        break;
    }

    return TRUE;
}