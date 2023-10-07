#pragma once

#include <chrono>

using namespace std::chrono;

class Timer
{
public:
    Timer(bool start = false)
    {
        if (start)
            m_start = system_clock::now();
        else
            m_start = system_clock::time_point::min();
    }
    void Clear() { m_start = system_clock::time_point::min(); }
    bool IsStarted() { return (m_start.time_since_epoch() != system_clock::duration(0)); }
    void Start() { m_start = system_clock::now(); }
    long long GetMS() 
    {
        if (IsStarted())
        {
            system_clock::duration diff = system_clock::now() - m_start;
            return duration_cast<milliseconds>(diff).count();
        }

        return 0;
    }
private:
    system_clock::time_point m_start;
};

