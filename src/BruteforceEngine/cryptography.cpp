#include "cryptography.hpp"
#include <openssl/aes.h>

bruteforce::ContextWrapper::ContextWrapper() : m_ctx(EVP_CIPHER_CTX_new())
{}

bruteforce::ContextWrapper::~ContextWrapper()
{
    EVP_CIPHER_CTX_free(m_ctx);
}

EVP_CIPHER_CTX *bruteforce::ContextWrapper::get()
{
    return m_ctx;
}

std::vector<unsigned char> bruteforce::cutHash(std::vector<unsigned char>& data)
{
    std::vector<unsigned char>::iterator it = data.begin() + data.size() - SHA256_DIGEST_LENGTH;
    std::vector<unsigned char> hash(it, data.end());
    data.erase(it, data.end());
    return hash;
}

std::pair<std::vector<unsigned char>, std::vector<unsigned char>> bruteforce::passwordToKey(const std::string& password)
{
    std::string pass = password; // reinterpret_cast cannot be used with const data
    std::pair<std::vector<unsigned char>, std::vector<unsigned char>> keyIvPair{
        std::vector<unsigned char>(EVP_MAX_KEY_LENGTH), std::vector<unsigned char>(EVP_MAX_IV_LENGTH)
    }; // Key is the first, IV is the second
    EVP_BytesToKey(EVP_aes_128_cbc(), EVP_md5(), nullptr,
        reinterpret_cast<unsigned char*>(&pass[0]), pass.size(), 1, &keyIvPair.first[0], &keyIvPair.second[0]);
    return keyIvPair;
}

std::vector<unsigned char> bruteforce::calculateHash(const std::vector<unsigned char>& data)
{
    std::vector<unsigned char> hash(SHA256_DIGEST_LENGTH);
    SHA256_CTX context;
    SHA256_Init(&context);
    SHA256_Update(&context, &data[0], data.size());
    SHA256_Final(&hash[0], &context);
    return hash;
}

bool bruteforce::encryptAes(const std::vector<unsigned char>& plain, std::vector<unsigned char>& encrypted,
    const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv)
{
    ContextWrapper context{};
    if (!EVP_EncryptInit_ex(context.get(), EVP_aes_128_cbc(), nullptr, &key[0], &iv[0]))
        return 0;
    std::vector<unsigned char> encryptedBuf(plain.size() + AES_BLOCK_SIZE);
    int encryptedSize = 0;
    if (!EVP_EncryptUpdate(context.get(), &encryptedBuf[0], &encryptedSize, &plain[0], plain.size()))
        return 0;

    int lastPartLen = 0;
    if (!EVP_EncryptFinal_ex(context.get(), &encryptedBuf[0] + encryptedSize, &lastPartLen))
        return 0;

    encryptedSize += lastPartLen;
    encryptedBuf.erase(encryptedBuf.begin() + encryptedSize, encryptedBuf.end());
    encrypted.swap(encryptedBuf);
    return 1;
}

bool bruteforce::decryptAes(const std::vector<unsigned char>& encrypted, std::vector<unsigned char>& plain,
    const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv)
{
    ContextWrapper context{};
    if (!EVP_DecryptInit_ex(context.get(), EVP_aes_128_cbc(), nullptr, &key[0], &iv[0]))
        return 0;

    std::vector<unsigned char> plainBuf(encrypted.size() + AES_BLOCK_SIZE);
    int plainSize = 0;
    if (!EVP_DecryptUpdate(context.get(), &plainBuf[0], &plainSize, &encrypted[0], encrypted.size()))
        return 0;

    int lastPartLen = 0;
    if (!EVP_DecryptFinal_ex(context.get(), &plainBuf[0] + plainSize, &lastPartLen))
        return 0;

    plainSize += lastPartLen;
    plainBuf.erase(plainBuf.begin() + plainSize, plainBuf.end());
    plain.swap(plainBuf);
    return 1;
}