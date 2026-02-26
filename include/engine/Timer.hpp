#pragma once
#include <chrono>

namespace ee
{
    class Timer
    {
    private:
        float m_start = 0;
        float m_end = 0;

    public:
        Timer() {};

        float GetCurrentTime();

        float Start();

        float End();

        void Sleep(int _milliseconds);

        void ResetTimer();
    };

}