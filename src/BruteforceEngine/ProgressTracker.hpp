#pragma once

#include <string>
#include <atomic>
#include <chrono>

namespace bruteforce
{

class ProgressTracker
{
public:
    ProgressTracker(uint64_t endPoint);
    ~ProgressTracker() = default;

    void run();

    void update(uint64_t updateData);
    
    void interrupt(const std::string& message);

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_timeStart{};

    uint64_t m_endPoint{};
    std::atomic<uint64_t> m_current{};

    std::atomic_flag m_interrupted = ATOMIC_FLAG_INIT;
    std::string m_interruptMessage{};
};
    
} // namespace bruteforce
