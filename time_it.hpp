/*
 * @Descripttion:
 * @Author: lucky
 * @Date: 2024/4/3 17:46
 * @LastEditors: lucky
 * @LastEditTime: 2024/4/3 17:46
 */
//

#ifndef LIUCAODANGBANG_TIME_IT_H
#define LIUCAODANGBANG_TIME_IT_H

#include "xlogger.hpp"
namespace utility
{
class time_it
{
public:
    using param_t = typename std::conditional<std::is_reference<const char*>::value || std::is_pointer<const char*>::value, const char*, const char*&>::type;

    time_it(param_t filename, param_t funcname, int line)
        : filename_(filename)
        , funcname_(funcname)
        , line_(line)
    {
        begin_ = std::chrono::system_clock::now();
    }

    ~time_it()
    {
        xlogger::getInstance().log_(spdlog::level::info, filename_, funcname_, line_, "End executing {}, taking {}ms to execute", funcname_, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - begin_).count());
    }

private:
    time_it()                           = delete;
    time_it(const time_it&)             = delete;
    time_it& operator=(const time_it&)  = delete;
    time_it(const time_it&&)            = delete;
    time_it& operator=(const time_it&&) = delete;

private:
    param_t                               filename_;
    param_t                               funcname_;
    int                                   line_;
    std::chrono::system_clock::time_point begin_;
};
}
#define TIME_IT() utility::time_it ti(__FILENAME__, __FUNCTION__, __LINE__);

#endif  // LIUCAODANGBANG_TIME_IT_H
