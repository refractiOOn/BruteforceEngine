#pragma once

#include <string>
#include <vector>
#include <openssl/evp.h>
#include <openssl/sha.h>

namespace bruteforce
{

class ContextWrapper
{
public:
    ContextWrapper();
    ~ContextWrapper();

    EVP_CIPHER_CTX* get();

private:
    EVP_CIPHER_CTX* m_ctx{};

};

std::vector<unsigned char> cutHash(std::vector<unsigned char>& data);

std::pair<std::vector<unsigned char>, std::vector<unsigned char>> passwordToKey(const std::string& password);

std::vector<unsigned char> calculateHash(const std::vector<unsigned char>& data);

bool encryptAes(const std::vector<unsigned char>& plain, std::vector<unsigned char>& encrypted,
    const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv);

bool decryptAes(const std::vector<unsigned char>& encrypted, std::vector<unsigned char>& plain,
    const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv);
    
} // namespace bruteforce