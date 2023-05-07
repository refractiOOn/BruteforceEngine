#pragma once

#include "ProgressTracker.hpp"
#include "Logger.hpp"

#include <string>
#include <vector>
#include <memory>
#include <atomic>

namespace bruteforce
{

struct ConfigurationContainer;

struct BruteforceDataContainer
{
    std::vector<unsigned char> encryptedData{};
    std::vector<unsigned char> plainData{};
    std::vector<unsigned char> correctHash{};
    std::string correctPassword{};
};

class BruteforceEngine
{
public:
    BruteforceEngine(std::shared_ptr<ConfigurationContainer> configuration);
    ~BruteforceEngine() = default;

    void run();

private:
    std::string generatePassword(uint64_t index) const;
    std::vector<std::string> generatePasswordPack(uint64_t begin, uint64_t end);
    bool checkPassword(const std::string& password);
    void findPassword(uint64_t begin, uint64_t end);

    void decrypt();

private:
    std::shared_ptr<ConfigurationContainer> m_configuration{};
    std::unique_ptr<ProgressTracker> m_tracker{};
    std::unique_ptr<Logger> m_logger{};

    std::atomic_flag m_passwordFound = ATOMIC_FLAG_INIT;

    BruteforceDataContainer m_dataContainer{};

};
    
} // namespace bruteforce