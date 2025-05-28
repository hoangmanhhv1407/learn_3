#include "pch.h"
#include "viet_hoa.h"

// Khai báo địa chỉ các hàm cần hook
DWORD VietHoaTextAdr = 0x007DE7E3;
DWORD VietHoaText1Adr = 0x007DA969;
DWORD VietHoaText2Adr = 0x005BC364;
DWORD LoadLanguageVNTestAdr = 0x007C2517;
DWORD LoadLanguageVNTestPrintAdr = 0x007CF166;
DWORD LoadLanguageVNTest2Adr = 0x007C254B;
DWORD FixKhoangCachTextAdr = 0x007CDD0A;
DWORD LoadVnText = 0x007C250D;
DWORD LoadTextVn22 = 0x007CF166;

// Biến toàn cục cho UniCode tiếng Việt
LPVOID UniCodeVN = NULL;
DWORD oldProtect1 = 0;

// Cài đặt extHook
extHook::extHook(HANDLE handle, DWORD hook, DWORD target, DWORD bytes) {
    hProcess = handle;
    hookAddress = hook;
    targetAddress = target;
    byteCount = bytes;
    isHooked = false;

    // Đọc byte gốc
    ReadProcessMemory(hProcess, (LPVOID)targetAddress, originalBytes, byteCount, NULL);

    // Tạo byte hook
    hookBytes[0] = 0xE9; // JMP opcode
    *(DWORD*)(&hookBytes[1]) = (DWORD)hookAddress - targetAddress - 5;
    for (DWORD i = 5; i < byteCount; i++) {
        hookBytes[i] = 0x90; // NOP opcode
    }
}

extHook::~extHook() {
    // Restore original bytes if hooked
    if (isHooked) {
        DWORD oldProtect;
        VirtualProtectEx(hProcess, (LPVOID)targetAddress, byteCount, PAGE_EXECUTE_READWRITE, &oldProtect);
        WriteProcessMemory(hProcess, (LPVOID)targetAddress, originalBytes, byteCount, NULL);
        VirtualProtectEx(hProcess, (LPVOID)targetAddress, byteCount, oldProtect, &oldProtect);
    }
}

void extHook::ToggleHook() {
    DWORD oldProtect;
    VirtualProtectEx(hProcess, (LPVOID)targetAddress, byteCount, PAGE_EXECUTE_READWRITE, &oldProtect);

    if (!isHooked) {
        // Apply hook
        WriteProcessMemory(hProcess, (LPVOID)targetAddress, hookBytes, byteCount, NULL);
        isHooked = true;
    }
    else {
        // Remove hook
        WriteProcessMemory(hProcess, (LPVOID)targetAddress, originalBytes, byteCount, NULL);
        isHooked = false;
    }

    VirtualProtectEx(hProcess, (LPVOID)targetAddress, byteCount, oldProtect, &oldProtect);
}

void extHook::ToggleHook2(LPVOID lpBufferVN) {
    DWORD oldProtect;
    VirtualProtectEx(hProcess, (LPVOID)targetAddress, byteCount, PAGE_EXECUTE_READWRITE, &oldProtect);

    if (!isHooked) {
        // Apply hook and set buffer
        WriteProcessMemory(hProcess, (LPVOID)targetAddress, hookBytes, byteCount, NULL);
        isHooked = true;
    }
    else {
        // Remove hook
        WriteProcessMemory(hProcess, (LPVOID)targetAddress, originalBytes, byteCount, NULL);
        isHooked = false;
    }

    VirtualProtectEx(hProcess, (LPVOID)targetAddress, byteCount, oldProtect, &oldProtect);
}

// Đặt mã Unicode tiếng Việt vào bộ nhớ
void InitializeVietnameseUnicode(HANDLE gameHandle) {
    // Cấp phát bộ nhớ cho Unicode tiếng Việt
    UniCodeVN = (LPVOID)VirtualAllocEx(gameHandle, NULL, 880, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    VirtualProtectEx(gameHandle, (LPVOID)UniCodeVN, 880, PAGE_EXECUTE_READWRITE, &oldProtect1);

    // Bảng mã Unicode tiếng Việt (sẽ được copy từ đoạn mã gốc)
    BYTE ByteUniCodeCheck[866] = { 0x00,0x00,0x41,0x00,0x61,0x00,0x41,0xec,0x61,0xec,0x41,0xcc,0x61,0xcc,0x41,0xd2,0x61,0xd2,0x41,0xde,0x61,0xde,0x41,0xf2,0x61,0xf2,0xc2,0x00,0xe2,0x00,0xc2,0xec,
0xe2,0xec,0xc2,0xcc,0xe2,0xcc,0xc2,0xd2,0xe2,0xd2,0xc2,0xde,0xe2,0xde,0xc2,0xf2,0xe2,0xf2,0xc3,0x00,0xe3,0x00,0xc3,0xec,0xe3,0xec,0xc3,0xcc,0xe3,0xcc,
0xc3,0xd2,0xe3,0xd2,0xc3,0xde,0xe3,0xde,0xc3,0xf2,0xe3,0xf2,0x42,0x00,0x62,0x00,0x43,0x00,0x63,0x00,0x44,0x00,0x64,0x00,0xd0,0x00,0xf0,0x00,0x45,0x00,
0x65,0x00,0x45,0xec,0x65,0xec,0x45,0xcc,0x65,0xcc,0x45,0xd2,0x65,0xd2,0x45,0xde,0x65,0xde,0x45,0xf2,0x65,0xf2,0xca,0x00,0xea,0x00,0xca,0xec,0xea,0xec,
0xca,0xcc,0xea,0xcc,0xca,0xd2,0xea,0xd2,0xca,0xde,0xea,0xde,0xca,0xf2,0xea,0xf2,0x46,0x00,0x66,0x00,0x47,0x00,0x67,0x00,0x48,0x00,0x68,0x00,0x49,0x00,
0x69,0x00,0x49,0xec,0x69,0xec,0x49,0xcc,0x69,0xcc,0x49,0xd2,0x69,0xd2,0x49,0xde,0x69,0xde,0x49,0xf2,0x69,0xf2,0x4a,0x00,0x6a,0x00,0x4b,0x00,0x6b,0x00,
0x4c,0x00,0x6c,0x00,0x4d,0x00,0x6d,0x00,0x4e,0x00,0x6e,0x00,0x4f,0x00,0x6f,0x00,0x4f,0xec,0x6f,0xec,0x4f,0xcc,0x6f,0xcc,0x4f,0xd2,0x6f,0xd2,0x4f,0xde,
0x6f,0xde,0x4f,0xf2,0x6f,0xf2,0xd4,0x00,0xf4,0x00,0xd4,0xec,0xf4,0xec,0xd4,0xcc,0xf4,0xcc,0xd4,0xd2,0xf4,0xd2,0xd4,0xde,0xf4,0xde,0xd4,0xf2,0xf4,0xf2,
0xd5,0x00,0xf5,0x00,0xd5,0xec,0xf5,0xec,0xd5,0xcc,0xf5,0xcc,0xd5,0xd2,0xf5,0xd2,0xd5,0xde,0xf5,0xde,0xd5,0xf2,0xf5,0xf2,0x50,0x00,0x70,0x00,0x51,0x00,
0x71,0x00,0x52,0x00,0x72,0x00,0x53,0x00,0x73,0x00,0x54,0x00,0x74,0x00,0x55,0x00,0x75,0x00,0x55,0xec,0x75,0xec,0x55,0xcc,0x75,0xcc,0x55,0xd2,0x75,0xd2,
0x55,0xde,0x75,0xde,0x55,0xf2,0x75,0xf2,0xdd,0x00,0xfd,0x00,0xdd,0xec,0xfd,0xec,0xdd,0xcc,0xfd,0xcc,0xdd,0xd2,0xfd,0xd2,0xdd,0xde,0xfd,0xde,0xdd,0xf2,
0xfd,0xf2,0x56,0x00,0x76,0x00,0x57,0x00,0x77,0x00,0x58,0x00,0x78,0x00,0x59,0x00,0x79,0x00,0x59,0xec,0x79,0xec,0x59,0xcc,0x79,0xcc,0x59,0xd2,0x79,0xd2,
0x59,0xde,0x79,0xde,0x59,0xf2,0x79,0xf2,0x5a,0x00,0x7a,0x00,0x80,0x00,0x82,0x00,0x83,0x00,0x84,0x00,0x85,0x00,0x86,0x00,0x87,0x00,0x88,0x00,0x89,0x00,
0x8A,0x00,0x8B,0x00,0x8C,0x00,0x8E,0x00,0x91,0x00,0x92,0x00,0x93,0x00,0x94,0x00,0x95,0x00,0x96,0x00,0x97,0x00,0x98,0x00,0x99,0x00,0x9A,0x00,0x9B,0x00,
0x9C,0x00,0x9E,0x00,0x9F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x41,0x00,0x61,0x00,0xc1,0x00,0xe1,0x00,0xc0,0x00,0xe0,0x00,0xa2,0x1e,0xa3,0x1e,0xc3,0x00,
0xe3,0x00,0xa0,0x1e,0xa1,0x1e,0xc2,0x00,0xe2,0x00,0xa4,0x1e,0xa5,0x1e,0xa6,0x1e,0xa7,0x1e,0xa8,0x1e,0xa9,0x1e,0xaa,0x1e,0xab,0x1e,0xac,0x1e,0xad,0x1e,
0x02,0x01,0x03,0x01,0xae,0x1e,0xaf,0x1e,0xb0,0x1e,0xb1,0x1e,0xb2,0x1e,0xb3,0x1e,0xb4,0x1e,0xb5,0x1e,0xb6,0x1e,0xb7,0x1e,0x42,0x00,0x62,0x00,0x43,0x00,
0x63,0x00,0x44,0x00,0x64,0x00,0x10,0x01,0x11,0x01,0x45,0x00,0x65,0x00,0xc9,0x00,0xe9,0x00,0xc8,0x00,0xe8,0x00,0xba,0x1e,0xbb,0x1e,0xbc,0x1e,0xbd,0x1e,
0xb8,0x1e,0xb9,0x1e,0xca,0x00,0xea,0x00,0xbe,0x1e,0xbf,0x1e,0xc0,0x1e,0xc1,0x1e,0xc2,0x1e,0xc3,0x1e,0xc4,0x1e,0xc5,0x1e,0xc6,0x1e,0xc7,0x1e,0x46,0x00,
0x66,0x00,0x47,0x00,0x67,0x00,0x48,0x00,0x68,0x00,0x49,0x00,0x69,0x00,0xcd,0x00,0xed,0x00,0xcc,0x00,0xec,0x00,0xc8,0x1e,0xc9,0x1e,0x28,0x01,0x29,0x01,
0xca,0x1e,0xcb,0x1e,0x4a,0x00,0x6a,0x00,0x4b,0x00,0x6b,0x00,0x4c,0x00,0x6c,0x00,0x4d,0x00,0x6d,0x00,0x4e,0x00,0x6e,0x00,0x4f,0x00,0x6f,0x00,0xd3,0x00,
0xf3,0x00,0xd2,0x00,0xf2,0x00,0xce,0x1e,0xcf,0x1e,0xd5,0x00,0xf5,0x00,0xcc,0x1e,0xcd,0x1e,0xd4,0x00,0xf4,0x00,0xd0,0x1e,0xd1,0x1e,0xd2,0x1e,0xd3,0x1e,
0xd4,0x1e,0xd5,0x1e,0xd6,0x1e,0xd7,0x1e,0xd8,0x1e,0xd9,0x1e,0xa0,0x01,0xa1,0x01,0xda,0x1e,0xdb,0x1e,0xdc,0x1e,0xdd,0x1e,0xde,0x1e,0xdf,0x1e,0xe0,0x1e,
0xe1,0x1e,0xe2,0x1e,0xe3,0x1e,0x50,0x00,0x70,0x00,0x51,0x00,0x71,0x00,0x52,0x00,0x72,0x00,0x53,0x00,0x73,0x00,0x54,0x00,0x74,0x00,0x55,0x00,0x75,0x00,
0xda,0x00,0xfa,0x00,0xd9,0x00,0xf9,0x00,0xe6,0x1e,0xe7,0x1e,0x68,0x01,0x69,0x01,0xe4,0x1e,0xe5,0x1e,0xaf,0x01,0xb0,0x01,0xe8,0x1e,0xe9,0x1e,0xea,0x1e,
0xeb,0x1e,0xec,0x1e,0xed,0x1e,0xee,0x1e,0xef,0x1e,0xf0,0x1e,0xf1,0x1e,0x56,0x00,0x76,0x00,0x57,0x00,0x77,0x00,0x58,0x00,0x78,0x00,0x59,0x00,0x79,0x00,
0x00,0x00,0xfd,0x00,0xf2,0x1e,0xf3,0x1e,0xf6,0x1e,0xf7,0x1e,0xf8,0x1e,0xf9,0x1e,0xf4,0x1e,0xf5,0x1e,0x5a,0x00,0x7a,0x00,0xAC,0x20,0xA1,0x20,0x92,0x01,
0x1E,0x20,0x26,0x20,0x20,0x20,0x21,0x20,0xC6,0x02,0x30,0x20,0x60,0x01,0x39,0x20,0x52,0x01,0x7D,0x01,0x18,0x20,0x19,0x20,0x1C,0x20,0x1D,0x20,0x22,0x20,
0x13,0x20,0x14,0x20,0xDC,0x02,0x22,0x21,0x61,0x01,0x3A,0x20,0x53,0x01,0x7E,0x01,0x78,0x01,0x00,0x00,0x00,0x00,0x00,0x00 };

    // Patch dữ liệu Unicode vào bộ nhớ
    WriteProcessMemory(gameHandle, (LPVOID)UniCodeVN, ByteUniCodeCheck, 866, NULL);
}

// Khởi tạo các hook tiếng Việt
void InitializeVietnameseHooks(HANDLE gameHandle) {
    // Khởi tạo các hook
    extHook VietHoaTextFun(gameHandle, (DWORD)VietHoaText, (DWORD)VietHoaTextAdr, 6);
    extHook VietHoaText2Fun(gameHandle, (DWORD)VietHoaText2, (DWORD)VietHoaText2Adr, 6);
    extHook VietHoaText1Fun(gameHandle, (DWORD)VietHoaText1, (DWORD)VietHoaText1Adr, 6);

    extHook LoadLanguageVNTestFun(gameHandle, (DWORD)LoadLanguageVNTest, (DWORD)LoadLanguageVNTestAdr, 8);
    extHook LoadLanguageVNTestPrintFun(gameHandle, (DWORD)LoadLanguageVNTestPrint, (DWORD)LoadLanguageVNTestPrintAdr, 5);
    extHook LoadLanguageVNTest2Fun(gameHandle, (DWORD)LoadLanguageVNTest2, (DWORD)LoadLanguageVNTest2Adr, 11);
    extHook FixKhoangCachTextFun(gameHandle, (DWORD)FixKhoangCachText, (DWORD)FixKhoangCachTextAdr, 8);

    extHook LoadNDtextVn(gameHandle, (DWORD)LoadTextVn, (DWORD)LoadVnText, 7);
    extHook LoadNDtextVn2(gameHandle, (DWORD)PrintOutText, (DWORD)LoadTextVn22, 5);

    // Kích hoạt các hook
    VietHoaTextFun.ToggleHook();
    VietHoaText2Fun.ToggleHook();
    VietHoaText1Fun.ToggleHook();

    LoadLanguageVNTestFun.ToggleHook2(UniCodeVN);
    LoadLanguageVNTestPrintFun.ToggleHook2(UniCodeVN);
    LoadLanguageVNTest2Fun.ToggleHook();
    FixKhoangCachTextFun.ToggleHook();

    LoadNDtextVn.ToggleHook();
    LoadNDtextVn2.ToggleHook();
}

// Định nghĩa các hàm hook assembly
// Bạn sẽ copy các định nghĩa hàm hook từ mã nguồn gốc vào đây

// VietHoaText function - sẽ được định nghĩa với mã assembly trong phần copy
__declspec(naked)void VietHoaText()  //007DE7E3
{
	__asm
	{
		nop
		nop
		nop
		nop
		nop
		push eax
		mov eax, [esp + 0x18]
		mov ecx, [ebp + 0x00000164]
		xor edx, edx
		mov dx, [ecx]
		test edx, edx
		jz CheckAnotherText
		cmp ax, 0xEC
		jz ConvertText
		cmp ax, 0xCC
		jz ConvertText
		cmp ax, 0xD2
		jz ConvertText
		cmp ax, 0xDE
		jz ConvertText
		cmp ax, 0xF2
		jz ConvertText
		jmp Default

		ConvertText :
		mov ecx, [esp]
			mov[ecx + 0x1], al
			mov ecx, [ebp + 0x00000164]
			imul edx, 0x2
			add ecx, 0x4
			mov ax, [ecx + edx - 0x2]
			cmp ax, 0x1B0
			jz Chuuu2CanFix
			cmp ax, 0x1A1
			jz ChuoooCanFix
			xor eax, eax
			mov al, [ecx + edx - 0x2]
			mov ecx, [esp]
			mov[ecx], al
			xor eax, eax
			mov ax, [ecx]
			//Loop Check
			xor ecx, ecx
			LoopCheckUniCode :
		mov edx, [esp]
			add edx, 0x2
			cmp ax, [edx + ecx]
			jnz CheckAndLoop
			mov edx, [esp]
			mov ax, [edx + ecx + 0x1B0 + 0x2]
			mov ecx, [ebp + 0x00000164]
			xor edx, edx
			mov dx, [ecx]
			imul edx, 0x2
			add ecx, 0x4
			mov word ptr[ecx + edx - 0x2], 0x0
			xor edx, edx
			mov ecx, [ebp + 0x00000164]
			mov dx, [ecx]
			sub dx, 0x1
			mov[ecx], dx
			pop edx
			mov ecx, [esp + 0x14]
			mov cx, ax
			push 0x1
			push 0x007DE7E9
			ret
			CheckAndLoop :
		add ecx, 0x2
			cmp ecx, 0x1B0
			jl LoopCheckUniCode

			//


		Default:
		pop eax
			mov ecx, [esp + 0x14]
			push 0x1
			push 0x007DE7E9
			ret

			CheckAnotherText :
		cmp ax, 0xD0
			jz ChuDFix
			cmp ax, 0xF0
			jz ChudnhoFix
			cmp ax, 0xF5
			jz ChuOFix
			cmp ax, 0xFD
			jz ChuuFix
			jmp Default

			ChuDFix :
		xor eax, eax
			mov ax, 0x110
			pop edx
			mov ecx, [esp + 0x14]
			mov cx, ax
			push 0x1
			push 0x007DE7E9
			ret

			ChudnhoFix :
		xor eax, eax
			mov ax, 0x111
			pop edx
			mov ecx, [esp + 0x14]
			mov cx, ax
			push 0x1
			push 0x007DE7E9
			ret

			ChuOFix :
		xor eax, eax
			mov ax, 0x1A1
			pop edx
			mov ecx, [esp + 0x14]
			mov cx, ax
			push 0x1
			push 0x007DE7E9
			ret

			ChuuFix :
		xor eax, eax
			mov ax, 0x1B0
			pop edx
			mov ecx, [esp + 0x14]
			mov cx, ax
			push 0x1
			push 0x007DE7E9
			ret


			Chuuu2CanFix :
		mov eax, [esp + 0x18]
			cmp al, 0xEC
			jnz CheckU2
			xor eax, eax
			mov ax, 0x1ee9
			mov ecx, [ebp + 0x00000164]
			xor edx, edx
			mov dx, [ecx]
			imul edx, 0x2
			add ecx, 0x4
			mov word ptr[ecx + edx - 0x2], 0x0
			xor edx, edx
			mov ecx, [ebp + 0x00000164]
			mov dx, [ecx]
			sub dx, 0x1
			mov[ecx], dx
			pop edx
			mov ecx, [esp + 0x14]
			mov cx, ax
			push 0x1
			push 0x007DE7E9
			ret

			CheckU2 :
		cmp al, 0xCC
			jnz CheckU3
			xor eax, eax
			mov ax, 0x1eeb
			mov ecx, [ebp + 0x00000164]
			xor edx, edx
			mov dx, [ecx]
			imul edx, 0x2
			add ecx, 0x4
			mov word ptr[ecx + edx - 0x2], 0x0
			xor edx, edx
			mov ecx, [ebp + 0x00000164]
			mov dx, [ecx]
			sub dx, 0x1
			mov[ecx], dx
			pop edx
			mov ecx, [esp + 0x14]
			mov cx, ax
			push 0x1
			push 0x007DE7E9
			ret

			CheckU3 :
		cmp al, 0xD2
			jnz CheckU4
			xor eax, eax
			mov ax, 0x1eed
			mov ecx, [ebp + 0x00000164]
			xor edx, edx
			mov dx, [ecx]
			imul edx, 0x2
			add ecx, 0x4
			mov word ptr[ecx + edx - 0x2], 0x0
			xor edx, edx
			mov ecx, [ebp + 0x00000164]
			mov dx, [ecx]
			sub dx, 0x1
			mov[ecx], dx
			pop edx
			mov ecx, [esp + 0x14]
			mov cx, ax
			push 0x1
			push 0x007DE7E9
			ret

			CheckU4 :
		cmp al, 0xDE
			jnz CheckU5
			xor eax, eax
			mov ax, 0x1eef
			mov ecx, [ebp + 0x00000164]
			xor edx, edx
			mov dx, [ecx]
			imul edx, 0x2
			add ecx, 0x4
			mov word ptr[ecx + edx - 0x2], 0x0
			xor edx, edx
			mov ecx, [ebp + 0x00000164]
			mov dx, [ecx]
			sub dx, 0x1
			mov[ecx], dx
			pop edx
			mov ecx, [esp + 0x14]
			mov cx, ax
			push 0x1
			push 0x007DE7E9
			ret

			CheckU5 :
		cmp al, 0xF2
			jnz UnknowU
			xor eax, eax
			mov ax, 0x1ef1
			mov ecx, [ebp + 0x00000164]
			xor edx, edx
			mov dx, [ecx]
			imul edx, 0x2
			add ecx, 0x4
			mov word ptr[ecx + edx - 0x2], 0x0
			xor edx, edx
			mov ecx, [ebp + 0x00000164]
			mov dx, [ecx]
			sub dx, 0x1
			mov[ecx], dx
			pop edx
			mov ecx, [esp + 0x14]
			mov cx, ax
			push 0x1
			push 0x007DE7E9
			ret


			UnknowU :
		xor eax, eax
			mov ax, 0x01b0
			mov ecx, [ebp + 0x00000164]
			xor edx, edx
			mov dx, [ecx]
			imul edx, 0x2
			add ecx, 0x4
			mov word ptr[ecx + edx - 0x2], 0x0
			xor edx, edx
			mov ecx, [ebp + 0x00000164]
			mov dx, [ecx]
			sub dx, 0x1
			mov[ecx], dx
			pop edx
			mov ecx, [esp + 0x14]
			mov cx, ax
			push 0x1
			push 0x007DE7E9
			ret

			ChuoooCanFix :
		mov eax, [esp + 0x18]
			cmp al, 0xEC
			jnz CheckO2
			xor eax, eax
			mov ax, 0x1edb
			mov ecx, [ebp + 0x00000164]
			xor edx, edx
			mov dx, [ecx]
			imul edx, 0x2
			add ecx, 0x4
			mov word ptr[ecx + edx - 0x2], 0x0
			xor edx, edx
			mov ecx, [ebp + 0x00000164]
			mov dx, [ecx]
			sub dx, 0x1
			mov[ecx], dx
			pop edx
			mov ecx, [esp + 0x14]
			mov cx, ax
			push 0x1
			push 0x007DE7E9
			ret

			CheckO2 :
		cmp al, 0xCC
			jnz CheckO3
			xor eax, eax
			mov ax, 0x1edd
			mov ecx, [ebp + 0x00000164]
			xor edx, edx
			mov dx, [ecx]
			imul edx, 0x2
			add ecx, 0x4
			mov word ptr[ecx + edx - 0x2], 0x0
			xor edx, edx
			mov ecx, [ebp + 0x00000164]
			mov dx, [ecx]
			sub dx, 0x1
			mov[ecx], dx
			pop edx
			mov ecx, [esp + 0x14]
			mov cx, ax
			push 0x1
			push 0x007DE7E9
			ret

			CheckO3 :
		cmp al, 0xD2
			jnz CheckO4
			xor eax, eax
			mov ax, 0x1edf
			mov ecx, [ebp + 0x00000164]
			xor edx, edx
			mov dx, [ecx]
			imul edx, 0x2
			add ecx, 0x4
			mov word ptr[ecx + edx - 0x2], 0x0
			xor edx, edx
			mov ecx, [ebp + 0x00000164]
			mov dx, [ecx]
			sub dx, 0x1
			mov[ecx], dx
			pop edx
			mov ecx, [esp + 0x14]
			mov cx, ax
			push 0x1
			push 0x007DE7E9
			ret

			CheckO4 :
		cmp al, 0xDE
			jnz CheckO5
			xor eax, eax
			mov ax, 0x1ee1
			mov ecx, [ebp + 0x00000164]
			xor edx, edx
			mov dx, [ecx]
			imul edx, 0x2
			add ecx, 0x4
			mov word ptr[ecx + edx - 0x2], 0x0
			xor edx, edx
			mov ecx, [ebp + 0x00000164]
			mov dx, [ecx]
			sub dx, 0x1
			mov[ecx], dx
			pop edx
			mov ecx, [esp + 0x14]
			mov cx, ax
			push 0x1
			push 0x007DE7E9
			ret

			CheckO5 :
		cmp al, 0xF2
			jnz UnknowO
			xor eax, eax
			mov ax, 0x1ee3
			mov ecx, [ebp + 0x00000164]
			xor edx, edx
			mov dx, [ecx]
			imul edx, 0x2
			add ecx, 0x4
			mov word ptr[ecx + edx - 0x2], 0x0
			xor edx, edx
			mov ecx, [ebp + 0x00000164]
			mov dx, [ecx]
			sub dx, 0x1
			mov[ecx], dx
			pop edx
			mov ecx, [esp + 0x14]
			mov cx, ax
			push 0x1
			push 0x007DE7E9
			ret


			UnknowO :
		xor eax, eax
			mov ax, 0x01a1
			mov ecx, [ebp + 0x00000164]
			xor edx, edx
			mov dx, [ecx]
			imul edx, 0x2
			add ecx, 0x4
			mov word ptr[ecx + edx - 0x2], 0x0
			xor edx, edx
			mov ecx, [ebp + 0x00000164]
			mov dx, [ecx]
			sub dx, 0x1
			mov[ecx], dx
			pop edx
			mov ecx, [esp + 0x14]
			mov cx, ax
			push 0x1
			push 0x007DE7E9
			ret
	}
}
__declspec(naked)void VietHoaText1()  //007DA969
{
	__asm
	{
		jne Adr007DBA7B
		push 0x007DA96F
		ret
		Adr007DBA7B :
		push 0x007DBA7B
			ret
	}
}
__declspec(naked)void VietHoaText2()  //005BC364
{
	__asm
	{
		push esi
		xor ecx, ecx
		push ecx
		push ecx
		mov dword ptr[esp], 0x0
		mov dword ptr[esp + 0x4], 0x0  //Count So Byte da add
		BackLoopGetText:
		imul ecx, 0x2
			xor eax, eax
			mov al, [esi + ecx + 0x4]
			test eax, eax
			jz Default
			mov al, [esi + ecx + 0x1 + 0x4]
			test eax, eax
			jz ChiCoMotByte
			cmp eax, 0x1E
			jz EditCode1E
			cmp eax, 0x01
			jz EditCode10
			jmp ChiCoMotByte

			EditCode10 :
		lea eax, [esp + 0x30]
			xor edx, edx
			mov dl, [esi + ecx + 0x4]
			mov ecx, [esp + 0x4]
			mov[eax + ecx + 0x1], dl
			mov byte ptr[eax + ecx], 0xFE
			add ecx, 0x2
			mov[esp + 0x4], ecx
			mov ecx, [esp]
			add ecx, 0x1
			mov[esp], ecx
			jmp BackLoopGetText

			EditCode1E :
		lea eax, [esp + 0x30]
			xor edx, edx
			mov dl, [esi + ecx + 0x4]
			mov ecx, [esp + 0x4]
			mov[eax + ecx + 0x1], dl
			mov byte ptr[eax + ecx], 0xFF
			add ecx, 0x2
			mov[esp + 0x4], ecx
			mov ecx, [esp]
			add ecx, 0x1
			mov[esp], ecx
			jmp BackLoopGetText


			ChiCoMotByte :
		lea eax, [esp + 0x30]
			xor edx, edx
			mov dl, [esi + ecx + 0x4]
			mov ecx, [esp + 0x4]
			mov[eax + ecx], dl
			add ecx, 0x1
			mov[esp + 0x4], ecx
			mov ecx, [esp]
			add ecx, 0x1
			mov[esp], ecx
			jmp BackLoopGetText

			Default :
		pop ecx
			pop ecx
			pop esi
			lea edx, [ebx + 0x0000492C]
			mov ecx, 0x00000014
			xor eax, eax
			mov edi, edx
			repe stosd
			lea ecx, [esp + 0x24]
			push 0x005BC36A
			ret
	}
}



// LoadLanguageVNTest2 function - sẽ được định nghĩa với mã assembly trong phần copy
__declspec(naked) void LoadLanguageVNTest()   //007C2517
{
	__asm
	{
		nop
		nop
		nop
		nop
		nop
		pop edx
		lea edx, [esp + 0x91C]
		push edx
		push eax
		call ProcessConvertCode
		lea ecx, [esp + 0xDC]
		lea edx, [esp + 0x91C]
		push ecx
		push edx
		call ConvertWtoB
		lea edx, [esp + 0xDC]
		call GetCountNumberTextB
		test eax, eax
		jz KhongCoGiaTri
		add eax, 0x1
		KhongCoGiaTri:
		push 0x007C253B
			ret


			ConvertWtoB :
		sub esp, 0x8
			xor ecx, ecx
			mov[esp + 0x4], ecx
			mov[esp], ecx
			BackConvertWtoB22 :
		mov edx, [esp + 0xC]
			xor eax, eax
			mov ax, [edx + ecx]
			test eax, eax
			jz EndFuncConvertWtoB22
			cmp eax, 0xFF
			ja HaiByte
			mov edx, [esp + 0x10]
			mov ecx, [esp + 0x4]
			mov[edx + ecx], al
			add ecx, 0x1
			mov[esp + 0x4], ecx
			Jmp CheckNextCodeText22
			HaiByte :
		mov edx, [esp + 0x10]
			mov ecx, [esp + 0x4]
			mov[edx + ecx], ax
			add ecx, 0x2
			mov[esp + 0x4], ecx
			CheckNextCodeText22 :
		mov ecx, [esp]
			add ecx, 0x2
			mov[esp], ecx
			jmp BackConvertWtoB22
			EndFuncConvertWtoB22 :
		add esp, 0x8
			ret 0x8

			GetCountNumberTextB :
			xor ecx, ecx
			BackLoopCountTextB :
		xor ebx, ebx
			mov bl, [edx + ecx]
			test ebx, ebx
			jnz NextCountTextB
			mov eax, ecx
			ret

			NextCountTextB :
		add ecx, 0x1
			jmp BackLoopCountTextB


			ProcessConvertCode :
		sub esp, 0x4
			xor ecx, ecx
			mov[esp], ecx
			BackLoopConvertText :
		mov edx, [esp + 0xC]
			xor eax, eax
			mov ax, [edx + ecx]
			test eax, eax
			jz CompleteConvertText
			push eax
			mov eax, [esp + 0xC]
			push eax
			call GetTextConvert
			mov edx, [esp + 0xC]
			mov ecx, [esp]
			mov[edx + ecx], ax
			CheckNextText :
		mov ecx, [esp]
			add ecx, 0x2
			mov[esp], ecx
			jmp BackLoopConvertText
			CompleteConvertText :
		add esp, 0x4
			ret 0x8

			GetTextConvert :
			xor ecx, ecx
			LoopBackCheckText :
		mov eax, [esp + 0x8]
			mov edx, [esp + 0x4]
			xor ebx, ebx
			mov bx, [edx + ecx + 0x1B2]
			cmp eax, ebx
			jnz CheckNextTextConvert
			xor eax, eax
			mov ax, [edx + ecx + 0x2]
			jmp EndFunCoonvet
			CheckNextTextConvert :
		add ecx, 0x2
			cmp ecx, 0x1AC
			jl LoopBackCheckText
			mov eax, [esp + 0x8]
			EndFunCoonvet :
			ret 0x8
	}
}
__declspec(naked) void LoadLanguageVNTest2()   //007C254B
{
	__asm
	{
		push 0x00891D1C
		mov eax, 0x7918E0
		call eax
		add esp, 0x4
		mov ecx, [esp + 0x14]
		lea edi, [esp + 0xE0]
		push 0x007C2556
		ret
	}
}
__declspec(naked) void LoadLanguageVNTestPrint()   //007CF166
{
	__asm
	{
		nop
		nop
		nop
		nop
		nop
		pushad
		sub esp, 0x8
		xor ecx, ecx
		mov[esp], ecx
		mov[esp + 0x4], ecx
		BackConvertText :
		xor eax, eax
			mov al, [esi + ecx]
			test eax, eax
			jz Default
			test ecx, ecx
			jz KhongCanKiemTraLoi
			mov ecx, [esp]
			xor eax, eax
			mov al, [esi + ecx - 0x1]
			test eax, eax
			jz Default
			KhongCanKiemTraLoi :
		xor eax, eax
			mov ecx, [esp]
			mov ax, [esi + ecx]
			push eax
			mov eax, [esp + 0x28]
			push eax
			call GetASIIText
			test eax, eax
			jnz Byte2CodeConvert
			xor eax, eax
			mov ecx, [esp]
			mov al, [esi + ecx]
			push eax
			mov eax, [esp + 0x28]
			push eax
			call GetASIIText
			test eax, eax
			jnz NextCvTextStep
			xor eax, eax
			mov ecx, [esp]
			mov al, [esi + ecx]
			NextCvTextStep:
		mov edi, 0x01774788
			mov ecx, [esp + 0x4]
			imul ecx, 0x2
			mov[edi + ecx], ax
			mov ecx, [esp + 0x4]
			add ecx, 0x1
			mov[esp + 0x4], ecx
			mov ecx, [esp]
			add ecx, 0x1
			mov[esp], ecx
			jmp BackConvertText

			Default :
		add esp, 0x8
			popad
			push 0x007CF18D
			ret

			Byte2CodeConvert :
		mov edi, 0x01774788
			mov ecx, [esp + 0x4]
			imul ecx, 0x2
			mov[edi + ecx], ax
			mov ecx, [esp + 0x4]
			add ecx, 0x1
			mov[esp + 0x4], ecx
			mov ecx, [esp]
			add ecx, 0x2
			mov[esp], ecx
			jmp BackConvertText

			GetASIIText :
		sub esp, 0x4
			xor ecx, ecx
			mov[esp], ecx
			LoopBackGetASII :
		mov eax, [esp + 0xC]
			mov ebx, [esp + 0x8]
			xor edx, edx
			mov dx, [ebx + ecx + 0x2]
			cmp ax, dx
			jnz CheckGetASII
			xor edx, edx
			mov dx, [ebx + ecx + 0x1B2]
			mov eax, edx
			add esp, 0x4
			ret 0x8
			CheckGetASII:
		mov ecx, [esp]
			add ecx, 0x2
			mov[esp], ecx
			cmp ecx, 0x1AC
			jbe LoopBackGetASII
			mov eax, 0x00
			add esp, 0x4
			ret 0x8
	}
};
__declspec(naked)void FixKhoangCachText()  //007CDD0A
{
	__asm
	{
		cmp ax, 0x00C0
		jz KhongTangThemSpace
		cmp ax, 0x00C1
		jz KhongTangThemSpace
		cmp ax, 0x00C3
		jz KhongTangThemSpace
		cmp ax, 0x00C4
		jz KhongTangThemSpace
		cmp ax, 0x00C5
		jz KhongTangThemSpace
		cmp ax, 0x00C6
		jz KhongTangThemSpace
		cmp ax, 0x00C7
		jz KhongTangThemSpace
		cmp ax, 0x00C8
		jz KhongTangThemSpace
		cmp ax, 0x00C9
		jz KhongTangThemSpace
		cmp ax, 0x00CB
		jz KhongTangThemSpace
		cmp ax, 0x00CC
		jz KhongTangThemSpace
		cmp ax, 0x00CD
		jz KhongTangThemSpace
		cmp ax, 0x00CE
		jz KhongTangThemSpace
		cmp ax, 0x00CF
		jz KhongTangThemSpace
		cmp ax, 0x00D1
		jz KhongTangThemSpace
		cmp ax, 0x00D2
		jz KhongTangThemSpace
		cmp ax, 0x00D3
		jz KhongTangThemSpace
		cmp ax, 0x00D5
		jz KhongTangThemSpace
		cmp ax, 0x00D6
		jz KhongTangThemSpace
		cmp ax, 0x00D7
		jz KhongTangThemSpace
		cmp ax, 0x00D8
		jz KhongTangThemSpace
		cmp ax, 0x00D9
		jz KhongTangThemSpace
		cmp ax, 0x00DA
		jz KhongTangThemSpace
		cmp ax, 0x00DB
		jz KhongTangThemSpace
		cmp ax, 0x00DC
		jz KhongTangThemSpace
		cmp ax, 0x00DD
		jz KhongTangThemSpace
		cmp ax, 0x00DE
		jz KhongTangThemSpace
		cmp ax, 0x00DF
		jz KhongTangThemSpace
		cmp ax, 0x00E0
		jz KhongTangThemSpace
		cmp ax, 0x00E1
		jz KhongTangThemSpace
		cmp ax, 0x00E4
		jz KhongTangThemSpace
		cmp ax, 0x00E5
		jz KhongTangThemSpace
		cmp ax, 0x00E6
		jz KhongTangThemSpace
		cmp ax, 0x00E7
		jz KhongTangThemSpace
		cmp ax, 0x00E8
		jz KhongTangThemSpace
		cmp ax, 0x00E9
		jz KhongTangThemSpace
		cmp ax, 0x00EB
		jz KhongTangThemSpace
		cmp ax, 0x00EC
		jz KhongTangThemSpace
		cmp ax, 0x00ED
		jz KhongTangThemSpace
		cmp ax, 0x00EE
		jz KhongTangThemSpace
		cmp ax, 0x00EF
		jz KhongTangThemSpace
		cmp ax, 0x00F0
		jz KhongTangThemSpace
		cmp ax, 0x00F1
		jz KhongTangThemSpace
		cmp ax, 0x00F2
		jz KhongTangThemSpace
		cmp ax, 0x00F3
		jz KhongTangThemSpace
		cmp ax, 0x00F6
		jz KhongTangThemSpace
		cmp ax, 0x00F8
		jz KhongTangThemSpace
		cmp ax, 0x00F9
		jz KhongTangThemSpace
		cmp ax, 0x00FA
		jz KhongTangThemSpace
		cmp ax, 0x00FB
		jz KhongTangThemSpace
		cmp ax, 0x00FC
		jz KhongTangThemSpace
		cmp ax, 0x00FE
		jz KhongTangThemSpace
		cmp ax, 0x00FF
		jz KhongTangThemSpace
		push eax
		mov ecx, ebx
		mov eax, 0x007D4910
		call eax
		push 0x007CDD12
		ret

		KhongTangThemSpace :
		mov eax, 0
			push 0x007CDD12
			ret
	}
}

// LoadTextVn function - sẽ được định nghĩa với mã assembly trong phần copy
__declspec(naked) void LoadTextVn()
{
	__asm
	{
		lea edi, [esp + 0xDC]
		pushad
		repe stosd
		lea edi, [esp + 0x93C]//93C
		mov ecx, [edi]
		test ecx, ecx
		jle OutRead
		or ecx, 0xFFFFFFFF
		xor eax, eax
		repne scasw
		not ecx
		add ecx, ecx
		sub edi, ecx
		lea esi, [esp + 0xFC]//FC
		mov edx, 0x0
		mov ebp, 0x0 //default

		LoopBack:
		mov  al, [edi + edx] // c-a-c
			cmp al, 0x00
			je Loop1
			//Default
			mov bl, [edi + edx + 1]
			cmp bl, 0x1E  // FF
			je Jump1E

			cmp bl, 0x01  // FE
			je Jump01
			//end
			Default ://0x00
		mov[esi + ebp], al
			add edx, 0x2
			inc ebp
			jmp LoopBack

			//Default
			Jump01 : //0x01
		mov BYTE PTR SS : [esi + ebp] , 0xFE
			mov BYTE PTR SS : [esi + ebp + 1] , al
			add edx, 0x2
			add ebp, 0x2
			jmp LoopBack

			Jump1E :
		mov BYTE PTR SS : [esi + ebp] , 0xFF
			mov BYTE PTR SS : [esi + ebp + 1] , al
			add edx, 0x2
			add ebp, 0x2
			jmp LoopBack
			//end
			Loop1 : //OutLoop
		popad
			mov ecx, 0x00000000
			xor ecx, 0xFFFFFFFF
			lea edi, [esp + 0xDC]//DC
			repne scasb
			not ecx
			mov eax, ecx
			mov ebp, 0x007C253B
			jmp ebp
			/*ret 0x20*/
			OutRead :
		popad
			mov eax, 0x0
			mov ebp, 0x007C253B
			jmp ebp
			/*ret 0x20*/
	}
}
__declspec(naked) void PrintOutText()
{
	__asm
	{
		pushad
		mov edi, esi
		mov ecx, [edi]
		cmp ecx, 0x0
		je OutRead
		or ecx, 0xFFFFFFFF
		xor eax, eax
		repne scasb
		not ecx
		sub edi, ecx
		mov esi, 0x01774788
		mov edx, 0x0
		mov ebp, 0x0

		LoopBack:
		mov al, [edi + edx] // c-a-c
			cmp al, 0x00
			jz Loop1
			//default
			cmp al, 0xFF
			je Default1E

			cmp al, 0xFE
			je Default01

			Default00 :
		mov BYTE PTR SS : [esi + ebp] , al
			mov BYTE PTR SS : [esi + ebp + 1] , 0x00
			inc edx
			add ebp, 0x2
			jmp LoopBack

			Default01 :
		mov al, [edi + edx + 1]
			mov BYTE PTR SS : [esi + ebp] , al
			mov BYTE PTR SS : [esi + ebp + 1] , 0x01
			add edx, 0x2
			add ebp, 0x2
			jmp LoopBack

			Default1E :
		mov al, [edi + edx + 1]
			mov BYTE PTR SS : [esi + ebp] , al
			mov BYTE PTR SS : [esi + ebp + 1] , 0x1E
			add edx, 0x2
			add ebp, 0x2
			jmp LoopBack

			Loop1 :
		popad
			mov ecx, 0x007CF18D
			jmp ecx
			/*ret 0x20*/
			OutRead :
		popad
			mov ecx, 0x007CF18D
			jmp ecx
	}
}