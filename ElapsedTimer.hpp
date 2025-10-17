// Copyright (c) 2023 Shandong Matrix Software Engineering Co., Ltd All rights reserved.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

/*
 * @Description:
 * @Author: lucky
 * @Date: 2023/8/1 8:45
 * @LastEditors: lucky
 * @LastEditTime: 2023/8/1 8:45
 */

#ifndef ELAPSEDTIMER_H
#define ELAPSEDTIMER_H

#include <chrono>
namespace utility
{
class ElapsedTimer
{
public:
    ElapsedTimer()
        : t1(INT64_C(0x8000000000000000))
    {
        start();
    }

    void start()
    {
        restart();
    }

    uint64_t restart()
    {
        t1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        return t1;
    }

    int64_t elapsed() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - t1;
    }

    bool hasExpired(int64_t timeout) const noexcept
    {
        // if timeout is -1, quint64(timeout) is LLINT_MAX, so this will be
        // considered as never expired
        return elapsed() > timeout;
    }

private:
    int64_t t1;
};
}

#endif  // ELAPSEDTIMER_H
