#pragma once

#include <windows.h>
#include "detours.h"

// Khai báo các hàm hook chính - không có __declspec(naked) trong khai báo
extern "C" {
    void VietHoaText();  // Hook tại 0x007DE7E3
    void VietHoaText1(); // Hook tại 0x007DA969
    void VietHoaText2(); // Hook tại 0x005BC364

    // Khai báo các hàm xử lý ngôn ngữ tiếng Việt
    void LoadLanguageVNTest();   // Hook tại 0x007C2517
    void LoadLanguageVNTest2();  // Hook tại 0x007C254B
    void LoadLanguageVNTestPrint(); // Hook tại 0x007CF166
    void FixKhoangCachText();    // Hook tại 0x007CDD0A
    void LoadTextVn();         // Hook tại 0x007C250D
    void PrintOutText();       // Hook tại 0x007CF166
}

// Khai báo biến global cho Unicode tiếng Việt
extern LPVOID UniCodeVN;
extern DWORD oldProtect1;

// Địa chỉ các hàm cần hook
extern DWORD VietHoaTextAdr;
extern DWORD VietHoaText1Adr;
extern DWORD VietHoaText2Adr;
extern DWORD LoadLanguageVNTestAdr;
extern DWORD LoadLanguageVNTestPrintAdr;
extern DWORD LoadLanguageVNTest2Adr;
extern DWORD FixKhoangCachTextAdr;
extern DWORD LoadVnText;
extern DWORD LoadTextVn22;

// Hàm khởi tạo các hook tiếng Việt
void InitializeVietnameseHooks(HANDLE gameHandle);

// Hàm cài đặt UniCode tiếng Việt
void InitializeVietnameseUnicode(HANDLE gameHandle);

// Cấu trúc cho extHook
class extHook {
private:
    HANDLE hProcess;
    DWORD hookAddress;
    DWORD targetAddress;
    BYTE originalBytes[10];
    BYTE hookBytes[10];
    DWORD byteCount;
    bool isHooked;

public:
    extHook(HANDLE handle, DWORD hook, DWORD target, DWORD bytes);
    ~extHook();

    void ToggleHook();
    void ToggleHook2(LPVOID lpBufferVN);
};