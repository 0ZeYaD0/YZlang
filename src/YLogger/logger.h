#pragma once
// -------------- MINI-LOGGER-CPP ------------
// made by yassin ibrahim (DeltaY)

#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <mutex>
#include <time.h>
#include <vector>

#define LOCK_MUTEX(x) std::lock_guard<std::mutex> lock(x)

#include <chrono>
using std::chrono::system_clock;

// NOTE: remove this line to remove colors
#define TEXT_COLORS

#ifdef TEXT_COLORS
#define TEXT_RED "\x1b[31m"
#define TEXT_GREEN "\x1b[32m"
#define TEXT_YELLOW "\x1b[33m"
#define TEXT_BLUE "\x1b[34m"
#define TEXT_PURPLE "\x1b[35m"
#define TEXT_CYAN "\x1b[36m"
#define TEXT_WHITE "\x1b[0m"
#else
#define TEXT_RED "\x1b[0m"
#define TEXT_GREEN "\x1b[0m"
#define TEXT_YELLOW "\x1b[0m"
#define TEXT_BLUE "\x1b[0m"
#define TEXT_PURPLE "\x1b[0m"
#define TEXT_CYAN "\x1b[0m"
#define TEXT_WHITE "\x1b[0m"
#endif

#define RED_TEXT(x) TEXT_RED, x, TEXT_WHITE
#define GREEN_TEXT(x) TEXT_GREEN, x, TEXT_WHITE
#define YELLOW_TEXT(x) TEXT_YELLOW, x, TEXT_WHITE
#define BLUE_TEXT(x) TEXT_BLUE, x, TEXT_WHITE
#define PURPLE_TEXT(x) TEXT_PURPLE, x, TEXT_WHITE
#define CYAN_TEXT(x) TEXT_CYAN, x, TEXT_WHITE

#define LOG_NONE 0
#define LOG_FATAL 1
#define LOG_ERROR 2
#define LOG_WARN 3
#define LOG_DEBUG 4
#define LOG_TRACE 5
#define LOG_INFO 6
#define LOG_INFO_ONLY 7

#define LOG_CONSOLE 1
#define LOG_FILE 2
#define LOG_ALL 3

#define LOG_DEFAULT_FILE "log.txt"
#define __FILENAME__ (strstr(__FILE__, "src") ? strstr(__FILE__, "src") + 3 : __FILE__)
#define FILE_INFO __FILENAME__, __LINE__
#define NO_FILE_INFO nulllptr, nullptr

#define _LIBCPP_TOSTRING2(s) #s

#define LOGINIT() Logger log
#define LOGINFO(x, y, z)      \
    LogInfo info;             \
    info.level = (LogLevel)x; \
    info.filename = y;        \
    info.linenumber = _LIBCPP_TOSTRING2(z);

#define LOG_CHANGE_PRIORITY(x) Logger::priority = (LogLevel)x;

#define LFATAL(x...)                               \
    {                                              \
        LOGINIT();                                 \
        LOGINFO(LOG_FATAL, __FILENAME__, __LINE__) \
        log.Log(info, x);                          \
    }

#define LERROR(x...)                               \
    {                                              \
        LOGINIT();                                 \
        LOGINFO(LOG_FATAL, __FILENAME__, __LINE__) \
        log.Log(info, x);                          \
    }

#define LWARN(x, y...)                                \
    {                                                 \
        LOGINIT();                                    \
        if (x)                                        \
        {                                             \
            LOGINFO(LOG_WARN, __FILENAME__, __LINE__) \
            log.Log(info, y);                         \
        }                                             \
        else                                          \
        {                                             \
            LOGINFO(LOG_WARN, "", "")                 \
            log.Log(info, y);                         \
        }                                             \
    }

#define LDEBUG(x, y...)                                \
    {                                                  \
        LOGINIT();                                     \
        if (x)                                         \
        {                                              \
            LOGINFO(LOG_DEBUG, __FILENAME__, __LINE__) \
            log.Log(info, y);                          \
        }                                              \
        else                                           \
        {                                              \
            LOGINFO(LOG_DEBUG, "", "")                 \
            log.Log(info, y);                          \
        }                                              \
    }

#define LTRACE(x, y...)                                \
    {                                                  \
        LOGINIT();                                     \
        if (x)                                         \
        {                                              \
            LOGINFO(LOG_TRACE, __FILENAME__, __LINE__) \
            log.Log(info, y);                          \
        }                                              \
        else                                           \
        {                                              \
            LOGINFO(LOG_TRACE, "", "")                 \
            log.Log(info, y);                          \
        }                                              \
    }

#define LINFO(x, y...)                                \
    {                                                 \
        LOGINIT();                                    \
        if (x)                                        \
        {                                             \
            LOGINFO(LOG_INFO, __FILENAME__, __LINE__) \
            log.Log(info, y);                         \
        }                                             \
        else                                          \
        {                                             \
            LOGINFO(LOG_INFO, "", "")                 \
            log.Log(info, y);                         \
        }                                             \
    }

#define LASSERT(x, y)  \
    {                  \
        if (!(x))      \
        {              \
            LFATAL(y); \
            exit(1);   \
        }              \
    }

// log without anyting.
#define LLOG(x...)                \
    {                             \
        LOGINIT();                \
        LOGINFO(LOG_NONE, "", "") \
        log.Log(info, x);         \
    }

enum class LogLevel
{
    NONE = 0,
    FATAL = 1,
    ERROR = 2,
    WARN = 3,
    DEBUG = 4,
    TRACE = 5,
    INFO = 6,
    INFO_ONLY = 7
};

enum class OutputType
{
    NONE = 0,
    CONSOLE = 1,
    FILE = 2,
    ALL = 3
};

struct LogInfo
{
    LogLevel level;
    std::string filename;
    std::string linenumber;
};

//___________________ LOGGER CLASS _____________________
class Logger
{
private:
    static std::string filePath;
    static std::ofstream fileStream;

    static std::mutex mut;

public:
    static LogLevel priority;
    static OutputType outType;

private:
    static inline void EnableFileOutput(std::string filepath = LOG_DEFAULT_FILE)
    {
        if (filepath.size() != 0)
            filePath = filepath;

        if (fileStream.is_open())
            fileStream.close();

        fileStream.open(filePath, std::ios::app);
    }

    static inline void DisableFileOutput()
    {
        if (fileStream.is_open())
            fileStream.close();
    }

    static inline std::string GetTitle(LogLevel ll, bool colored)
    {
        std::string output;
        switch (ll)
        {
        case LogLevel::NONE:
            break;
        case LogLevel::FATAL:
            output = "";
            if (colored)
                output.append(TEXT_RED);
            output.append("FATAL");
            return output;
            break;
        case LogLevel::ERROR:
            if (colored)
                output = TEXT_RED;
            output.append("ERROR");
            return output;
            break;
        case LogLevel::WARN:
            if (colored)
                output = TEXT_YELLOW;
            output.append("WARN");
            return output;
            break;
        case LogLevel::DEBUG:
            if (colored)
                output = TEXT_BLUE;
            output.append("DEBUG");
            return output;
            break;
        case LogLevel::TRACE:
            if (colored)
                output = TEXT_CYAN;
            output.append("TRACE");
            return output;
            break;
        case LogLevel::INFO:
            if (colored)
                output = TEXT_PURPLE;
            output.append("INFO");
            return output;
            break;
        case LogLevel::INFO_ONLY:
            if (colored)
                output = TEXT_PURPLE;
            output.append("INFO");
            break;
        }
        return output;
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    static inline std::string GetTimestamp()
    {
        auto now = system_clock::now();
        auto inTime = system_clock::to_time_t(now);

        std::stringstream ss;
        ss << std::put_time(std::localtime(&inTime), "%X");
        return ss.str();
    }
#pragma GCC diagnostic pop

    // clang-format off
    template<typename F, typename L>
    static inline std::string GetFileInfo(F file, L line)
    {
        std::stringstream ss;
        ss << file << ":" << line;

        return ss.str();
    }

    static inline std::string ResetColor()
    {
        return TEXT_WHITE;
    }

    template<typename F, typename L>
    static inline std::string GetFullHeader(LogLevel lvl , bool colored, F file, L line)
    {
        std::stringstream ss;
        ss << '[';

        ss << GetTitle(lvl, colored) << ' ';
        
        ss << GetFileInfo(file, line) << ' ';
        
        ss << GetTimestamp();

        if(colored)
            ss << ResetColor();

        ss << "]\t";

        return ss.str();
    }
    
    static inline std::string GetFullHeader(LogLevel lvl, bool colored)
    {
        std::stringstream ss;
        ss << '[';

        ss << GetTitle(lvl, colored) << ' ';
        
        ss << GetTimestamp();

        if(colored)
            ss << ResetColor();

        ss << "]\t";

        return ss.str();
    }

    public:
    Logger() {}

    template<typename T>
    static inline void ChangeOutputType(T type, std::string filepath = LOG_DEFAULT_FILE)
    {
        Logger::outType = (OutputType) type;
        switch(Logger::outType)
        {
            case OutputType::NONE:
                DisableFileOutput();
                break;
            case OutputType::CONSOLE:
                DisableFileOutput();
                break;
            case OutputType::FILE:
                EnableFileOutput(filepath);
                break;
            case OutputType::ALL:
                EnableFileOutput(filepath);
                break;
        }
    }

    template<typename T>
    static inline void ChangePriority(T ll)
    {
        Logger::priority = (LogLevel) ll;
    }

    template<typename Arg, typename... Args>
    void Log(LogInfo info, Arg&& arg, Args&&... args)
    {
        LOCK_MUTEX(mut);
        std::stringstream out;

        if(info.level > Logger::priority)
            return;

        if(Logger::priority == LogLevel::INFO_ONLY && info.level < LogLevel::INFO)
            return;

        switch(Logger::outType)
        {
        case OutputType::NONE:
            return;
        case OutputType::CONSOLE:
            if(info.level != LogLevel::NONE)
            {
                if(info.filename != "")
                    out << GetFullHeader(info.level, true, info.filename, info.linenumber);
                else
                    out << GetFullHeader(info.level, true);
            }

            out << std::forward<Arg>(arg);
            using expander = int[];
            (void) expander{0, (void(out << std::forward<Args>(args)), 0)...};

            std::cout << out.str();
            break;

        case OutputType::FILE:
            if(info.level != LogLevel::NONE)
            {
                if(info.filename != "")
                    out << GetFullHeader(info.level, true, info.filename, info.linenumber);
                else
                    out << GetFullHeader(info.level, true);
            }

            out << std::forward<Arg>(arg);
            using expander = int[];
            (void) expander{0, (void(out << std::forward<Args>(args)), 0)...};

            if(fileStream.is_open())
                fileStream << out.str();
            else
            {
                fileStream.open(filePath, std::ios::app);
                fileStream << out.str();
            }

            break;

        case OutputType::ALL:
            // console
            if(info.level != LogLevel::NONE)
            {
                if(info.filename != "")
                    out << GetFullHeader(info.level, true, info.filename, info.linenumber);
                else
                    out << GetFullHeader(info.level, true);
            }

            out << std::forward<Arg>(arg);
            using expander = int[];
            (void) expander{0, (void(out << std::forward<Args>(args)), 0)...};

            std::cout << out.str();

            out.str(std::string());

            // file
            std::stringstream out; 
            if(info.level != LogLevel::NONE)
            {
                if(info.filename != "")
                    out << GetFullHeader(info.level, true, info.filename, info.linenumber);
                else
                    out << GetFullHeader(info.level, true);
            }

            out << std::forward<Arg>(arg);
            using expander = int[];
            (void) expander{0, (void(out << std::forward<Args>(args)), 0)...};
            
            if(fileStream.is_open())
                fileStream << out.str();
            else
            {
                fileStream.open(filePath, std::ios::app);
                fileStream << out.str();
            }
            
            break;
        }
    }
    // clang-format on

    ~Logger()
    {
        if (fileStream.is_open())
            fileStream.close();
    }
};