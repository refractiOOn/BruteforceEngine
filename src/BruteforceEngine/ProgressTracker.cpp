#include "ProgressTracker.hpp"
#include <iostream>
#include <thread>

bruteforce::ProgressTracker::ProgressTracker(uint64_t endpoint) :
    m_endPoint(endpoint)
{}

void bruteforce::ProgressTracker::run() {
    std::cout << "Tracker started" << std::endl;

    while (!m_interrupted.test())
    {
        if (m_current < m_endPoint)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - m_timeStart;

            uint64_t cur = m_current; // To avoid displaying an incorrect information
            //during the next steps because of the m_current modification

            int percent = cur * 100 / m_endPoint;
            std::cout << "[" << percent << "%] Checked passwords: "
                << cur << " from " << m_endPoint << std::endl;
            std::cout << "Time elapsed: " << elapsed.count() << "s" << std::endl;
        }
        else
        {
            interrupt("The end point has been reached");
        }
    }
}

void bruteforce::ProgressTracker::update(uint64_t updateData) {
    m_current += updateData;
}

void bruteforce::ProgressTracker::interrupt(const std::string& message) {
    m_interrupted.test_and_set();
    m_interruptMessage = message;
}