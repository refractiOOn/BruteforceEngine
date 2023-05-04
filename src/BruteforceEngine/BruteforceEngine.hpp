#pragma once

#include <memory>
#include <string>

namespace bruteforce
{

struct ConfigurationContainer;

class BruteforceEngine
{
public:
    BruteforceEngine(std::shared_ptr<ConfigurationContainer> configuration);

    void run();

private:
    std::string generatePassword(uint64_t index) const;
    void findPassword(uint64_t begin, uint64_t end);

private:
    std::shared_ptr<ConfigurationContainer> m_configuration{};

};
    
} // namespace bruteforce