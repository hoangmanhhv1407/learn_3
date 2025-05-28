// Cập nhật ndgm.h
#pragma once

#include <windows.h>
#include "detours.h"

// Khai báo các biến bên ngoài
extern HANDLE handlGame;

// Định nghĩa địa chỉ GM Command
#define AddressGmCommand 0x005A9BA5
#define HienThiThanhGMToolsAdr 0x0065F9F9
#define SendPacketAdr 0x006B3FF8

// Kiểu con trỏ hàm cho các hàm GM
typedef void (*_GMCommand_t)();
typedef void (*_HienThiThanhGMTools_t)();
typedef void (*_SendPacketNetWord_t)();

extern _GMCommand_t original_GMCommand;
extern _HienThiThanhGMTools_t original_HienThiThanhGMTools;
extern _SendPacketNetWord_t original_SendPacketNetWord;

// Khai báo các hàm GM
void GMCommand();
void HienThiThanhGMTools();
void SendPackedNetWord();

// Hàm khởi tạo các hook GM
void InitializeGMHooks();

// Hàm dọn dẹp các hook GM
void CleanupGMHooks();