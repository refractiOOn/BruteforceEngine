#include "ConfigurationHandler.hpp"
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/error/en.h>
#include <wil/resource.h>

bruteforce::ConfigurationHandler::ConfigurationHandler(const std::filesystem::path& file) : m_file(file)
{}

void bruteforce::ConfigurationHandler::setFile(const std::filesystem::path& file) noexcept
{
    m_file = file;
}

std::filesystem::path bruteforce::ConfigurationHandler::file() const noexcept
{
    return m_file;
}

std::shared_ptr<bruteforce::ConfigurationContainer> bruteforce::ConfigurationHandler::load()
{
    // Open the file
    wil::unique_file file(fopen(m_file.string().c_str(), "r"));
    if (!file.get())
    {
        throw std::runtime_error("File does not exist");
    }

    // Read the file into a buffer
    constexpr int bufSize = 65536; // 64 KB
    std::string buf(bufSize, '\0');
    rapidjson::FileReadStream fs(file.get(), &buf[0], buf.length());

    // Parse the JSON document
    rapidjson::Document doc{};
    rapidjson::ParseResult res = doc.Parse(buf.c_str());
    if (!res)
    {
        throw std::runtime_error("JSON parse error: " + std::string(rapidjson::GetParseError_En(res.Code())));
    }

    // Check for required names in configuration
    if (!doc.HasMember("plainFile") or !doc.HasMember("encryptedFile") or !doc.HasMember("logFile")
        or !doc.HasMember("maxLength") or !doc.HasMember("validSymbols"))
    {
        throw std::logic_error("Some of the required JSON names are missing");
    }

    // Create a container for the configuration data
    std::shared_ptr<ConfigurationContainer> container = std::make_shared<ConfigurationContainer>();
    container->plainFile = doc["plainFile"].GetString();
    container->encryptedFile = doc["encryptedFile"].GetString();
    container->logFile = doc["logFile"].GetString();
    container->maxLength = doc["maxLength"].GetInt();
    container->validSymbols = doc["validSymbols"].GetString();

    return container;
}