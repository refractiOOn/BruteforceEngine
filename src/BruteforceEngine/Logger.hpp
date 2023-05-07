#pragma once

#include <filesystem>
#include <vector>
#include <atomic>
#include <condition_variable>
#include <mutex>

namespace bruteforce
{

class Logger
{
public:
    Logger(const std::filesystem::path& file);
    ~Logger() = default;

    void run();
    void stop();

    void pushData(const std::vector<std::string>& data);

private:
    void write();

private:
    std::filesystem::path m_file{};

    std::vector<std::string> m_queuedBuffer{};
    std::vector<std::string> m_processedBuffer{};

    std::atomic_flag m_stopped = ATOMIC_FLAG_INIT;

    std::condition_variable m_queuedDataAvailable{};
    bool m_queuedDataAvailableFlag{};
    std::mutex m_queuedBufferMutex{};

};
    
} // namespace bruteforce