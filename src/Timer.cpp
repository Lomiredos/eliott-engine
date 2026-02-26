#include "engine/Timer.hpp"
#include <thread>

float ee::Timer::GetCurrentTime()
{
    auto now = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());

    return (float)(ms.count());
}

float ee::Timer::Start()
{
    m_start = GetCurrentTime();

    return m_start;
}

float ee::Timer::End()
{
    m_end = GetCurrentTime();

    return m_end;
}

void ee::Timer::Sleep(int _milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(_milliseconds));
}

void ee::Timer::ResetTimer()
{
    m_start = 0;
    m_end = 0;
}