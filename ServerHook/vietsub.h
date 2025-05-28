// vietsub.h
#pragma once

#include <windows.h>
#include <string>
#include <unordered_map>

// Khai báo các hàm để có thể gọi từ ndclient_6.cpp
void InitializeVietHoaHooks();
void CleanupVietHoaHooks();
void ToggleVietHoa(bool enable);

// Khai báo các biến được chia sẻ
extern HANDLE handlGame;