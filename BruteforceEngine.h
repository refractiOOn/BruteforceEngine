#pragma once

#include "cryptography.h"
#include "BruteforceSettings.hpp"
#include <filesystem>
#include <vector>
#include <mutex>
#include <map>
#include <memory>

class BruteforceEngine
{
public:
	BruteforceEngine(std::filesystem::path encrypted, std::filesystem::path plain);

	void AddConfig(BruteforceSettings *config);
	void EnablePasswordLog(std::filesystem::path path);
	void Start();
private:
	BruteforceSettings *m_config = nullptr;

	int m_currentPasswordLength = 0;
	int64_t m_checkedPasswordsAmount = 0;
	int64_t m_possiblePasswordsAmount = 0;
	bool m_passwordIsFound = false;
	bool m_lastIndexIsReached = false;

	std::vector<size_t> m_index;

	std::vector<unsigned char> m_encryptedText;
	std::vector<unsigned char> m_plainText;
	std::vector<unsigned char> m_correctHash;
	std::string m_password;
	std::mutex m_mutex;
private:
	std::filesystem::path m_encryptedFile;
	std::filesystem::path m_plainFile;
	std::filesystem::path m_logFile;
	bool m_passwordLogIsUsed = false;
private:
	std::vector<std::string> GetBunchOfPasswords(size_t amount);
	bool IndexIncrement();
	void Tracker();
	void FindPassword();
	void Decrypt();
};