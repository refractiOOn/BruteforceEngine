#pragma once

#include <filesystem>
#include <fstream>
#include <vector>
#include <string_view>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/sha.h>

namespace fs = std::filesystem;

void ReadFile(const fs::path& path, std::vector<unsigned char>& buf);
void WriteFile(const fs::path& path, std::vector<unsigned char>& buf);
void AppendToFile(const fs::path& path, std::vector<unsigned char>& buf);
void SetPassword(std::string& password, std::vector<unsigned char>& key, std::vector<unsigned char>& iv);
void CalculateHash(const std::vector<unsigned char>& data, std::vector<unsigned char>& hash);
void CutHash(std::vector<unsigned char>& data, std::vector<unsigned char>& hash);
bool DecryptAes(const std::vector<unsigned char>& encrypted, std::vector<unsigned char>& plain, const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv);
bool EncryptAes(const std::vector<unsigned char>& plain, std::vector<unsigned char>& encrypted, const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv);