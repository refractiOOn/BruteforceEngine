#pragma once

#include <memory>

namespace bruteforce
{

struct ConfigurationContainer;

class BruteforceEngine
{
public:
    BruteforceEngine(std::unique_ptr<ConfigurationContainer> configuration);

private:

private:
    std::unique_ptr<ConfigurationContainer> m_configuration;

};
    
} // namespace bruteforce