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
 * @Description:
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

#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#ifdef WIN32
#include <spdlog/sinks/msvc_sink.h>
#define __FILENAME__ __FILE__
#endif

#include <iostream>

#define UNUSED(x) (void)x;

#ifndef LOGER_CONFIG
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
#endif

#ifdef USE_GFLAGS
DEFINE_int32(log_level, -1, "日志级别（0-6: TRACE-Off）");
DEFINE_bool(log_console, false, "是否启用控制台日志（true/false）");
DEFINE_bool(log_file, false, "是否启用文件日志（true/false）");
DEFINE_int32(log_max_files, -1, "最大文件数");
DEFINE_bool(log_rotation, false, "是否启用环形日志（true）/轮替（false）");
DEFINE_int32(log_max_size, -1, "单个文件最大尺寸（MB，环形有效）");
DEFINE_int32(log_hour, -1, "轮替日志切换小时");
DEFINE_int32(log_minute, -1, "轮替日志切换分钟");
DEFINE_int32(log_interval, -1, "强制刷新间隔（秒）");
#endif

#ifdef USE_YAML
#include <boost/algorithm/string.hpp>
#include <yaml-cpp/yaml.h>
#endif

namespace utility
{
class xlogger
{
    SINGLE_TPL(xlogger);

private:
    void create()
    {
        init_default_config(config_);
#ifdef USE_YAML
        std::string _config_path = utility::utility::basepath();
        _config_path += "/../config/config.yaml";
        YAML::Node config = YAML::LoadFile(_config_path.c_str());

        read_config_from_yaml(_config_path, config_);
#endif


        std::vector<spdlog::sink_ptr> _sinkList;

        if (config_.console)
        {
            auto _stdoutColorSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            _stdoutColorSink->set_level(static_cast<spdlog::level::level_enum>(config_.level));
            _stdoutColorSink->set_pattern("%^[%L][%m-%d %H:%M:%S.%e][%!,%@][%t] %v%$");
            _sinkList.push_back(_stdoutColorSink);
#ifdef WIN32
            auto _msvcColorSink = std::make_shared<spdlog::sinks::windebug_sink_mt>();
            _msvcColorSink->set_level(static_cast<spdlog::level::level_enum>(config_.level));
            _msvcColorSink->set_pattern("%^[%L][%m-%d %H:%M:%S.%e][%!,%@][%t] %v%$");
            _sinkList.push_back(_msvcColorSink);
#endif  // WIN32
        }

        std::string _save_path = utility::basepath();
        _save_path += "/../logs/";
        _save_path += utility::basename();
        _save_path += ".log";


        std::size_t _max_files = config_.max_files;
        if (config_.file)
        {
            if (config_.rotation)
            {
                std::size_t _max_size        = config_.max_size * 1024 * 1024;  // 默认200M
                auto        rotatingFileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(_save_path, _max_size, _max_files);
                rotatingFileSink->set_level(static_cast<spdlog::level::level_enum>(config_.level));
                rotatingFileSink->set_pattern("[%L][%Y-%m-%d %H:%M:%S.%e][%!,%@][%t] %v");
                _sinkList.push_back(rotatingFileSink);
            }
            else
            {
                int  _rotation_hour   = config_.hour;
                int  _rotation_minute = config_.minute;
                auto dailyFileSink    = std::make_shared<spdlog::sinks::daily_file_sink_mt>(_save_path, _rotation_hour, _rotation_minute, false, static_cast<int>(_max_files * 4));
                // auto dailyFileSink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(_save_path, _rotation_hour, _rotation_minute);
                dailyFileSink->set_level(static_cast<spdlog::level::level_enum>(config_.level));
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
            // logger_->set_level(static_cast<spdlog::level::level_enum>(config_.level));

            // sync
            logger_ = std::make_shared<spdlog::logger>("multi_sink", begin(_sinkList), end(_sinkList));
            spdlog::register_logger(logger_);
            logger_->flush_on(spdlog::level::critical);

            spdlog::flush_every(std::chrono::seconds(config_.interval));
            logger_->set_level(static_cast<spdlog::level::level_enum>(config_.level));
        }
    }

    void destory()
    {
        //  spdlog::drop_all();
    }

public:
    void setLevel(int level)
    {
        if (level < 0 || level > 6)
        {
            std::cerr << "日志级别无效（必须0-6）" << std::endl;
            return;
        }
        config_.level = level;
        logger_->set_level(static_cast<spdlog::level::level_enum>(config_.level));
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

    // 初始化默认配置
    void init_default_config(log_config& config)
    {
        config.level     = 1;      // 默认INFO级别
        config.console   = true;   // 默认开启控制台日志
        config.file      = false;  // 默认关闭文件日志
        config.max_files = 5;      // 默认最大5个文件
        config.rotation  = true;   // 默认环形日志
        config.max_size  = 10;     // 默认单个文件10MB
        config.hour      = 0;      // 默认0点切换
        config.minute    = 0;      // 默认0分切换
        config.interval  = 120;    // 默认60秒刷新一次
    }

#ifdef USE_YAML
    // YAML方式读取配置
    /*
    logs:
      level: 0 # TRACE 0 DEBUG 1 INFO 2 WARN 3 ERROR 4 CRITICAL 5 OFF 6
      console: true # 控制台日志
      file: true #文件日志
      max_files: 3 #最大文件数 如果是环形日志，则就是文件数，如果是轮替日志，则为max_files*4
      rotation: true #环形日志 true 启用环形日志，false 启用轮替日志
      max_size: 200  #单个文件最大尺寸 单位M（环形日志启用有效）
      hour: 0 #日志切换时间（启用轮替日志有效）
      minute: 1 #日志切换时间
      interval: 10 #强制刷新
    */
    bool read_config_from_yaml(const std::string& filename, log_config& conf)
    {
        try
        {
            YAML::Node config = YAML::LoadFile(filename);

            // 校验级别范围
            int level = config["logs"]["level"].as<int>();
            if (level < 0 || level > 6)
            {
                std::cerr << "日志级别无效（必须0-6）" << std::endl;
            }
            else
            {
                conf.level      = level;      // TRACE 0 DEBUG 1 INFO 2 WARN 3 ERROR 4 CRITICAL 5 OFF 6
            }
            conf.console    = config["logs"]["console"].as<bool>();   // 控制台日志
            conf.file       = config["logs"]["file"].as<bool>();      // 文件日志
            conf.max_files  = config["logs"]["max_files"].as<int>();  // 最大文件数
            conf.rotation   = config["logs"]["rotation"].as<bool>();  // 环形日志 true 启用环形日志，false 启用轮替日志
            conf.max_size   = config["logs"]["max_size"].as<int>();   // 单个文件最大尺寸 单位M（环形日志启用有效）
            conf.hour       = config["logs"]["hour"].as<int>();       // 日志切换时间（启用轮替日志有效）
            conf.minute     = config["logs"]["minute"].as<int>();     // 日志切换时间
            conf.interval   = config["logs"]["interval"].as<int>();   // 强制刷新
        }
        catch (const YAML::Exception& e)
        {
            std::cerr << "failed to read log config: " << e.what() << std::endl;
            return false;
        }
        return true;
    }
#endif

#ifdef USE_GFLAGS
    // gflags方式读取配置（通过命令行参数）
    void read_config_from_gflags(log_config& conf)
    {
        if (FLAGS_log_level != -1)
        {
            // 校验级别范围
            if (FLAGS_log_level < 0 || cFLAGS_log_level > 6)
            {
                std::cerr << "日志级别无效（必须0-6）" << std::endl;
            }
            else
            {
                conf.level = FLAGS_log_level;
            }
        }
        if (FLAGS_log_max_files != -1)
        {
            conf.max_files = FLAGS_log_max_files
        };
        if (FLAGS_log_max_size != -1)
        {
            conf.max_size = FLAGS_log_max_size
        };
        if (FLAGS_log_hour != -1)
        {
            conf.hour = FLAGS_log_hour
        };
        if (FLAGS_log_minute != -1)
        {
            conf.minute = FLAGS_log_minute
        };
        if (FLAGS_log_interval != -1)
        {
            conf.interval = FLAGS_log_interval
        };

        // bool类型参数默认值为false，需判断是否被显式设置（通过命令行传递）
        // 注：gflags无法直接判断bool参数是否被设置，此处简化处理（显式传递则覆盖）
        conf.console  = FLAGS_log_console;
        conf.file     = FLAGS_log_file;
        conf.rotation = FLAGS_log_rotation;
    }
#endif
#ifdef USE_INI
    // INI方式读取配置
    /*
    [logs]
    level=2          ; INFO级别
    console=true
    file=true
    max_files=10
    rotation=false   ; 轮替日志
    max_size=20      ; 环形有效，此处可忽略
    hour=2           ; 每天2点
    minute=30        ; 30分
    interval=30      ; 30秒刷新
    */
    bool read_config_from_ini(const std::string& filename, log_config& conf);
    {
        try
        {
            // 加载INI文件到property_tree
            boost::property_tree::ptree pt;
            boost::property_tree::read_ini(filename, pt);

            // 读取[log]节下的配置（存在则覆盖默认值）
            auto log_section = pt.get_child_optional("log");
            if (!log_section)
            {
                std::cerr << "INI文件中未找到[log]节" << std::endl;
                return false;
            }

            // 解析每个字段（使用get_optional避免字段不存在时抛异常）
            if (auto val = log_section->get_optional<int>("level"))
            {
                // 校验日志级别范围
                if (*val < 0 || *val > 6)
                {
                    std::cerr << "日志级别无效（必须0-6）" << std::endl;
                }
                else
                {
                    conf.level = *val;
                }
            }
            if (auto val = log_section->get_optional<std::string>("console"))
            {
                // 支持true/false、1/0两种格式
                conf.console = (*val == "true" || *val == "1");
            }
            if (auto val = log_section->get_optional<std::string>("file"))
            {
                conf.file = (*val == "true" || *val == "1");
            }
            if (auto val = log_section->get_optional<int>("max_files"))
            {
                conf.max_files = *val;
            }
            if (auto val = log_section->get_optional<std::string>("rotation"))
            {
                conf.rotation = (*val == "true" || *val == "1");
            }
            if (auto val = log_section->get_optional<int>("max_size"))
            {
                conf.max_size = *val;
            }
            if (auto val = log_section->get_optional<int>("hour"))
            {
                conf.hour = *val;
            }
            if (auto val = log_section->get_optional<int>("minute"))
            {
                conf.minute = *val;
            }
            if (auto val = log_section->get_optional<int>("interval"))
            {
                conf.interval = *val;
            }

            return true;
        }
        catch (const boost::property_tree::ini_parser_error& e)
        {
            std::cerr << "INI解析错误: " << e.what() << "（文件: " << e.filename() << "，行号: " << e.line() << "）" << std::endl;
            return false;
        }
        catch (const std::exception& e)
        {
            std::cerr << "配置解析异常: " << e.what() << std::endl;
            return false;
        }
    }
#endif

private:
    // std::shared_ptr<spdlog::async_logger> logger_;
    std::shared_ptr<spdlog::logger> logger_;
    log_config                      config_;
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
#define everbose(fmt, ...) utility::xlogger::getInstance().log_(spdlog::level::trace, __FILENAME__, __FUNCTION__, __LINE__, "Id: {}, " #fmt, engineId_, ##__VA_ARGS__)
#define edebug(fmt, ...) utility::xlogger::getInstance().log_(spdlog::level::debug, __FILENAME__, __FUNCTION__, __LINE__, "Id: {}, " #fmt, engineId_, ##__VA_ARGS__)
#define einfo(fmt, ...) utility::xlogger::getInstance().log_(spdlog::level::info, __FILENAME__, __FUNCTION__, __LINE__, "Id: {}, " #fmt, engineId_, ##__VA_ARGS__)
#define ewarn(fmt, ...) utility::xlogger::getInstance().log_(spdlog::level::warn, __FILENAME__, __FUNCTION__, __LINE__, "Id: {}, " #fmt, engineId_, ##__VA_ARGS__)
#define eerror(fmt, ...) utility::xlogger::getInstance().log_(spdlog::level::err, __FILENAME__, __FUNCTION__, __LINE__, "Id: {}, " #fmt, engineId_, ##__VA_ARGS__)
#define efatal(fmt, ...) utility::xlogger::getInstance().log_(spdlog::level::critical, __FILENAME__, __FUNCTION__, __LINE__, "EngineId: {}, " #fmt, engineId_, ##__VA_ARGS__)


#define xverbose_if(exp, ...)                                                                                          \
    if (exp)                                                                                                           \
    {                                                                                                                  \
        utility::xlogger::getInstance().log_(spdlog::level::trace, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__); \
    }

#define xdebug_if(exp, ...)                                                                                            \
    if (exp)                                                                                                           \
    {                                                                                                                  \
        utility::xlogger::getInstance().log_(spdlog::level::debug, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__); \
    }

#define xinfo_if(exp, ...)                                                                                            \
    if (exp)                                                                                                          \
    {                                                                                                                 \
        utility::xlogger::getInstance().log_(spdlog::level::info, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__); \
    }

#define xwarn_if(exp, ...)                                                                                            \
    if (exp)                                                                                                          \
    {                                                                                                                 \
        utility::xlogger::getInstance().log_(spdlog::level::warn, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__); \
    }

#define xerror_if(exp, ...)                                                                                          \
    if (exp)                                                                                                         \
    {                                                                                                                \
        utility::xlogger::getInstance().log_(spdlog::level::err, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__); \
    }

#define xfatal_if(exp, ...)                                                                                               \
    if (exp)                                                                                                              \
    {                                                                                                                     \
        utility::xlogger::getInstance().log_(spdlog::level::critical, __FILENAME__, __FUNCTION__, __LINE__, __VA_ARGS__); \
    }
#endif  // XLOGGER_HPP
