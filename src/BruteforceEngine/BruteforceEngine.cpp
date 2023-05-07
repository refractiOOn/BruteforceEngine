#include "BruteforceEngine.hpp"
#include "ConfigurationHandler.hpp"
#include "FileHandler.hpp"
#include "ProgressTracker.hpp"
#include "Logger.hpp"
#include "cryptography.hpp"
#include <thread>
#include <iostream>
#include <functional>

bruteforce::BruteforceEngine::BruteforceEngine(std::shared_ptr<ConfigurationContainer> configuration) :
    m_configuration(configuration)
{
    m_logger = std::make_unique<Logger>(m_configuration->logFile);
}

void bruteforce::BruteforceEngine::run()
{
    uint64_t possiblePasswords = 0;
    // Calculate an amount of possible passwords
    for (int i = m_configuration->maxLength; i > 0; --i)
    {
        possiblePasswords += std::pow(m_configuration->validSymbols.size(), i);
    }
    m_tracker = std::make_unique<ProgressTracker>(possiblePasswords);
    // possibilities calculated

    // Read encrypted file data
    FileHandler fileHandler(m_configuration->encryptedFile);
    m_dataContainer.encryptedData = fileHandler.read();
    m_dataContainer.correctHash = cutHash(m_dataContainer.encryptedData);
    // Files read

    std::jthread logger(&Logger::run, m_logger.get());
    std::jthread tracker(&ProgressTracker::run, m_tracker.get());

    std::vector<std::jthread> threads;
    // Divide the amount of passwords evenly between the threads (rounding up)
    // std::ceil to round up
    int passwordsPerThread = std::ceil(static_cast<double>(possiblePasswords) / std::jthread::hardware_concurrency());
    uint64_t firstIndex = 0, secondIndex = 0;

    for (int i = 0; i < std::jthread::hardware_concurrency(); ++i)
    {
        firstIndex = secondIndex;
        secondIndex += passwordsPerThread;
        // possiblePasswords value is the max index
        if (secondIndex > possiblePasswords)
            secondIndex = possiblePasswords;

        threads.push_back(std::jthread(&BruteforceEngine::findPassword, this, firstIndex, secondIndex));
    }

    // Wait for the threads to finish their work
    std::for_each(threads.begin(), threads.end(), [](auto& el){ el.join(); });

    m_logger->stop();
    if (!m_dataContainer.correctPassword.empty())
    {
        m_tracker->interrupt("Correct password: " + m_dataContainer.correctPassword);
        // decrypt
        decrypt();
        fileHandler.setFile(m_configuration->plainFile);
        fileHandler.write(m_dataContainer.plainData);
    }
    else
    {
        m_tracker->interrupt("Correct password was not found");
    }
}

std::string bruteforce::BruteforceEngine::generatePassword(uint64_t index) const
{
    // Compute the password length
    int length{};
    uint64_t counter{};
    /* If index is 0, then length is automatically 1 because of
    the overhead caused by checks at each function call
    (since the type used for indexes is 'unsigned', which cannot be less than 0)
    */
    if (index)
    {
        do
        {
            ++length;
            counter += std::pow(m_configuration->validSymbols.size(), length);
            if (length == 1) --counter;
        } while (counter < index);
    }
    else length = 1;
    
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

std::vector<std::string> bruteforce::BruteforceEngine::generatePasswordPack(uint64_t begin, uint64_t end)
{
    std::vector<std::string> pack{};
    for (uint64_t i = begin; i < end; ++i)
    {
        std::string password = generatePassword(i);
        pack.push_back(password);
    }
    return pack;
}

bool bruteforce::BruteforceEngine::checkPassword(const std::string& password)
{
    std::pair<std::vector<unsigned char>, std::vector<unsigned char>> keyIv = passwordToKey(password);
    std::vector<unsigned char> plain{};
    if (decryptAes(m_dataContainer.encryptedData, plain, keyIv.first, keyIv.second))
    {
        std::vector<unsigned char> hash = calculateHash(plain);
        if (hash == m_dataContainer.correctHash)
        {
            return true;
        }
    }
    return false;
}

void bruteforce::BruteforceEngine::findPassword(uint64_t begin, uint64_t end)
{
    std::vector<std::string> pack = generatePasswordPack(begin, end), checked{};

    // Notify tracker and logger about progress, then clear it
    std::function<void()> approveProgress = [&](){
        m_tracker->update(checked.size());
        m_logger->pushData(checked);
        checked.clear();
    };

    uint64_t percentIterationCounter = (end - begin) / 10;
    for (auto& el : pack)
    {
        if (m_passwordFound.test()) break;

        bool res = checkPassword(el);
        checked.push_back(el);

        if (checked.size() == percentIterationCounter)
            approveProgress();

        if (res)
        {
            m_dataContainer.correctPassword = el;
            if (!m_passwordFound.test())
                m_passwordFound.test_and_set();
        }
    }
    if (checked.size())
        approveProgress();
}

void bruteforce::BruteforceEngine::decrypt()
{
    std::pair<std::vector<unsigned char>, std::vector<unsigned char>> keyIv =
        passwordToKey(m_dataContainer.correctPassword);
    decryptAes(m_dataContainer.encryptedData, m_dataContainer.plainData, keyIv.first, keyIv.second);
}