/*
    License (OLC-3)
    ~~~~~~~~~~~~~~~
    Copyright 2018 - 2021 OneLoneCoder.com
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    1. Redistributions or derivations of source code must retain the above
    copyright notice, this list of conditions and the following disclaimer.
    2. Redistributions or derivative works in binary form must reproduce
    the above copyright notice. This list of conditions and the following
    disclaimer must be reproduced in the documentation and/or other
    materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its
    contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
    Links
    ~~~~~
    YouTube:	https://www.youtube.com/javidx9
    Discord:	https://discord.gg/WhwHUMV
    Twitter:	https://www.twitter.com/javidx9
    Twitch:		https://www.twitch.tv/javidx9
    GitHub:		https://www.github.com/onelonecoder
    Homepage:	https://www.onelonecoder.com
    Author
    ~~~~~~
    David Barr, aka javidx9, ©OneLoneCoder 2019, 2020, 2021
*/

#ifndef _TS_QUEUE_H
#define _TS_QUEUE_H

#include <deque>
#include <mutex>
#include <condition_variable>

template <class T>
class tsqueue
{
public:
    tsqueue() = default;

    const T& front()
    {
        std::lock_guard<std::mutex> lg(m);
        return dq.front();
    }

    const T& back()
    {
        std::lock_guard<std::mutex> lg(m);
        return dq.back();
    }

    void push_back(const T& value)
    {
        std::lock_guard<std::mutex> lg(m);
        dq.push_back(value);
    }

    void push_front(const T& value)
    {
        std::lock_guard<std::mutex> lg(m);
        dq.push_front(value);
    }

    T pop_front()
    {
        std::lock_guard<std::mutex> lg(m);
        T value = std::move(dq.front());
        dq.pop_front();
        cv.notify_one();
        return value;
    }

    T pop_back()
    {
        std::lock_guard<std::mutex> lg(m);
        T value = std::move(dq.back());
        dq.pop_back();
        cv.notify_one();
        return value;
    }

    bool empty()
    {
        std::lock_guard<std::mutex> lg(m);
        return dq.empty();
    }

    size_t size()
    {
        std::lock_guard<std::mutex> lg(m);
        return dq.size();
    }

    void clear()
    {
        std::lock_guard<std::mutex> lg(m);
        return dq.clear();
    }

    void wait()
    {
        std::unique_lock<std::mutex> lg(wait_block);
        cv.wait(lg, [this]{return !empty();});
    }

    void unwait(){}

    tsqueue(const tsqueue &) = delete;
    tsqueue(tsqueue &&) = delete;
    tsqueue &operator=(tsqueue &&) = delete;
    tsqueue &operator=(const tsqueue &) = delete;

private:
    std::deque<T> dq;
    mutable std::mutex m;
    mutable std::mutex wait_block;
    std::condition_variable cv;
};
#endif