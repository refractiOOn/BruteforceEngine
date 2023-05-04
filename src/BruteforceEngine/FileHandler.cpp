#include "FileHandler.hpp"
#include <fstream>

bruteforce::FileHandler::FileHandler(const std::filesystem::path& file) : m_file(file)
{}

void bruteforce::FileHandler::setFile(const std::filesystem::path& file)
{
    m_file = file;
}

std::filesystem::path bruteforce::FileHandler::file() const
{
    return m_file;
}

std::vector<unsigned char> bruteforce::FileHandler::read() const
{
    std::basic_ifstream<unsigned char> file(m_file, std::ios::binary);
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open file for reading");
    }
    return std::vector<unsigned char>((std::istreambuf_iterator<unsigned char>(file)),
        std::istreambuf_iterator<unsigned char>());
}

void bruteforce::FileHandler::write(const std::vector<unsigned char>& data) const
{
    std::basic_ofstream<unsigned char> file(m_file, std::ios::binary);
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open or create file for writing");
    }
    file.write(&data[0], data.size());
}

void bruteforce::FileHandler::write(const std::vector<unsigned char>& data) const
{
    std::basic_ofstream<unsigned char> file(m_file, std::ios::binary | std::ios::app);
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open or create file for appending");
    }
    file.write(&data[0], data.size());
}