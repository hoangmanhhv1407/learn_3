// vietsub.cpp: Chứa các hàm và các cấu trúc cần thiết cho việt hóa game
#include "pch.h"
#include "vietsub.h"
#include <windows.h>
#include <cstdint>
#include <string>
#include <unordered_map>

// Forward declaration 
void WriteLog(const char* format, ...);
extern HANDLE handlGame;

// Định nghĩa địa chỉ cho các hàm việt hóa
DWORD AcceptTiengVietAdr = 0x00422110;
DWORD DatTenTiengVietAdr = 0x004224E6;
DWORD VietHoaTextAdr = 0x007DE7E3;
DWORD VietHoaText1Adr = 0x007DA969;
DWORD VietHoaText2Adr = 0x005BC364;
DWORD LoadVnText = 0x007C250D;
DWORD LoadTextVn22 = 0x007CF166;

// Bảng chuyển đổi Unicode tiếng Việt
struct VietnameseChar {
    WORD unicodeValue;
    BYTE firstByte;
    BYTE secondByte;
};

// Bảng ánh xạ cho các ký tự Việt
std::unordered_map<WORD, VietnameseChar> vnChars = {
    // Nguyên âm có dấu - chữ thường
    {0x00E1, {0x00E1, 0xFE, 0x61}}, // á
    {0x00E0, {0x00E0, 0xFE, 0x61}}, // à
    {0x1EA3, {0x1EA3, 0xFE, 0x61}}, // ả
    {0x00E3, {0x00E3, 0xFE, 0x61}}, // ã
    {0x1EA1, {0x1EA1, 0xFE, 0x61}}, // ạ

    // Chi tiết của các ký tự khác đã bị cắt để rút gọn
    // Trong triển khai thực tế, bạn cần sao chép tất cả các ánh xạ từ file gốc vào đây
};

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

// Khai báo các hàm việt hóa
__declspec(naked)void AcceptTiengViet()  //00422110
{
    __asm
    {
        cmp eax, 0x6
        jz AcceptTiengViet_Label
        cmp eax, 0x6
        ja Adr0x00422F0F
        push 0x00422119
        ret

        AcceptTiengViet_Label :
        mov eax, 0x0
            cmp eax, 0x6
            ja Adr0x00422F0F
            push 0x00422119
            ret

            Adr0x00422F0F :
        push 0x00422F0F
            ret
    }
}

__declspec(naked)void DatTenTiengViet()  //004224E6
{
    __asm
    {
        sub esp, 0xC
        // esp count
        mov[esp + 0x4], eax
        lea ecx, [ebx + 0x00000208]
        xor eax, eax
        mov[esp + 0x8], eax
        add ecx, 0x4
        xor edi, edi
        lookbackcheckbyte :
        mov[esp], edi
            xor eax, eax
            imul edi, 0x2
            mov al, [ecx + edi]
            test eax, eax
            jz completeconvert
            xor eax, eax
            mov al, [ecx + edi + 0x1]
            cmp al, 0x01
            jz Change01
            cmp al, 0x1E
            jz Change1E
            cmp al, 0x00
            jz Bang0
            //Chi co 1 byte
            mov eax, [esp + 0x4]
            xor edx, edx
            mov edi, [esp]
            imul edi, 0x2
            mov dl, [ecx + edi]
            mov edi, [esp + 0x8]
            mov byte ptr[eax + edi], dl
            //
            mov edi, [esp]
            imul edi, 0x2
            mov dl, [ecx + edi + 0x1]
            mov edi, [esp + 0x8]
            mov byte ptr[eax + edi + 0x1], dl

            //
            mov edi, [esp + 0x8]
            add edi, 0x2
            mov[esp + 0x8], edi
            mov edi, [esp]
            add edi, 0x1
            jmp lookbackcheckbyte

            Change01 :
        mov eax, [esp + 0x4]
            xor edx, edx
            mov edi, [esp]
            imul edi, 0x2
            mov dl, [ecx + edi]
            mov edi, [esp + 0x8]
            mov byte ptr[eax + edi], 0xFE
            mov byte ptr[eax + edi + 0x1], dl
            mov edi, [esp + 0x8]
            add edi, 0x2
            mov[esp + 0x8], edi
            mov edi, [esp]
            add edi, 0x1
            jmp lookbackcheckbyte

            Change1E :
        mov eax, [esp + 0x4]
            xor edx, edx
            mov edi, [esp]
            imul edi, 0x2
            mov dl, [ecx + edi]
            mov edi, [esp + 0x8]
            mov byte ptr[eax + edi], 0xFF
            mov byte ptr[eax + edi + 0x1], dl
            mov edi, [esp + 0x8]
            add edi, 0x2
            mov[esp + 0x8], edi
            mov edi, [esp]
            add edi, 0x1
            jmp lookbackcheckbyte

            Bang0 :
        mov eax, [esp + 0x4]
            xor edx, edx
            mov edi, [esp]
            imul edi, 0x2
            mov dl, [ecx + edi]
            mov edi, [esp + 0x8]
            mov byte ptr[eax + edi], dl
            mov edi, [esp + 0x8]
            add edi, 0x1
            mov[esp + 0x8], edi
            mov edi, [esp]
            add edi, 0x1
            jmp lookbackcheckbyte

            completeconvert :
        mov eax, [esp + 0x4]
            add esp, 0xC
            mov edi, eax
            or ecx, 0xFFFFFFFF
            push 0x004224EB
            ret
    }
}

// Đây là phần sửa đổi chính cho hàm VietHoaText
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

        // THÊM VÀO: Kiểm tra ký tự trực tiếp trước khi xử lý dấu
        cmp ax, 0xFD  // Mã thô của "ư"
        jz DirectChuuFix
        cmp ax, 0xF5  // Mã thô của "ơ"
        jz DirectChuOFix
        cmp ax, 0xF0  // Mã thô của "đ"
        jz DirectChudnhoFix
        cmp ax, 0xD0  // Mã thô của "Đ"
        jz DirectChuDFix
        cmp ax, 0xE3  // Thêm kiểm tra cho ă
        jz DirectChu_a_breve_Fix

        mov ecx, [ebp + 0x00000164]
        xor edx, edx
        mov dx, [ecx]
        test edx, edx
        jz CheckAnotherText

        // Nhóm ký tự với dấu thanh/dấu câu
        cmp al, 0xEC  // Dấu sắc (đã xác nhận)
        jz CheckBase
        cmp al, 0xCC  // Dấu huyền (đã xác nhận)
        jz CheckBase
        cmp al, 0xD2  // Dấu hỏi
        jz CheckBase
        cmp al, 0xDE  // Dấu ngã
        jz CheckBase
        cmp al, 0xF2  // Dấu nặng
        jz CheckBase
        jmp Default

        // Kiểm tra ký tự cơ sở
        CheckBase :
        mov ecx, [ebp + 0x00000164]
            imul edx, 0x2
            add ecx, 0x4
            mov dx, [ecx + edx - 0x2]

            // Kiểm tra ký tự cơ sở
            cmp dx, 0x01B0  // chữ ư
            jz Handle_u_caron
            cmp dx, 0x01A1  // chữ ơ
            jz Handle_o_horn
            cmp dx, 0x00F4  // chữ ô
            jz Handle_o_circumflex
            cmp dx, 0x00EA  // chữ ê
            jz Handle_e_circumflex
            cmp dx, 0x00E2  // chữ â
            jz Handle_a_circumflex
            cmp dx, 0x0103  // chữ ă
            jz Handle_a_breve

            // Kiểm tra các nguyên âm thường
            cmp dx, 'a'
            jz Handle_a
            cmp dx, 'e'
            jz Handle_e
            cmp dx, 'i'
            jz Handle_i
            cmp dx, 'o'
            jz Handle_o
            cmp dx, 'u'
            jz Handle_u
            cmp dx, 'y'
            jz Handle_y

            // Mặc định
            jmp Default

            // Xử lý chữ ư có dấu - CHỈNH SỬA: Đảo dấu sắc/huyền
            Handle_u_caron :
        mov ecx, [esp + 0x18]
            cmp cl, 0xCC  // dấu huyền
            jnz Check_u_caron_1
            mov ax, 0x1EEB  // ừ
            jmp Return_Unicode

            Check_u_caron_1 :
        cmp cl, 0xEC  // dấu sắc
            jnz Check_u_caron_2
            mov ax, 0x1EE9  // ứ
            jmp Return_Unicode

            Check_u_caron_2 :
        cmp cl, 0xD2  // dấu hỏi
            jnz Check_u_caron_3
            mov ax, 0x1EED  // ử
            jmp Return_Unicode

            Check_u_caron_3 :
        cmp cl, 0xDE  // dấu ngã
            jnz Check_u_caron_4
            mov ax, 0x1EEF  // ữ
            jmp Return_Unicode

            Check_u_caron_4 :
        cmp cl, 0xF2  // dấu nặng
            jnz Default
            mov ax, 0x1EF1  // ự
            jmp Return_Unicode

            // Xử lý chữ ơ có dấu - CHỈNH SỬA: Đảo dấu sắc/huyền
            Handle_o_horn :
        mov ecx, [esp + 0x18]
            cmp cl, 0xCC  // dấu huyền
            jnz Check_o_horn_1
            mov ax, 0x1EDD  // ờ
            jmp Return_Unicode

            Check_o_horn_1 :
        cmp cl, 0xEC  // dấu sắc
            jnz Check_o_horn_2
            mov ax, 0x1EDB  // ớ
            jmp Return_Unicode

            Check_o_horn_2 :
        cmp cl, 0xD2  // dấu hỏi
            jnz Check_o_horn_3
            mov ax, 0x1EDF  // ở
            jmp Return_Unicode

            Check_o_horn_3 :
        cmp cl, 0xDE  // dấu ngã
            jnz Check_o_horn_4
            mov ax, 0x1EE1  // ỡ
            jmp Return_Unicode

            Check_o_horn_4 :
        cmp cl, 0xF2  // dấu nặng
            jnz Default
            mov ax, 0x1EE3  // ợ
            jmp Return_Unicode

            // Xử lý chữ ô có dấu - CHỈNH SỬA: Đảo dấu sắc/huyền
            Handle_o_circumflex :
        mov ecx, [esp + 0x18]
            cmp cl, 0xCC  // dấu huyền
            jnz Check_o_circum_1
            mov ax, 0x1ED3  // ồ
            jmp Return_Unicode

            Check_o_circum_1 :
        cmp cl, 0xEC  // dấu sắc
            jnz Check_o_circum_2
            mov ax, 0x1ED1  // ố
            jmp Return_Unicode

            Check_o_circum_2 :
        cmp cl, 0xD2  // dấu hỏi
            jnz Check_o_circum_3
            mov ax, 0x1ED5  // ổ
            jmp Return_Unicode

            Check_o_circum_3 :
        cmp cl, 0xDE  // dấu ngã
            jnz Check_o_circum_4
            mov ax, 0x1ED7  // ỗ
            jmp Return_Unicode

            Check_o_circum_4 :
        cmp cl, 0xF2  // dấu nặng
            jnz Default
            mov ax, 0x1ED9  // ộ
            jmp Return_Unicode

            // Xử lý chữ ê có dấu - CHỈNH SỬA: Đảo dấu sắc/huyền
            Handle_e_circumflex :
        mov ecx, [esp + 0x18]
            cmp cl, 0xCC  // dấu huyền
            jnz Check_e_circum_1
            mov ax, 0x1EC1  // ề
            jmp Return_Unicode

            Check_e_circum_1 :
        cmp cl, 0xEC  // dấu sắc
            jnz Check_e_circum_2
            mov ax, 0x1EBF  // ế
            jmp Return_Unicode

            Check_e_circum_2 :
        cmp cl, 0xD2  // dấu hỏi
            jnz Check_e_circum_3
            mov ax, 0x1EC3  // ể
            jmp Return_Unicode

            Check_e_circum_3 :
        cmp cl, 0xDE  // dấu ngã
            jnz Check_e_circum_4
            mov ax, 0x1EC5  // ễ
            jmp Return_Unicode

            Check_e_circum_4 :
        cmp cl, 0xF2  // dấu nặng
            jnz Default
            mov ax, 0x1EC7  // ệ
            jmp Return_Unicode

            // Xử lý chữ â có dấu - CHỈNH SỬA: Đảo dấu sắc/huyền
            Handle_a_circumflex :
        mov ecx, [esp + 0x18]
            cmp cl, 0xCC  // dấu huyền
            jnz Check_a_circum_1
            mov ax, 0x1EA7  // ầ
            jmp Return_Unicode

            Check_a_circum_1 :
        cmp cl, 0xEC  // dấu sắc
            jnz Check_a_circum_2
            mov ax, 0x1EA5  // ấ
            jmp Return_Unicode

            Check_a_circum_2 :
        cmp cl, 0xD2  // dấu hỏi
            jnz Check_a_circum_3
            mov ax, 0x1EA9  // ẩ
            jmp Return_Unicode

            Check_a_circum_3 :
        cmp cl, 0xDE  // dấu ngã
            jnz Check_a_circum_4
            mov ax, 0x1EAB  // ẫ
            jmp Return_Unicode

            Check_a_circum_4 :
        cmp cl, 0xF2  // dấu nặng
            jnz Default
            mov ax, 0x1EAD  // ậ
            jmp Return_Unicode

            // Xử lý chữ ă có dấu - CHỈNH SỬA: Đảo dấu sắc/huyền & sửa lỗi chữ ă
            Handle_a_breve :
        mov ecx, [esp + 0x18]
            cmp cl, 0xCC  // dấu huyền
            jnz Check_a_breve_1
            mov ax, 0x1EB1  // ằ
            jmp Return_Unicode

            Check_a_breve_1 :
        cmp cl, 0xEC  // dấu sắc
            jnz Check_a_breve_2
            mov ax, 0x1EAF  // ắ
            jmp Return_Unicode

            Check_a_breve_2 :
        cmp cl, 0xD2  // dấu hỏi
            jnz Check_a_breve_3
            mov ax, 0x1EB3  // ẳ
            jmp Return_Unicode

            Check_a_breve_3 :
        cmp cl, 0xDE  // dấu ngã
            jnz Check_a_breve_4
            mov ax, 0x1EB5  // ẵ
            jmp Return_Unicode

            Check_a_breve_4 :
        cmp cl, 0xF2  // dấu nặng
            jnz Default
            mov ax, 0x1EB7  // ặ
            jmp Return_Unicode

            // Xử lý chữ a thường có dấu - CHỈNH SỬA: Đảo dấu sắc/huyền
            Handle_a :
        mov ecx, [esp + 0x18]
            cmp cl, 0xCC  // dấu huyền
            jnz Check_a_1
            mov ax, 0x00E0  // à
            jmp Return_Unicode

            Check_a_1 :
        cmp cl, 0xEC  // dấu sắc
            jnz Check_a_2
            mov ax, 0x00E1  // á
            jmp Return_Unicode

            Check_a_2 :
        cmp cl, 0xD2  // dấu hỏi
            jnz Check_a_3
            mov ax, 0x1EA3  // ả
            jmp Return_Unicode

            Check_a_3 :
        cmp cl, 0xDE  // dấu ngã
            jnz Check_a_4
            mov ax, 0x00E3  // ã
            jmp Return_Unicode

            Check_a_4 :
        cmp cl, 0xF2  // dấu nặng
            jnz Default
            mov ax, 0x1EA1  // ạ
            jmp Return_Unicode

            // Xử lý chữ e thường có dấu - CHỈNH SỬA: Đảo dấu sắc/huyền
            Handle_e :
        mov ecx, [esp + 0x18]
            cmp cl, 0xCC  // dấu huyền
            jnz Check_e_1
            mov ax, 0x00E8  // è
            jmp Return_Unicode

            Check_e_1 :
        cmp cl, 0xEC  // dấu sắc
            jnz Check_e_2
            mov ax, 0x00E9  // é
            jmp Return_Unicode

            Check_e_2 :
        cmp cl, 0xD2  // dấu hỏi
            jnz Check_e_3
            mov ax, 0x1EBB  // ẻ
            jmp Return_Unicode

            Check_e_3 :
        cmp cl, 0xDE  // dấu ngã
            jnz Check_e_4
            mov ax, 0x1EBD  // ẽ
            jmp Return_Unicode

            Check_e_4 :
        cmp cl, 0xF2  // dấu nặng
            jnz Default
            mov ax, 0x1EB9  // ẹ
            jmp Return_Unicode

            // Xử lý chữ i thường có dấu - CHỈNH SỬA: Đảo dấu sắc/huyền
            Handle_i :
        mov ecx, [esp + 0x18]
            cmp cl, 0xCC  // dấu huyền
            jnz Check_i_1
            mov ax, 0x00EC  // ì
            jmp Return_Unicode

            Check_i_1 :
        cmp cl, 0xEC  // dấu sắc
            jnz Check_i_2
            mov ax, 0x00ED  // í
            jmp Return_Unicode

            Check_i_2 :
        cmp cl, 0xD2  // dấu hỏi
            jnz Check_i_3
            mov ax, 0x1EC9  // ỉ
            jmp Return_Unicode

            Check_i_3 :
        cmp cl, 0xDE  // dấu ngã
            jnz Check_i_4
            mov ax, 0x0129  // ĩ
            jmp Return_Unicode

            Check_i_4 :
        cmp cl, 0xF2  // dấu nặng
            jnz Default
            mov ax, 0x1ECB  // ị
            jmp Return_Unicode

            // Xử lý chữ o thường có dấu - CHỈNH SỬA: Đảo dấu sắc/huyền
            Handle_o :
        mov ecx, [esp + 0x18]
            cmp cl, 0xCC  // dấu huyền
            jnz Check_o_1
            mov ax, 0x00F2  // ò
            jmp Return_Unicode

            Check_o_1 :
        cmp cl, 0xEC  // dấu sắc
            jnz Check_o_2
            mov ax, 0x00F3  // ó
            jmp Return_Unicode

            Check_o_2 :
        cmp cl, 0xD2  // dấu hỏi
            jnz Check_o_3
            mov ax, 0x1ECF  // ỏ
            jmp Return_Unicode

            Check_o_3 :
        cmp cl, 0xDE  // dấu ngã
            jnz Check_o_4
            mov ax, 0x00F5  // õ
            jmp Return_Unicode

            Check_o_4 :
        cmp cl, 0xF2  // dấu nặng
            jnz Default
            mov ax, 0x1ECD  // ọ
            jmp Return_Unicode

            // Xử lý chữ u thường có dấu - CHỈNH SỬA: Đảo dấu sắc/huyền
            Handle_u :
        mov ecx, [esp + 0x18]
            cmp cl, 0xCC  // dấu huyền
            jnz Check_u_1
            mov ax, 0x00F9  // ù
            jmp Return_Unicode

            Check_u_1 :
        cmp cl, 0xEC  // dấu sắc
            jnz Check_u_2
            mov ax, 0x00FA  // ú
            jmp Return_Unicode

            Check_u_2 :
        cmp cl, 0xD2  // dấu hỏi
            jnz Check_u_3
            mov ax, 0x1EE7  // ủ
            jmp Return_Unicode

            Check_u_3 :
        cmp cl, 0xDE  // dấu ngã
            jnz Check_u_4
            mov ax, 0x0169  // ũ
            jmp Return_Unicode

            Check_u_4 :
        cmp cl, 0xF2  // dấu nặng
            jnz Default
            mov ax, 0x1EE5  // ụ
            jmp Return_Unicode

            // Xử lý chữ y thường có dấu - CHỈNH SỬA: Đảo dấu sắc/huyền
            Handle_y :
        mov ecx, [esp + 0x18]
            cmp cl, 0xCC  // dấu huyền
            jnz Check_y_1
            mov ax, 0x1EF3  // ỳ
            jmp Return_Unicode

            Check_y_1 :
        cmp cl, 0xEC  // dấu sắc
            jnz Check_y_2
            mov ax, 0x00FD  // ý
            jmp Return_Unicode

            Check_y_2 :
        cmp cl, 0xD2  // dấu hỏi
            jnz Check_y_3
            mov ax, 0x1EF7  // ỷ
            jmp Return_Unicode

            Check_y_3 :
        cmp cl, 0xDE  // dấu ngã
            jnz Check_y_4
            mov ax, 0x1EF9  // ỹ
            jmp Return_Unicode

            Check_y_4 :
        cmp cl, 0xF2  // dấu nặng
            jnz Default
            mov ax, 0x1EF5  // ỵ
            jmp Return_Unicode

            Return_Unicode :
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

            // Xử lý mặc định và các trường hợp khác
            Default :
        pop eax
            mov ecx, [esp + 0x14]
            push 0x1
            push 0x007DE7E9
            ret

            // CẢI TIẾN: Sửa lỗi chữ đ và các ký tự đặc biệt
            CheckAnotherText :
        // Kiểm tra ký tự đặc biệt
        cmp ax, 0xD0  // Đ
            jz ChuDFix
            cmp ax, 0xF0  // đ
            jz ChudnhoFix
            cmp ax, 0xF5  // ỡ
            jz ChuOFix
            cmp ax, 0xFD  // ý
            jz ChuuFix

            // Thử kiểm tra xem có phải là ă không (THÊM MỚI)
            cmp ax, 0xE3  // Nếu nhập ã nhưng thực ra là ă
            jz Fix_a_breve_special
            jmp Default

            Fix_a_breve_special :
        xor eax, eax
            mov ax, 0x0103  // ă
            pop edx
            mov ecx, [esp + 0x14]
            mov cx, ax
            push 0x1
            push 0x007DE7E9
            ret

            // THÊM MỚI: Xử lý cho lỗi đặc biệt của chữ "đ"
            ChudnhoFix :
        xor eax, eax
            mov ax, 0x0111  // đ Unicode chính xác
            // Lưu lại thông tin về ký tự đ đã được xử lý 
            // (ví dụ: có thể lưu vào một biến tạm nếu cần theo dõi)
            pop edx
            mov ecx, [esp + 0x14]
            mov cx, ax
            push 0x1
            push 0x007DE7E9
            ret

            ChuDFix :
        xor eax, eax
            mov ax, 0x0110  // Đ Unicode chính xác
            pop edx
            mov ecx, [esp + 0x14]
            mov cx, ax
            push 0x1
            push 0x007DE7E9
            ret

            ChuOFix :
        xor eax, eax
            mov ax, 0x01A1  // ơ Unicode chính xác
            pop edx
            mov ecx, [esp + 0x14]
            mov cx, ax
            push 0x1
            push 0x007DE7E9
            ret

            ChuuFix :
        xor eax, eax
            mov ax, 0x01B0  // ư Unicode chính xác
            pop edx
            mov ecx, [esp + 0x14]
            mov cx, ax
            push 0x1
            push 0x007DE7E9
            ret


            // THÊM VÀO: Labels xử lý trực tiếp
        DirectChuuFix:
        xor eax, eax
            mov ax, 0x01B0  // Unicode "ư"
            pop edx
            mov ecx, [esp + 0x14]
            mov cx, ax
            push 0x1
            push 0x007DE7E9
            ret

            DirectChuOFix :
        xor eax, eax
            mov ax, 0x01A1  // Unicode "ơ"
            pop edx
            mov ecx, [esp + 0x14]
            mov cx, ax
            push 0x1
            push 0x007DE7E9
            ret

            DirectChudnhoFix :
        xor eax, eax
            mov ax, 0x0111  // Unicode "đ"
            pop edx
            mov ecx, [esp + 0x14]
            mov cx, ax
            push 0x1
            push 0x007DE7E9
            ret

            DirectChuDFix :
        xor eax, eax
            mov ax, 0x0110  // Unicode "Đ"
            pop edx
            mov ecx, [esp + 0x14]
            mov cx, ax
            push 0x1
            push 0x007DE7E9
            ret

            DirectChu_a_breve_Fix :
        xor eax, eax
            mov ax, 0x0103  // Unicode "ă"
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

// Các biến hook việt hóa toàn cục
extHook* vietHoaTextHook = nullptr;
extHook* vietHoaText1Hook = nullptr;
extHook* vietHoaText2Hook = nullptr;
extHook* loadNDtextVnHook = nullptr;
extHook* loadNDtextVn2Hook = nullptr;
extHook* AcceptTiengVietFun = nullptr;
extHook* DatTenTiengVietFun = nullptr;

// Hàm hỗ trợ chuyển đổi ký tự Việt từ Unicode sang CP 1258
bool ConvertVietnameseChar(WORD unicodeChar, BYTE& firstByte, BYTE& secondByte) {
    auto it = vnChars.find(unicodeChar);
    if (it != vnChars.end()) {
        firstByte = it->second.firstByte;
        secondByte = it->second.secondByte;
        return true;
    }
    return false;
}

// Định nghĩa các hàm hook
// Ở đây bạn sẽ sao chép tất cả các hàm naked từ file ndclient_6.cpp
// Ví dụ: AcceptTiengViet(), DatTenTiengViet(), v.v.

// Hàm khởi tạo việt hóa
void InitializeVietHoaHooks() {
    WriteLog("InitializeVietHoaHooks: Setting up Vietnamese localization hooks");

    try {
        // Tạo các hook cho việt hóa
        vietHoaTextHook = new extHook(handlGame, (DWORD)VietHoaText, VietHoaTextAdr, 6);
        vietHoaText1Hook = new extHook(handlGame, (DWORD)VietHoaText1, VietHoaText1Adr, 6);
        vietHoaText2Hook = new extHook(handlGame, (DWORD)VietHoaText2, VietHoaText2Adr, 6);
        loadNDtextVnHook = new extHook(handlGame, (DWORD)LoadTextVn, LoadVnText, 7);
        loadNDtextVn2Hook = new extHook(handlGame, (DWORD)PrintOutText, LoadTextVn22, 5);

        // Thêm hook cho chức năng nhập tên tiếng Việt
        AcceptTiengVietFun = new extHook(handlGame, (DWORD)AcceptTiengViet, (DWORD)AcceptTiengVietAdr, 9);
        DatTenTiengVietFun = new extHook(handlGame, (DWORD)DatTenTiengViet, (DWORD)DatTenTiengVietAdr, 5);

        WriteLog("InitializeVietHoaHooks: Vietnamese localization hooks set up successfully");
    }
    catch (const std::exception& e) {
        WriteLog("InitializeVietHoaHooks: Exception caught: %s", e.what());
    }
    catch (...) {
        WriteLog("InitializeVietHoaHooks: Unknown exception caught");
    }
}

// Hàm dọn dẹp hooks việt hóa
void CleanupVietHoaHooks() {
    if (vietHoaTextHook) {
        vietHoaTextHook->Unhook();
        delete vietHoaTextHook;
        vietHoaTextHook = nullptr;
    }
    if (vietHoaText1Hook) {
        vietHoaText1Hook->Unhook();
        delete vietHoaText1Hook;
        vietHoaText1Hook = nullptr;
    }
    if (vietHoaText2Hook) {
        vietHoaText2Hook->Unhook();
        delete vietHoaText2Hook;
        vietHoaText2Hook = nullptr;
    }
    if (loadNDtextVnHook) {
        loadNDtextVnHook->Unhook();
        delete loadNDtextVnHook;
        loadNDtextVnHook = nullptr;
    }
    if (loadNDtextVn2Hook) {
        loadNDtextVn2Hook->Unhook();
        delete loadNDtextVn2Hook;
        loadNDtextVn2Hook = nullptr;
    }
    if (AcceptTiengVietFun) {
        AcceptTiengVietFun->Unhook();
        delete AcceptTiengVietFun;
        AcceptTiengVietFun = nullptr;
    }
    if (DatTenTiengVietFun) {
        DatTenTiengVietFun->Unhook();
        delete DatTenTiengVietFun;
        DatTenTiengVietFun = nullptr;
    }
}

// Bật/tắt việt hóa
void ToggleVietHoa(bool enable) {
    if (enable) {
        InitializeVietHoaHooks();
    }
    else {
        CleanupVietHoaHooks();
    }
}