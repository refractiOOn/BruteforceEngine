#include "ConfigurationHandler.hpp"
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <wil/resource.h>

bruteforce::ConfigurationHandler::ConfigurationHandler(const std::filesystem::path& file) : m_file(file)
{}

std::unique_ptr<bruteforce::ConfigurationContainer> bruteforce::ConfigurationHandler::load()
{
    // Open the file
    wil::unique_file file(fopen(m_file.string().c_str(), "r"));

    // Read the file into a buffer
    constexpr int bufSize = 65536; // 64 KB
    std::string buf(bufSize, '\0');
    rapidjson::FileReadStream fs(file.get(), &buf[0], buf.length());

    // Parse the JSON document
    rapidjson::Document doc{};
    if (doc.ParseStream(fs).HasParseError())
    {
        throw std::runtime_error("JSON parse error");
    }

    // Check for required names in configuration
    if (!doc.HasMember("plainFile") or !doc.HasMember("encryptedFile")
        or !doc.HasMember("maxLength") or !doc.HasMember("validSymbols"))
    {
        throw std::logic_error("Some of the required JSON names are missing");
    }

    // Create a container for the configuration data
    std::unique_ptr<ConfigurationContainer> res = std::make_unique<ConfigurationContainer>();
    res->plainFile = doc["plainFile"].GetString();
    res->encryptedFile = doc["encryptedFile"].GetString();
    res->maxLength = doc["maxLength"].GetInt();
    res->validSymbols = doc["validSymbols"].GetString();

    return res;
}