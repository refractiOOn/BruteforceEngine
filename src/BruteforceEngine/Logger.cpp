#include "Logger.hpp"

#include <fstream>

bruteforce::Logger::Logger(const std::filesystem::path& file) : m_file(file)
{}

void bruteforce::Logger::run()
{
    // Clean file if it exists. If doesn't - create.
    std::ofstream fs(m_file);
    fs.close();

    std::unique_lock queuedBufferLock(m_queuedBufferMutex, std::defer_lock);
    while (!m_stopped.test() || !m_queuedBuffer.empty())
    {
        // Wait for available data in m_queuedBuffer
        queuedBufferLock.lock();
        m_queuedDataAvailable.wait(queuedBufferLock, [&] { return m_queuedDataAvailableFlag; });

        // Move data from m_queuedBuffer to m_processesBuffer
        m_queuedBuffer.swap(m_processedBuffer);
        m_queuedDataAvailableFlag = false;
        queuedBufferLock.unlock();

        // Write into file
        write();
        m_processedBuffer.clear();
    }
}

void bruteforce::Logger::stop()
{
    if (!m_stopped.test())
        m_stopped.test_and_set();
}

void bruteforce::Logger::pushData(const std::vector<std::string>& data)
{
    {
        std::lock_guard lock(m_queuedBufferMutex);
        m_queuedBuffer.insert(m_queuedBuffer.end(), data.begin(), data.end());
        m_queuedDataAvailableFlag = true;
    }
    m_queuedDataAvailable.notify_one();
}

void bruteforce::Logger::write()
{
    // Open the file
    std::ofstream fs(m_file, std::ios::app);
    if (!fs.is_open())
    {
        throw std::runtime_error("Log file was not opened");
    }

    // Write
    for (auto& el : m_processedBuffer)
    {
        fs.write(el.c_str(), el.length());
        fs << '\n';
    }
    fs.close();
}