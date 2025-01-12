/*
* Copyright (c) 2018 Eyecool Inc. All rights reserved.
*
* The Original Code and all software distributed under the License are
* distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
* EXPRESS OR IMPLIED, AND EYECOOL HEREBY DISCLAIMS ALL SUCH WARRANTIES,
* INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
* Please see the License for the specific language governing rights and
* limitations under the License.
*
* @Descripttion:
* @Author: l2q
* @Date: 2021/3/8 13:27
* @LastEditors: lucky
* @LastEditTime: 2023/4/7 8:15
*/

#ifndef XLOGGER_HPP
#define XLOGGER_HPP

#include "config/config.h"
#include "precompiled.hpp"
#include "utility.hpp"

#include <spdlog/async.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <fstream>
#include <string>

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#ifdef WIN32
#include <spdlog/sinks/msvc_sink.h>
#define __FILENAME__ __FILE__
#endif

#include <iostream>
#include <string>
#include <vector>

#define UNUSED(x) (void)x;

/*
typedef struct
{
    int  level;      // TRACE 0 DEBUG 1 INFO 2 WARN 3 ERROR 4 CRITICAL 5 OFF 6
    bool console;    // 控制台日志
    bool file;       // 文件日志
    int  max_files;  // 最大文件数
    bool rotation;   // 环形日志 true 启用环形日志，false 启用轮替日志
    int  max_size;   // 单个文件最大尺寸 单位M（环形日志启用有效）
    int  hour;       // 日志切换时间（启用轮替日志有效）
    int  minute;     // 日志切换时间
    int  interval;   // 强制刷新                                                                                                                                                                  hour : 0 #日志切换时间（启用轮替日志有效） minute : 1 #日志切换时间 interval : 10 #强制刷新
} log_config;
 */
namespace utility
{
class xlogger
{
    SINGLE_TPL(xlogger);

private:
    void create()
    {
        std::string _config_path = utility::utility::basepath();
        _config_path += "/../config/config.yaml";
        YAML::Node config      = YAML::LoadFile(_config_path.c_str());

        // 解析日志
        log_config cfg;
        cfg.level     = config["logs"]["level"].as<int>();              // TRACE 0 DEBUG 1 INFO 2 WARN 3 ERROR 4 CRITICAL 5 OFF 6
        cfg.console   = config["logs"]["console"].as<bool>();           // 控制台日志
        cfg.file      = config["logs"]["file"].as<bool>();              // 文件日志
        cfg.max_files = config["logs"]["max_files"].as<int>();  // 最大文件数
        cfg.rotation  = config["logs"]["rotation"].as<bool>();          // 环形日志 true 启用环形日志，false 启用轮替日志
        cfg.max_size  = config["logs"]["max_size"].as<int>();           // 单个文件最大尺寸 单位M（环形日志启用有效）
        cfg.hour      = config["logs"]["hour"].as<int>();               // 日志切换时间（启用轮替日志有效）
        cfg.minute    = config["logs"]["minute"].as<int>();             // 日志切换时间
        cfg.interval  = config["logs"]["interval"].as<int>();           // 强制刷新

        level_                 = static_cast<spdlog::level::level_enum>(cfg.level);

        std::vector<spdlog::sink_ptr> _sinkList;

        if (cfg.console)
        {
            auto _stdoutColorSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            _stdoutColorSink->set_level(level_);
            _stdoutColorSink->set_pattern("%^[%L][%m-%d %H:%M:%S.%e][%!,%@][%t] %v%$");
            _sinkList.push_back(_stdoutColorSink);
#ifdef WIN32
            auto _msvcColorSink = std::make_shared<spdlog::sinks::windebug_sink_mt>();
            _msvcColorSink->set_level(level_);
            _msvcColorSink->set_pattern("%^[%L][%m-%d %H:%M:%S.%e][%!,%@][%t] %v%$");
            _sinkList.push_back(_msvcColorSink);
#endif  // WIN32
        }

        std::string _save_path = utility::basepath();
        _save_path += "/../logs/";
        _save_path += utility::basename();
        _save_path += ".log";


        std::size_t _max_files = cfg.max_files;
        if (cfg.file)
        {
            if (cfg.rotation)
            {
                std::size_t _max_size        = cfg.max_size * 1024 * 1024;  // 默认200M
                auto        rotatingFileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(_save_path, _max_size, _max_files);
                rotatingFileSink->set_level(level_);
                rotatingFileSink->set_pattern("[%L][%Y-%m-%d %H:%M:%S.%e][%!,%@][%t] %v");
                _sinkList.push_back(rotatingFileSink);
            }
            else
            {
                int  _rotation_hour   = cfg.hour;
                int  _rotation_minute = cfg.minute;
                auto dailyFileSink    = std::make_shared<spdlog::sinks::daily_file_sink_mt>(_save_path, _rotation_hour, _rotation_minute, false, static_cast<int>(_max_files * 4));
                // auto dailyFileSink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(_save_path, _rotation_hour, _rotation_minute);
                dailyFileSink->set_level(level_);
                dailyFileSink->set_pattern("[%L][%Y-%m-%d %H:%M:%S.%e][%!,%@][%t] %v");
                _sinkList.push_back(dailyFileSink);
            }
        }

        if (_sinkList.size() > 0)
        {
            // async
            // spdlog::init_thread_pool(2048, 1);
            // logger_ = std::make_shared<spdlog::async_logger>("multi_sink", _sinkList.begin(), _sinkList.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
            // spdlog::register_logger(logger_);
            // logger_->flush_on(spdlog::level::err);
            // spdlog::flush_every(std::chrono::seconds(_config.get("global.log.interval", 10)));
            // logger_->set_level(level_);

            // sync
            logger_ = std::make_shared<spdlog::logger>("multi_sink", begin(_sinkList), end(_sinkList));
            spdlog::register_logger(logger_);
            logger_->flush_on(spdlog::level::critical);

            spdlog::flush_every(std::chrono::seconds(cfg.interval));
            logger_->set_level(level_);
        }
    }

    void destory()
    {
        //  spdlog::drop_all();
    }

public:
    void setLevel(int level)
    {
        level_ = static_cast<spdlog::level::level_enum>(level);
        logger_->set_level(level_);
    }

    template <typename... Args>
    void log_(spdlog::level::level_enum lvl, const char* filename, const char* funcname, int line, spdlog::string_view_t fmt, const Args&... args)
    {
        try
        {
            spdlog::source_loc loc(filename, line, funcname);
            logger_->log(loc, lvl, fmt, args...);
        }
        catch (...)
        {
            std::cerr << "failed to write log" << std::endl;
        }
    }

private:
    spdlog::level::level_enum       level_;
    // std::shared_ptr<spdlog::async_logger> logger_;
    std::shared_ptr<spdlog::logger> logger_;
};
}

// 基础的输出封装
#define xlevel(level) utility::xlogger::getInstance().setLevel(level)
#define xverbose(...) utility::xlogger::getInstance().log_(spdlog::level::trace, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define xdebug(...) utility::xlogger::getInstance().log_(spdlog::level::debug, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define xinfo(...) utility::xlogger::getInstance().log_(spdlog::level::info, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define xwarn(...) utility::xlogger::getInstance().log_(spdlog::level::warn, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define xerror(...) utility::xlogger::getInstance().log_(spdlog::level::err, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define xfatal(...) utility::xlogger::getInstance().log_(spdlog::level::critical, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__)

// 给引擎用的封装，附加输出引擎id，方便调试，禁止在引擎以外使用
#define everbose(fmt, ...) utility::xlogger::getInstance().log_(spdlog::level::trace, __FILENAME__, __FUNCTION__, __LINE__, "Id: {}, "#fmt, engineId_, ##__VA_ARGS__)
#define edebug(fmt, ...) utility::xlogger::getInstance().log_(spdlog::level::debug, __FILENAME__, __FUNCTION__, __LINE__, "Id: {}, "#fmt, engineId_, ##__VA_ARGS__)
#define einfo(fmt, ...) utility::xlogger::getInstance().log_(spdlog::level::info, __FILENAME__, __FUNCTION__, __LINE__, "Id: {}, "#fmt, engineId_, ##__VA_ARGS__)
#define ewarn(fmt, ...) utility::xlogger::getInstance().log_(spdlog::level::warn, __FILENAME__, __FUNCTION__, __LINE__, "Id: {}, "#fmt, engineId_, ##__VA_ARGS__)
#define eerror(fmt, ...) utility::xlogger::getInstance().log_(spdlog::level::err, __FILENAME__, __FUNCTION__, __LINE__, "Id: {}, "#fmt, engineId_, ##__VA_ARGS__)
#define efatal(fmt, ...) utility::xlogger::getInstance().log_(spdlog::level::critical, __FILENAME__, __FUNCTION__, __LINE__, "EngineId: {}, "#fmt, engineId_, ##__VA_ARGS__)


#define xverbose_if(exp, ...)                                                                                                       \
    if (exp)                                                                                                                        \
    {                                                                                                                               \
        utility::xlogger::getInstance().log_(spdlog::level::trace, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__); \
    }

#define xdebug_if(exp, ...)                                                                                                         \
    if (exp)                                                                                                                        \
    {                                                                                                                               \
        utility::xlogger::getInstance().log_(spdlog::level::debug, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__); \
    }

#define xinfo_if(exp, ...)                                                                                                         \
    if (exp)                                                                                                                       \
    {                                                                                                                              \
        utility::xlogger::getInstance().log_(spdlog::level::info, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__); \
    }

#define xwarn_if(exp, ...)                                                                                                         \
    if (exp)                                                                                                                       \
    {                                                                                                                              \
        utility::xlogger::getInstance().log_(spdlog::level::warn, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__); \
    }

#define xerror_if(exp, ...)                                                                                                       \
    if (exp)                                                                                                                      \
    {                                                                                                                             \
        utility::xlogger::getInstance().log_(spdlog::level::err, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__); \
    }

#define xfatal_if(exp, ...)                                                                                                            \
    if (exp)                                                                                                                           \
    {                                                                                                                                  \
        utility::xlogger::getInstance().log_(spdlog::level::critical, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__); \
    }
#endif  // XLOGGER_HPP
