#include "ProgressTracker.hpp"
#include <iostream>
#include <thread>

bruteforce::ProgressTracker::ProgressTracker(uint64_t endpoint) :
    m_endPoint(endpoint)
{}

void bruteforce::ProgressTracker::run() {
    m_timeStart = std::chrono::high_resolution_clock::now();

    while (!m_interrupted.test() || m_updateInfo)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - m_timeStart;

        m_current += m_updateInfo;
        m_updateInfo = 0;

        int percent = m_current * 100 / m_endPoint;
        std::cout << "[" << percent << "%] Checked passwords: "
            << m_current << " from " << m_endPoint << std::endl;
        std::cout << "Time elapsed: " << elapsed.count() << "s" << std::endl;
    }
    std::cout << m_interruptMessage << std::endl;
}

void bruteforce::ProgressTracker::update(uint64_t updateData) {
    m_updateInfo += updateData;
}

void bruteforce::ProgressTracker::interrupt(const std::string& message) {
    if (!m_interrupted.test())
    {
        m_interrupted.test_and_set();
        m_interruptMessage = message;
    }
}