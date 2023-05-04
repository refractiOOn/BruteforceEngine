#pragma once

#include <filesystem>

namespace bruteforce
{

class FileHandler
{
public:
    FileHandler(const std::filesystem::path& file);
    ~FileHandler() = default;

    void setFile(const std::filesystem::path& file) noexcept;
    std::filesystem::path file() const noexcept;

    std::vector<unsigned char> read() const;
    void write(const std::vector<unsigned char>& data) const;
    void append(const std::vector<unsigned char>& data) const;

private:
    std::filesystem::path m_file{};

};
    
} // namespace bruteforce