#pragma once

#include <filesystem>
#include <memory>

namespace bruteforce
{

struct ConfigurationContainer
{
    std::filesystem::path plainFile{};
    std::filesystem::path encryptedFile{};
    std::filesystem::path logFile{};
    std::string validSymbols{};
    int maxLength{};
};

class ConfigurationHandler
{
public:
    ConfigurationHandler(const std::filesystem::path& file);
    ~ConfigurationHandler() = default;

    void setFile(const std::filesystem::path& file) noexcept;
    std::filesystem::path file() const noexcept;

    std::shared_ptr<ConfigurationContainer> load();

private:
    std::filesystem::path m_file{};

};
    
} // namespace bruteforce
