// ndlog.h - Header cho module log hệ thống
#pragma once

#include <windows.h>
#include <string>
#include <cstdarg>
#include <chrono>
#include <vector>
#include <mutex>

// Định nghĩa cấp độ log
enum LogLevel {
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARNING = 2,
    LOG_ERROR = 3,
    LOG_CRITICAL = 4
};

// Class quản lý log
class NDLogger {
private:
    static FILE* logFile;
    static bool consoleEnabled;
    static LogLevel minLevel;
    static std::mutex logMutex;
    static std::vector<std::string> logBuffer;
    static size_t maxBufferSize;
    static std::chrono::system_clock::time_point startTime;
    static std::string logFilePath;

    // Tạo singleton
    static NDLogger& getInstance();

    NDLogger();
    ~NDLogger();

    // Không cho phép sao chép
    NDLogger(const NDLogger&) = delete;
    NDLogger& operator=(const NDLogger&) = delete;

public:
    // Khởi tạo và dọn dẹp logger
    static bool Initialize(const char* logFilePath = "ndclient_debug.log", bool enableConsole = true, LogLevel level = LOG_DEBUG);
    static void Shutdown();

    // Ghi log với cấp độ
    static void Log(LogLevel level, const char* format, ...);

    // Các wrapper cho từng cấp độ
    static void Debug(const char* format, ...);
    static void Info(const char* format, ...);
    static void Warning(const char* format, ...);
    static void Error(const char* format, ...);
    static void Critical(const char* format, ...);

    // Ghi log cho exception
    static void LogException(EXCEPTION_POINTERS* pExceptionPointers);

    // Tiện ích memory dump
    static void DumpMemory(void* address, size_t size, const char* description = nullptr);

    // Đặt cấp độ log tối thiểu
    static void SetLogLevel(LogLevel level);

    // Bật/tắt log ra console
    static void EnableConsole(bool enable);

    // Flush buffer
    static void FlushBuffer();

    // Lấy thời gian từ khi khởi động
    static double GetElapsedTime();

    // Lấy đường dẫn log hiện tại
    static const std::string& GetLogFilePath();
};

// Định nghĩa macro tắt
#ifndef NDEBUG
#define ND_LOG_DEBUG(...)     NDLogger::Debug(__VA_ARGS__)
#define ND_LOG_INFO(...)      NDLogger::Info(__VA_ARGS__)
#define ND_LOG_WARNING(...)   NDLogger::Warning(__VA_ARGS__)
#define ND_LOG_ERROR(...)     NDLogger::Error(__VA_ARGS__)
#define ND_LOG_CRITICAL(...)  NDLogger::Critical(__VA_ARGS__)
#define ND_LOG_MEMORY(addr, size, desc) NDLogger::DumpMemory(addr, size, desc)
#else
#define ND_LOG_DEBUG(...)
#define ND_LOG_INFO(...)
#define ND_LOG_WARNING(...)
#define ND_LOG_ERROR(...)
#define ND_LOG_CRITICAL(...)
#define ND_LOG_MEMORY(...)
#endif

// Hoạt động như WriteLog cũ để tương thích ngược
void WriteLog(const char* format, ...);