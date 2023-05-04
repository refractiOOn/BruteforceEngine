#include "BruteforceEngine.hpp"
#include "ConfigurationHandler.hpp"

bruteforce::BruteforceEngine::BruteforceEngine(std::shared_ptr<ConfigurationContainer> configuration) :
    m_configuration(configuration)
{
}

void bruteforce::BruteforceEngine::run()
{
}

std::string bruteforce::BruteforceEngine::generatePassword(uint64_t index) const
{
    // Compute the password length
    int length = 0;
    uint64_t counter = index;
    do
    {
        counter /= m_configuration->validSymbols.size();
        ++length;
    } while (counter > 0);
    
    // Generate the password
    std::string password{};
    counter = index;
    for (int i = 1; i < length; ++i)
    {
        counter -= std::powl(m_configuration->validSymbols.size(), i);
    }
    for (int i = 0; i < length; ++i)
    {
        uint64_t ch = counter % m_configuration->validSymbols.size();
        password = m_configuration->validSymbols[ch] + password;
        counter /= m_configuration->validSymbols.size();
    }

    return password;
}

void bruteforce::BruteforceEngine::findPassword(uint64_t begin, uint64_t end)
{
}