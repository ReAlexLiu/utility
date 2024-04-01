#ifndef SLOGGER_HPP
#define SLOGGER_HPP
#include <sstream>

#include "xlogger.hpp"

namespace utility
{
class logstream
{
public:
    logstream(spdlog::level::level_enum lvl, const char* filename, const char* funcname, int line)
        : level_(lvl)
        , filename_(filename)
        , funcname_(funcname)
        , line_(line)
    {
    }

    ~logstream()
    {
        xlogger::getInstance().log_(level_, filename_.c_str(), funcname_.c_str(), line_, stream_.str().c_str());
    }

    std::ostringstream& log_()
    {
        return stream_;
    }

    static void setLevel(const std::string &log_level)
    {
        if(log_level == "DEBUG")
        {
            xlogger::getInstance().setLevel(1);
        }
        else if(log_level == "INFO")
        {
            xlogger::getInstance().setLevel(2);
        }
        else if(log_level == "WARN")
        {
            xlogger::getInstance().setLevel(3);
        }
        else if(log_level == "ERROR")
        {
            xlogger::getInstance().setLevel(4);
        }
        else if(log_level == "FATAL")
        {
            xlogger::getInstance().setLevel(5);
        }
        else if(log_level == "NONE")
        {
            xlogger::getInstance().setLevel(6);
        }
    }

private:
    std::ostringstream        stream_;
    spdlog::level::level_enum level_;
    std::string               filename_;
    std::string               funcname_;
    int                       line_;
};
}

#define LogLevel(level) utility::logstream::setLevel(level)
#define LogDebug utility::logstream(spdlog::level::debug, __FILENAME__, __FUNCTION__, __LINE__).log_()
#define LogInfo utility::logstream(spdlog::level::info, __FILENAME__, __FUNCTION__, __LINE__).log_()
#define LogWarn utility::logstream(spdlog::level::warn, __FILENAME__, __FUNCTION__, __LINE__).log_()
#define LogError utility::logstream(spdlog::level::err, __FILENAME__, __FUNCTION__, __LINE__).log_()
#define LogFatal utility::logstream(spdlog::level::critical, __FILENAME__, __FUNCTION__, __LINE__).log_()
#endif