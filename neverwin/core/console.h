#pragma once
#include <windows.h>
#include <cstdio>
#include <cstdarg>
#include <mutex>
#include <fstream>

namespace core {

enum class LogLevel { Debug, Info, Warn, Error, Success };

class Console {
public:
    static Console& Instance() { static Console c; return c; }

    void Init(const char* title = "Astraea") {
        AllocConsole();
        freopen_s(&out_, "CONOUT$", "w", stdout);
        freopen_s(&err_, "CONOUT$", "w", stderr);
        SetConsoleTitleA(title);
        handle_ = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleOutputCP(CP_UTF8);
        initialized_ = true;
        Log(LogLevel::Success, "[astraea] console initialized");
    }

    void Shutdown() {
        if (initialized_) { FreeConsole(); initialized_ = false; }
        if (file_.is_open()) file_.close();
    }

    void EnableFileLog(const char* path) {
        file_.open(path, std::ios::app);
    }

    void Log(LogLevel level, const char* fmt, ...) {
        if (!initialized_) return;
        std::lock_guard<std::mutex> lock(mutex_);
        char buf[4096];
        va_list args; va_start(args, fmt);
        vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);

        SYSTEMTIME st{}; GetLocalTime(&st);

        const char* prefix = "";
        WORD color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        switch (level) {
            case LogLevel::Debug:   prefix = "[dbg] "; color = FOREGROUND_INTENSITY; break;
            case LogLevel::Info:    prefix = "[inf] "; color = FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
            case LogLevel::Warn:    prefix = "[wrn] "; color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; break;
            case LogLevel::Error:   prefix = "[err] "; color = FOREGROUND_RED | FOREGROUND_INTENSITY; break;
            case LogLevel::Success: prefix = "[ok]  "; color = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY; break;
        }

        SetConsoleTextAttribute(handle_, color);
        printf("%02d:%02d:%02d %s%s\n", st.wHour, st.wMinute, st.wSecond, prefix, buf);
        SetConsoleTextAttribute(handle_, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);

        if (file_.is_open()) {
            file_ << st.wHour << ":" << st.wMinute << ":" << st.wSecond << " " << prefix << buf << "\n";
            file_.flush();
        }
    }

private:
    FILE* out_ = nullptr;
    FILE* err_ = nullptr;
    HANDLE handle_ = nullptr;
    bool initialized_ = false;
    std::ofstream file_;
    std::mutex mutex_;
};

} // namespace core

#define LOG_DEBUG(...)   core::Console::Instance().Log(core::LogLevel::Debug,   __VA_ARGS__)
#define LOG_INFO(...)    core::Console::Instance().Log(core::LogLevel::Info,    __VA_ARGS__)
#define LOG_WARN(...)    core::Console::Instance().Log(core::LogLevel::Warn,    __VA_ARGS__)
#define LOG_ERROR(...)   core::Console::Instance().Log(core::LogLevel::Error,   __VA_ARGS__)
#define LOG_SUCCESS(...) core::Console::Instance().Log(core::LogLevel::Success, __VA_ARGS__)