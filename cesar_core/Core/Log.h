#pragma once
#include <print>
#include <string_view>
#include <source_location>

namespace cesar
{
    enum class LogLevel { 
        Trace,
        Debug,
        Info,
        Warning, 
        Error, 
        Fatal 
    };

    namespace detail
    {
        constexpr std::string_view LevelToString(LogLevel level)
        {
            switch (level)
            {
            case LogLevel::Trace:   return "TRACE";
            case LogLevel::Debug:   return "DEBUG";
            case LogLevel::Info:    return "INFO";
            case LogLevel::Warning: return "WARNING";
            case LogLevel::Error:   return "ERROR";
            case LogLevel::Fatal:   return "FATAL";
            }
            return "UNKNOWN";
        }

        constexpr std::string_view LevelToColor(LogLevel level)
        {
            switch (level)
            {
                case LogLevel::Trace:   return "\033[37m";   // white
                case LogLevel::Debug:   return "\033[36m";   // cyan
                case LogLevel::Info:    return "\033[32m";   // green
                case LogLevel::Warning: return "\033[33m";   // yellow
                case LogLevel::Error:   return "\033[31m";   // red
                case LogLevel::Fatal:   return "\033[35m";   // magenta
            }
            return "\033[0m";
        }

        constexpr std::string_view ResetColor = "\033[0m";
    }

    class _declspec(dllexport) Logger
    {
    public:
        static Logger& Get()
        {
            static Logger instance;
            return instance;
        }

        void SetMinLevel(LogLevel level) { min_level = level; }

        template<typename... Args>
        void Log(LogLevel level, std::string_view fmt, const std::source_location& loc, Args&&... args)
        {
            if (level < min_level) return;

            auto msg = std::vformat(fmt, std::make_format_args(args...));

            std::println("{}[{}] {}:{} — {}{}",
                detail::LevelToColor(level),
                detail::LevelToString(level),
                loc.file_name(),
                loc.line(),
                msg,
                detail::ResetColor);
        }

    private:
        Logger() = default;
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

        LogLevel min_level = LogLevel::Trace;
    };

    // Helper to capture source_location transparently
    template<typename... Args>
    struct _declspec(dllexport) LogHelper
    {
        LogHelper(LogLevel level, std::string_view fmt, Args&&... args,
            const std::source_location& loc = std::source_location::current())
        {
            Logger::Get().Log(level, fmt, loc, std::forward<Args>(args)...);
        }
    };

    template<typename... Args>
    LogHelper(LogLevel, std::string_view, Args&&...) -> LogHelper<Args...>;

} // namespace sol_core


#define CESAR_LOG(level, fmt, ...) \
    cesar::LogHelper(cesar::LogLevel::level, fmt, ##__VA_ARGS__)

#define LOG_TRACE(fmt, ...)   CESAR_LOG(Trace,   fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...)   CESAR_LOG(Debug,   fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)    CESAR_LOG(Info,    fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)    CESAR_LOG(Warning, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)   CESAR_LOG(Error,   fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...)   CESAR_LOG(Fatal,   fmt, ##__VA_ARGS__)

#define LOG_LEVEL(level) cesar::Logger::Get().SetMinLevel(cesar::LogLevel::level)