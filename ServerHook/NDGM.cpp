// Cập nhật ndgm.cpp
#include "pch.h"
#include "NDGM.h"
#include "ndlog.h"

// Khai báo con trỏ hàm gốc
_GMCommand_t original_GMCommand = nullptr;
_HienThiThanhGMTools_t original_HienThiThanhGMTools = nullptr;
_SendPacketNetWord_t original_SendPacketNetWord = nullptr;

// Hàm GMCommand - giữ nguyên code assembly của bạn
__declspec(naked) void GMCommand()
{
    __asm
    {
        pushad
        push 0x2768
        mov ecx, 0x017323A8
        mov eax, 0x7D8550
        call eax
        test eax, eax
        jz OutFunc
        mov ecx, eax
        mov eax, [esp + 0x4]

        cmp eax, 0x14B4
        jne _XDEFLOSEBTN
        mov eax, [ecx + 0x208]
        test eax, eax
        jz UnOpenCommand
        push 0x0
        mov eax, 0x455470
        call eax
        jmp OutFunc
        UnOpenCommand :
        push 0x1
            mov eax, 0x455470
            call eax
            jmp OutFunc

            _XDEFLOSEBTN :
        cmp eax, 0x14C9
            jne _XDEFLOSEBTN0
            push 0x2769
            mov ecx, 0x017323A8
            mov eax, 0x7D8550
            call eax
            push    0
            push    0
            push    1
            push    9
            push    0xFA
            mov     ecx, 0x0155EE60
            mov     eax, 0x6B3170
            call    eax
            jmp OutFunc
            _XDEFLOSEBTN0 :
        cmp eax, 0x1468
            jne _XDEFLOSEBTN1
            push 0x2769
            mov ecx, 0x017323A8
            mov eax, 0x7D8550
            call eax
            mov esi, eax
            mov  edi, [esi]
            mov  ecx, esi
            call dword ptr[edi + 0x3C]
            neg  eax
            sbb  eax, eax
            mov  ecx, esi
            inc  eax
            push eax
            call dword ptr[edi + 0x30]
            jmp OutFunc
            _XDEFLOSEBTN1 :
        cmp eax, 0x1469
            jne _XDEFLOSEBTN5
            push 0x276A
            mov ecx, 0x017323A8
            mov eax, 0x7D8550
            call eax
            mov esi, eax
            mov  edi, [esi]
            mov  ecx, esi
            call dword ptr[edi + 0x3C]
            neg  eax
            sbb  eax, eax
            mov  ecx, esi
            inc  eax
            push eax
            call dword ptr[edi + 0x30]
            jmp OutFunc
            _XDEFLOSEBTN5 :
        cmp eax, 0x146D
            jne _XDEFCHARACTERCOORDSETTIClose
            push 0x276E
            mov ecx, 0x017323A8
            mov eax, 0x7D8550
            call eax
            mov esi, eax
            mov  edi, [esi]
            mov  ecx, esi
            call dword ptr[edi + 0x3C]
            neg  eax
            sbb  eax, eax
            mov  ecx, esi
            inc  eax
            push eax
            call dword ptr[edi + 0x30]
            jmp OutFunc
            _XDEFCHARACTERCOORDSETTIClose :
        cmp eax, 0x14D2
            jne GMCHARACTERCOORDSETTINGGMMOVE
            push 0x276A
            mov ecx, 0x017323A8
            mov eax, 0x7D8550
            call eax
            mov  edx, [eax]
            push 0
            mov  ecx, eax
            call dword ptr[edx + 0x30]
            jmp OutFunc
            GMCHARACTERCOORDSETTINGGMMOVE :
        cmp eax, 0x14D4
            jne GMCHARACTERCOORDSETTINGGMMOVETOUSER
            push 0x276A
            mov ecx, 0x017323A8
            mov eax, 0x7D8550
            call eax
            push    0
            push    0
            push    0
            push    1
            push    0xFC
            mov     ecx, 0x0155EE60
            mov     eax, 0x6B3170
            call    eax
            jmp OutFunc
            GMCHARACTERCOORDSETTINGGMMOVETOUSER :
        cmp eax, 0x14D5
            jne GMWINDOW_COMMANDBUTTON6
            push 0x276A
            mov ecx, 0x017323A8
            mov eax, 0x7D8550
            call eax
            push    0
            push    0
            push    0
            push    0
            push    0xFC
            mov     ecx, 0x0155EE60
            mov     eax, 0x6B3170
            call    eax
            jmp OutFunc
            GMWINDOW_COMMANDBUTTON6 :
        cmp eax, 0x146E
            jne GMWINDOW_COMMANDBUTTON7
            push 0x276F
            mov ecx, 0x017323A8
            mov eax, 0x7D8550
            call eax
            mov esi, eax
            mov  edi, [esi]
            mov  ecx, esi
            call dword ptr[edi + 0x3C]
            neg  eax
            sbb  eax, eax
            mov  ecx, esi
            inc  eax
            push eax
            call dword ptr[edi + 0x30]
            jmp OutFunc
            GMWINDOW_COMMANDBUTTON7 :
        cmp eax, 0x146F
            jne GMMONSTERSPAWNBUTTON
            push 0x2770
            mov ecx, 0x017323A8
            mov eax, 0x7D8550
            call eax
            mov esi, eax
            mov  edi, [esi]
            mov  ecx, esi
            call dword ptr[edi + 0x3C]
            neg  eax
            sbb  eax, eax
            mov  ecx, esi
            inc  eax
            push eax
            call dword ptr[edi + 0x30]
            jmp OutFunc
            GMMONSTERSPAWNBUTTON :
        cmp eax, 0x1505
            jne OutFunc
            push 0x276F
            mov ecx, 0x017323A8
            mov eax, 0x7D8550
            call eax
            mov esi, eax
            lea ecx, [esi + 0x208]
            mov eax, 0x7DA6F0
            call eax
            push eax
            mov eax, 0x0085C520
            call dword ptr[eax]
            add esp, 0x4
            push    0
            push    0
            push    eax
            mov     eax, [esi + 0x4AA0]
            push    eax
            push    0xF8
            mov     ecx, 0x0155EE60
            mov eax, 0x6B3170
            call    eax

            OutFunc :
        popad
            pop ecx
            pop ecx
            push 0x005A9BAA
            ret
    }
}

// Thêm HienThiThanhGMTools từ 
__declspec(naked)void HienThiThanhGMTools()  //0065F9F9
{
    __asm {
        mov eax, [edx + 0x000082BC]
        xor eax, 0xAE1CDB23
        jle Default
        push 0x2768
        mov ecx, 0x017323A8
        mov eax, 0x007D8550
        call eax
        test eax, eax
        jz Default
        mov ecx, [eax + 0x10]
        test ecx, ecx
        jnz Default
        mov dword ptr[eax + 0x10], 1

        Default:
        mov edx, 0x1107258
            mov eax, [edx + 0x00009D88]
            push 0x0065F9FF
            ret
    }
}

// Thêm SendPackedNetWord từ 
__declspec(naked)void SendPackedNetWord()
{
    __asm
    {
        cmp ecx, 0xF0
        jz RunSendGM
        cmp ecx, 0xF2
        jz Invisibility
        pop edi
        pop esi
        pop ebp
        pop ebx
        mov eax, 0x06B3FFD
        jmp eax
        RunSendGM :
        mov ecx, [esp + 0x8E4]
            xor edx, edx
            mov eax, edx
            mov[esp + 0x18], eax
            xor eax, 0xE
            mov[esp + 0x1C], edx
            mov[esp + 0x18], ax
            mov eax, [0x10D94A8]
            mov[esp + 0x20], edx
            mov byte ptr[esp + 0x1A], 0xF8
            mov[esp + 0x24], dx
            mov edx, [eax + 0x83D4]
            mov[esp + 0x1D], edx
            mov eax, [0x10D94A8]
            mov eax, [eax + 0x83DC]
            mov[esp + 0x21], eax
            add ecx, 0xFA0
            mov[esp + 0x1B], cx
            mov byte ptr[esp + 0x25], 0x1
            lea ecx, [esp + 0x18]
            push ecx
            mov ecx, ebp
            mov eax, 0x6B2F60
            call eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            mov eax, 0x06B3FFD
            jmp eax

            Invisibility :
        mov edx, [esp + 0x8E4]
            xor edx, edx
            mov eax, edx
            mov[esp + 0x18], eax
            xor eax, 0x23
            mov[esp + 0x1C], edx
            mov[esp + 0x18], ax
            mov byte ptr[esp + 0x1A], 0xFA
            mov byte ptr[esp + 0x1B], 0x9
            push 0x2769
            mov ecx, 0x017323A8
            mov eax, 0x7D8550
            call eax
            test eax, eax
            jz KhongTimThayGMForm
            mov eax, [eax + 0x208]
            mov cl, [eax + 0x30]
            test cl, cl
            jz TatCheDoTangHinh
            mov cl, 1
            mov[esp + 0x1C], cl
            jmp ProcessHienThi
            TatCheDoTangHinh :
        mov cl, 0
            mov[esp + 0x1C], cl
            jmp ProcessHienThi
            KhongTimThayGMForm :
        mov dword ptr[esp + 0x1C], 0
            ProcessHienThi :
            lea ecx, [esp + 0x18]
            push ecx
            mov ecx, ebp
            mov eax, 0x6B2F60
            call eax
            pop edi
            pop esi
            pop ebp
            pop ebx
            mov eax, 0x06B3FFD
            jmp eax

    }
}

// Cập nhật hàm khởi tạo các hook GM
void InitializeGMHooks()
{
    ND_LOG_INFO("InitializeGMHooks: Setting up GM hooks");

    try {
        // Sử dụng Detours để hook GMCommand
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        // Thiết lập hook GM Command
        original_GMCommand = (_GMCommand_t)AddressGmCommand;
        DetourAttach(&(PVOID&)original_GMCommand, GMCommand);

        // Thêm hook cho HienThiThanhGMTools
        original_HienThiThanhGMTools = (_HienThiThanhGMTools_t)HienThiThanhGMToolsAdr;
        DetourAttach(&(PVOID&)original_HienThiThanhGMTools, HienThiThanhGMTools);

        // Thêm hook cho SendPacketNetWord
        //original_SendPacketNetWord = (_SendPacketNetWord_t)SendPacketAdr;
        //DetourAttach(&(PVOID&)original_SendPacketNetWord, SendPackedNetWord);

        // Commit transaction
        LONG result = DetourTransactionCommit();
        if (result != NO_ERROR) {
            ND_LOG_ERROR("InitializeGMHooks: Error setting up GM hooks: %d", result);
        }
        else {
            ND_LOG_INFO("InitializeGMHooks: GM hooks set up successfully");
        }
    }
    catch (const std::exception& e) {
        ND_LOG_ERROR("InitializeGMHooks: Exception caught: %s", e.what());
    }
    catch (...) {
        ND_LOG_ERROR("InitializeGMHooks: Unknown exception caught");
    }
}

// Cập nhật hàm dọn dẹp các hook GM
void CleanupGMHooks()
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    if (original_GMCommand) {
        DetourDetach(&(PVOID&)original_GMCommand, GMCommand);
        original_GMCommand = nullptr;
    }

    if (original_HienThiThanhGMTools) {
        DetourDetach(&(PVOID&)original_HienThiThanhGMTools, HienThiThanhGMTools);
        original_HienThiThanhGMTools = nullptr;
    }

//    if (original_SendPacketNetWord) {
//        DetourDetach(&(PVOID&)original_SendPacketNetWord, SendPackedNetWord);
//        original_SendPacketNetWord = nullptr;
//    }

    DetourTransactionCommit();
    ND_LOG_INFO("CleanupGMHooks: GM hooks cleaned up");
}