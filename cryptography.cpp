#include "cryptography.h"

void ReadFile(const fs::path& path, std::vector<unsigned char>& buf)
{
	std::basic_fstream<unsigned char> file(path, std::ios::binary | std::fstream::in);
	if (!file.is_open())
	{
		throw std::runtime_error("Could not open file");
	}
	buf.clear();
	buf.insert(buf.begin(), std::istreambuf_iterator<unsigned char>(file), std::istreambuf_iterator<unsigned char>());
}

void WriteFile(const fs::path& path, std::vector<unsigned char>& buf)
{
	std::basic_ofstream<unsigned char> file(path, std::ios::binary);
	if (!file.is_open())
	{
		throw std::runtime_error("Could not open file");
	}
	file.write(&buf[0], buf.size());
}

void AppendToFile(const fs::path& path, std::vector<unsigned char>& buf)
{
	std::basic_ofstream<unsigned char> file(path, std::ios::binary | std::ios::app);
	if (!file.is_open())
	{
		throw std::runtime_error("Could not open file");
	}
	file.write(&buf[0], buf.size());
}

void SetPassword(std::string& password, std::vector<unsigned char>& key, std::vector<unsigned char>& iv)
{
	EVP_BytesToKey(EVP_aes_128_cbc(), EVP_md5(), nullptr, reinterpret_cast<unsigned char*>(&password[0]), password.size(), 1, &key[0], &iv[0]);
}

void CalculateHash(const std::vector<unsigned char>& data, std::vector<unsigned char>& hash)
{
	std::vector<unsigned char> temp(SHA256_DIGEST_LENGTH);
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, &data[0], data.size());
	SHA256_Final(&temp[0], &sha256);
	hash.swap(temp);
}

void CutHash(std::vector<unsigned char>& data, std::vector<unsigned char>& hash)
{
	std::vector<unsigned char> temp(SHA256_DIGEST_LENGTH);
	std::vector<unsigned char>::iterator it = data.begin() + ((data.size() - 1) - (SHA256_DIGEST_LENGTH - 1));
	temp.insert(temp.begin(), it, data.end());
	data.erase(it, data.end());
	hash.swap(temp);
}

bool DecryptAes(const std::vector<unsigned char>& encrypted, std::vector<unsigned char>& plain, const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv)
{
	EVP_CIPHER_CTX* context = EVP_CIPHER_CTX_new();
	if (!EVP_DecryptInit_ex(context, EVP_aes_128_cbc(), nullptr, &key[0], &iv[0]))
	{
		return 0;
	}
	std::vector<unsigned char> plainTextBuf(encrypted.size() + AES_BLOCK_SIZE);
	int plainTextSize = 0;
	if (!EVP_DecryptUpdate(context, &plainTextBuf[0], &plainTextSize, &encrypted[0], encrypted.size()))
	{
		EVP_CIPHER_CTX_free(context);
		return 0;
	}
	int lastPartLength = 0;
	if (!EVP_DecryptFinal_ex(context, &plainTextBuf[0] + plainTextSize, &lastPartLength))
	{
		EVP_CIPHER_CTX_free(context);
		return 0;
	}
	plainTextSize += lastPartLength;
	plainTextBuf.erase(plainTextBuf.begin() + plainTextSize, plainTextBuf.end());
	plain.swap(plainTextBuf);
	EVP_CIPHER_CTX_free(context);
	return 1;
}

bool EncryptAes(const std::vector<unsigned char>& plain, std::vector<unsigned char>& encrypted, const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv)
{
	EVP_CIPHER_CTX* context = EVP_CIPHER_CTX_new();
	if (!EVP_EncryptInit_ex(context, EVP_aes_128_cbc(), nullptr, &key[0], &iv[0]))
	{
		return 0;
	}
	std::vector<unsigned char> encryptedTextBuf(plain.size() + AES_BLOCK_SIZE);
	int encryptedTextSize = 0;
	if (!EVP_EncryptUpdate(context, &encryptedTextBuf[0], &encryptedTextSize, &plain[0], plain.size()))
	{
		EVP_CIPHER_CTX_free(context);
		return 0;
	}
	int lastPartLength = 0;
	if (!EVP_EncryptFinal_ex(context, &encryptedTextBuf[0] + encryptedTextSize, &lastPartLength))
	{
		EVP_CIPHER_CTX_free(context);
		return 0;
	}
	encryptedTextSize += lastPartLength;
	encryptedTextBuf.erase(encryptedTextBuf.begin() + encryptedTextSize, encryptedTextBuf.end());
	encrypted.swap(encryptedTextBuf);
	EVP_CIPHER_CTX_free(context);
	return 1;
}