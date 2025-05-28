// ndlog.cpp - Triển khai hệ thống log nâng cao
#include "pch.h"
#include "ndlog.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <cstdio>
// Thêm các include cần thiết
#include <Psapi.h>  // Cho GetModuleInformation
#include <DbgHelp.h> // Cho các hàm StackWalk64, SymFunctionTableAccess64, v.v.

#pragma comment(lib, "Dbghelp.lib") // Link với thư viện DbgHelp

// Khởi tạo các biến static
FILE* NDLogger::logFile = nullptr;
bool NDLogger::consoleEnabled = true;
LogLevel NDLogger::minLevel = LOG_DEBUG;
std::mutex NDLogger::logMutex;
std::vector<std::string> NDLogger::logBuffer;
size_t NDLogger::maxBufferSize = 100;
std::chrono::system_clock::time_point NDLogger::startTime = std::chrono::system_clock::now();
std::string NDLogger::logFilePath = "ndclient_debug.log";

// Singleton
NDLogger& NDLogger::getInstance() {
    static NDLogger instance;
    return instance;
}

// Constructor
NDLogger::NDLogger() {
    // RAII sẽ được xử lý bởi Initialize() và Shutdown()
}

// Destructor
NDLogger::~NDLogger() {
    Shutdown();
}

bool NDLogger::Initialize(const char* path, bool enableConsole, LogLevel level) {
    std::lock_guard<std::mutex> lock(logMutex);

    // Thử tạo thư mục chứa log nếu chưa tồn tại
    try {
        // Thay thế std::filesystem bằng các API Windows tiêu chuẩn
        char dirPath[MAX_PATH] = { 0 };
        strcpy_s(dirPath, path);

        // Tìm vị trí ký tự '\' cuối cùng để lấy thư mục
        char* lastSlash = strrchr(dirPath, '\\');
        if (lastSlash) {
            *lastSlash = '\0'; // Cắt tên file, giữ lại chỉ đường dẫn
            // Tạo thư mục nếu cần
            CreateDirectoryA(dirPath, NULL);
        }
    }
    catch (const std::exception& e) {
        if (enableConsole) {
            std::cerr << "Failed to create log directory: " << e.what() << std::endl;
        }
    }

    // Khởi tạo file log
    if (logFile) {
        fclose(logFile);
        logFile = nullptr;
    }

    logFilePath = path;
    logFile = fopen(path, "a");
    if (!logFile) {
        if (enableConsole) {
            std::cerr << "Failed to open log file: " << path << std::endl;
        }
        return false;
    }

    consoleEnabled = enableConsole;
    minLevel = level;
    startTime = std::chrono::system_clock::now();

    // Ghi header khi bắt đầu log mới
    time_t now = time(0);
    char timeStr[100];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&now));
    fprintf(logFile, "\n\n========== LOG SESSION STARTED AT %s ==========\n\n", timeStr);

    // Ghi thông tin hệ thống
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);

    fprintf(logFile, "System Information:\n");
    fprintf(logFile, "- Processor Architecture: %d\n", sysInfo.wProcessorArchitecture);
    fprintf(logFile, "- Number of Processors: %d\n", sysInfo.dwNumberOfProcessors);
    fprintf(logFile, "- Page Size: %d bytes\n", sysInfo.dwPageSize);

    // Lấy thông tin Windows version
    OSVERSIONINFOEX osInfo;
    ZeroMemory(&osInfo, sizeof(OSVERSIONINFOEX));
    osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    typedef LONG(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (hNtdll) {
        auto pRtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hNtdll, "RtlGetVersion");
        if (pRtlGetVersion) {
            pRtlGetVersion((PRTL_OSVERSIONINFOW)&osInfo);
            fprintf(logFile, "- Windows Version: %d.%d.%d (Build %d)\n",
                osInfo.dwMajorVersion, osInfo.dwMinorVersion,
                osInfo.dwBuildNumber, osInfo.wServicePackMajor);
        }
    }

    fprintf(logFile, "\nLog level set to: %d\n", minLevel);
    fprintf(logFile, "Console output: %s\n\n", consoleEnabled ? "Enabled" : "Disabled");
    fflush(logFile);

    return true;
}

void NDLogger::Shutdown() {
    std::lock_guard<std::mutex> lock(logMutex);

    // Flush buffer nếu còn dữ liệu
    FlushBuffer();

    if (logFile) {
        // Ghi footer khi kết thúc session
        time_t now = time(0);
        char timeStr[100];
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&now));
        fprintf(logFile, "\n\n========== LOG SESSION ENDED AT %s ==========\n", timeStr);

        // Tính thời gian chạy
        double elapsedTime = GetElapsedTime();
        fprintf(logFile, "Session duration: %.3f seconds\n\n", elapsedTime);

        fflush(logFile);
        fclose(logFile);
        logFile = nullptr;
    }
}

void NDLogger::Log(LogLevel level, const char* format, ...) {
    if (level < minLevel)
        return;

    std::lock_guard<std::mutex> lock(logMutex);

    // Timestamp cho mỗi log entry
    time_t now = time(0);
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", localtime(&now));

    // Lấy thời gian chạy
    double elapsedTime = GetElapsedTime();

    // Chuỗi cấp độ log
    const char* levelStr;
    switch (level) {
    case LOG_DEBUG:   levelStr = "DEBUG"; break;
    case LOG_INFO:    levelStr = "INFO"; break;
    case LOG_WARNING: levelStr = "WARNING"; break;
    case LOG_ERROR:   levelStr = "ERROR"; break;
    case LOG_CRITICAL: levelStr = "CRITICAL"; break;
    default:          levelStr = "UNKNOWN"; break;
    }

    // Định dạng header log
    char header[100];
    snprintf(header, sizeof(header), "[%s %.3fs][%s] ", timeStr, elapsedTime, levelStr);

    // Format log message
    va_list args;
    va_start(args, format);

    char messageBuffer[4096];
    vsnprintf(messageBuffer, sizeof(messageBuffer), format, args);
    va_end(args);

    // Kết hợp header và message
    std::string logMessage = std::string(header) + std::string(messageBuffer);

    // Lưu vào buffer nếu file chưa mở
    if (!logFile) {
        logBuffer.push_back(logMessage);
        if (logBuffer.size() > maxBufferSize) {
            logBuffer.erase(logBuffer.begin());
        }
    }
    else {
        // Ghi vào file
        fprintf(logFile, "%s\n", logMessage.c_str());
        fflush(logFile);
    }

    // In ra console nếu được bật
    if (consoleEnabled) {
        // Mã màu cho console
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        WORD colorAttrs;

        switch (level) {
        case LOG_DEBUG:
            colorAttrs = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; // Trắng
            break;
        case LOG_INFO:
            colorAttrs = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE; // Cyan
            break;
        case LOG_WARNING:
            colorAttrs = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN; // Vàng
            break;
        case LOG_ERROR:
            colorAttrs = FOREGROUND_INTENSITY | FOREGROUND_RED; // Đỏ
            break;
        case LOG_CRITICAL:
            colorAttrs = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE; // Magenta
            break;
        default:
            colorAttrs = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE; // Mặc định: Xám
            break;
        }

        SetConsoleTextAttribute(hConsole, colorAttrs);
        printf("%s\n", logMessage.c_str());

        // Reset lại màu
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
}

void NDLogger::Debug(const char* format, ...) {
    if (LOG_DEBUG < minLevel)
        return;

    va_list args;
    va_start(args, format);

    char buffer[4096];
    vsnprintf(buffer, sizeof(buffer), format, args);

    va_end(args);

    Log(LOG_DEBUG, "%s", buffer);
}

void NDLogger::Info(const char* format, ...) {
    if (LOG_INFO < minLevel)
        return;

    va_list args;
    va_start(args, format);

    char buffer[4096];
    vsnprintf(buffer, sizeof(buffer), format, args);

    va_end(args);

    Log(LOG_INFO, "%s", buffer);
}

void NDLogger::Warning(const char* format, ...) {
    if (LOG_WARNING < minLevel)
        return;

    va_list args;
    va_start(args, format);

    char buffer[4096];
    vsnprintf(buffer, sizeof(buffer), format, args);

    va_end(args);

    Log(LOG_WARNING, "%s", buffer);
}

void NDLogger::Error(const char* format, ...) {
    if (LOG_ERROR < minLevel)
        return;

    va_list args;
    va_start(args, format);

    char buffer[4096];
    vsnprintf(buffer, sizeof(buffer), format, args);

    va_end(args);

    Log(LOG_ERROR, "%s", buffer);
}

void NDLogger::Critical(const char* format, ...) {
    if (LOG_CRITICAL < minLevel)
        return;

    va_list args;
    va_start(args, format);

    char buffer[4096];
    vsnprintf(buffer, sizeof(buffer), format, args);

    va_end(args);

    Log(LOG_CRITICAL, "%s", buffer);
}

void NDLogger::LogException(EXCEPTION_POINTERS* pExceptionPointers) {
    std::lock_guard<std::mutex> lock(logMutex);

    // Mở file crash log
    FILE* crashFile = fopen("ndclient_crash.log", "a");
    if (!crashFile) {
        Error("Failed to open crash log file");
        return;
    }

    time_t now = time(0);
    char timeStr[100];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", localtime(&now));

    fprintf(crashFile, "\n\n=== CRASH DETECTED AT %s ===\n", timeStr);
    fprintf(crashFile, "Exception code: 0x%08X\n", pExceptionPointers->ExceptionRecord->ExceptionCode);
    fprintf(crashFile, "Exception address: 0x%p\n", pExceptionPointers->ExceptionRecord->ExceptionAddress);

    // Lấy tên module và offset
    char moduleName[MAX_PATH] = "Unknown";
    DWORD64 moduleBase = 0;
    DWORD moduleSize = 0;

    HMODULE hModule = NULL;
    if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        (LPCTSTR)pExceptionPointers->ExceptionRecord->ExceptionAddress, &hModule)) {
        GetModuleFileNameA(hModule, moduleName, MAX_PATH);

        // Lấy chỉ tên file, không lấy đường dẫn
        char* filename = strrchr(moduleName, '\\');
        if (filename) {
            filename++; // Bỏ dấu '\'
            strcpy(moduleName, filename);
        }

        // Lấy thông tin module
        MODULEINFO moduleInfo;
        if (GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(moduleInfo))) {
            moduleBase = (DWORD64)moduleInfo.lpBaseOfDll;
            moduleSize = moduleInfo.SizeOfImage;
        }
    }

    // In thêm thông tin về module
    if (moduleBase) {
        DWORD64 offset = (DWORD64)pExceptionPointers->ExceptionRecord->ExceptionAddress - moduleBase;
        fprintf(crashFile, "Module: %s (Base: 0x%p, Size: 0x%X)\n", moduleName, (void*)moduleBase, moduleSize);
        fprintf(crashFile, "Module offset: 0x%X\n", (DWORD)offset);
    }

    // In thông tin về các exception parameters
    fprintf(crashFile, "Exception parameters: %d\n", pExceptionPointers->ExceptionRecord->NumberParameters);
    for (DWORD i = 0; i < pExceptionPointers->ExceptionRecord->NumberParameters; i++) {
        fprintf(crashFile, "  Parameter %d: 0x%X\n", i, (DWORD)pExceptionPointers->ExceptionRecord->ExceptionInformation[i]);
    }

    // In giá trị của các thanh ghi
    CONTEXT* ctx = pExceptionPointers->ContextRecord;
    fprintf(crashFile, "\nRegisters:\n");
    fprintf(crashFile, "EAX: 0x%08X\n", ctx->Eax);
    fprintf(crashFile, "EBX: 0x%08X\n", ctx->Ebx);
    fprintf(crashFile, "ECX: 0x%08X\n", ctx->Ecx);
    fprintf(crashFile, "EDX: 0x%08X\n", ctx->Edx);
    fprintf(crashFile, "ESI: 0x%08X\n", ctx->Esi);
    fprintf(crashFile, "EDI: 0x%08X\n", ctx->Edi);
    fprintf(crashFile, "EBP: 0x%08X\n", ctx->Ebp);
    fprintf(crashFile, "ESP: 0x%08X\n", ctx->Esp);
    fprintf(crashFile, "EIP: 0x%08X\n", ctx->Eip);
    fprintf(crashFile, "EFLAGS: 0x%08X\n", ctx->EFlags);

    // In memory dump tại nơi xảy ra lỗi
    fprintf(crashFile, "\nMemory dump at EIP:\n");

    const int bytesToDump = 32; // Dump 32 bytes trước và sau EIP
    DWORD64 memoryAddress = (DWORD64)ctx->Eip - bytesToDump;

    for (int i = -bytesToDump; i < bytesToDump; i += 16) {
        // Xác định địa chỉ hiện tại
        DWORD64 currentAddress = (DWORD64)ctx->Eip + i;

        // In địa chỉ
        fprintf(crashFile, "%p:", (void*)currentAddress);

        // Buffer để lưu bytes
        BYTE buffer[16] = { 0 };
        SIZE_T bytesRead = 0;

        // Đọc memory
        if (ReadProcessMemory(GetCurrentProcess(), (LPCVOID)currentAddress, buffer, 16, &bytesRead)) {
            // In hex bytes
            for (size_t j = 0; j < bytesRead; j++) {
                fprintf(crashFile, " %02X", buffer[j]);

                // Highlight byte tại EIP
                if ((currentAddress + j) == ctx->Eip) {
                    fprintf(crashFile, "*"); // Đánh dấu byte tại EIP
                }
            }

            // In phần ASCII
            fprintf(crashFile, "  ");
            for (size_t j = 0; j < bytesRead; j++) {
                if (buffer[j] >= 32 && buffer[j] <= 126) {
                    fprintf(crashFile, "%c", buffer[j]);
                }
                else {
                    fprintf(crashFile, ".");
                }
            }
        }
        else {
            fprintf(crashFile, " Cannot read memory");
        }

        fprintf(crashFile, "\n");
    }

    // Đóng file
    fflush(crashFile);
    fclose(crashFile);

    // Ghi cả vào log file thông thường
    Critical("!!! EXCEPTION DETECTED !!! Code: 0x%08X, Address: %p - See crash log for details",
        pExceptionPointers->ExceptionRecord->ExceptionCode,
        pExceptionPointers->ExceptionRecord->ExceptionAddress);
}

void NDLogger::DumpMemory(void* address, size_t size, const char* description) {
    std::lock_guard<std::mutex> lock(logMutex);

    if (!logFile && !consoleEnabled)
        return;

    std::stringstream ss;
    if (description) {
        ss << "Memory Dump: " << description << std::endl;
    }
    else {
        ss << "Memory Dump at " << address << " (" << size << " bytes):" << std::endl;
    }

    BYTE* bytePtr = (BYTE*)address;
    char ascii[17];
    ascii[16] = '\0';

    for (size_t i = 0; i < size; i++) {
        // In địa chỉ mỗi dòng 16 bytes
        if (i % 16 == 0) {
            if (i != 0) {
                ss << "  " << ascii << std::endl;
            }
            ss << std::hex << std::setw(8) << std::setfill('0') << ((size_t)address + i) << ": ";
        }

        // In các bytes
        BYTE thisByte = 0;
        if (ReadProcessMemory(GetCurrentProcess(), bytePtr + i, &thisByte, 1, NULL)) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)thisByte << " ";

            // Chuẩn bị phần ASCII
            ascii[i % 16] = (thisByte >= 32 && thisByte <= 126) ? (char)thisByte : '.';
        }
        else {
            ss << "?? ";
            ascii[i % 16] = '?';
        }
    }

    // In phần ASCII cho dòng cuối
    size_t remaining = size % 16;
    if (remaining == 0) remaining = 16;

    // Padding cho dòng cuối nếu không đủ 16 bytes
    for (size_t i = remaining; i < 16; i++) {
        ss << "   ";
        ascii[i] = ' ';
    }

    ss << "  " << ascii << std::endl;

    std::string dumpText = ss.str();

    // Ghi vào file
    if (logFile) {
        fprintf(logFile, "%s\n", dumpText.c_str());
        fflush(logFile);
    }

    // In ra console nếu được bật
    if (consoleEnabled) {
        printf("%s\n", dumpText.c_str());
    }
}

void NDLogger::SetLogLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(logMutex);
    minLevel = level;

    if (logFile) {
        fprintf(logFile, "\n[CONFIG] Log level changed to: %d\n", level);
        fflush(logFile);
    }

    if (consoleEnabled) {
        printf("\n[CONFIG] Log level changed to: %d\n", level);
    }
}

void NDLogger::EnableConsole(bool enable) {
    std::lock_guard<std::mutex> lock(logMutex);
    consoleEnabled = enable;

    if (logFile) {
        fprintf(logFile, "\n[CONFIG] Console output %s\n", enable ? "enabled" : "disabled");
        fflush(logFile);
    }
}

void NDLogger::FlushBuffer() {
    if (!logFile || logBuffer.empty())
        return;

    fprintf(logFile, "\n--- Flushing buffered log messages (%zu entries) ---\n", logBuffer.size());

    for (const auto& msg : logBuffer) {
        fprintf(logFile, "%s\n", msg.c_str());
    }

    fprintf(logFile, "--- End of buffered messages ---\n\n");
    fflush(logFile);

    logBuffer.clear();
}

double NDLogger::GetElapsedTime() {
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = now - startTime;
    return elapsed.count();
}

const std::string& NDLogger::GetLogFilePath() {
    return logFilePath;
}

// Hàm tương thích với WriteLog cũ
void WriteLog(const char* format, ...) {
    va_list args;
    va_start(args, format);

    char buffer[4096];
    vsnprintf(buffer, sizeof(buffer), format, args);

    va_end(args);

    NDLogger::Info("%s", buffer);
}

// Định nghĩa wrapper cho LONG WINAPI MyUnhandledExceptionFilter
LONG WINAPI LogUnhandledException(EXCEPTION_POINTERS* pExceptionPointers) {
    NDLogger::LogException(pExceptionPointers);
    return EXCEPTION_CONTINUE_SEARCH;
}