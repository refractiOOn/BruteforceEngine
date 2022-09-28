#pragma once

#include "cryptography.h"
#include <iostream>
#include <string>
#include <mutex>
#include <map>
#include <thread>
#include <chrono>

class BruteforceEngine
{
public:
	BruteforceEngine(fs::path encrypted, fs::path plain);
	void SetConfig(fs::path path);
	void EnablePasswordLog(fs::path path);
	void Start();
private:
	size_t m_maxPasswordLength = 0;
	size_t m_currentPasswordLength = 0;
	int64_t m_checkedPasswordsAmount = 0;
	int64_t m_possiblePasswordsAmount = 0;
	bool m_passwordIsFound = false;
	bool m_lastIndexIsReached = false;

	std::vector<std::string> m_symbols;
	std::vector<size_t> m_index;

	std::vector<unsigned char> m_encryptedText;
	std::vector<unsigned char> m_plainText;
	std::vector<unsigned char> m_correctHash;
	std::string m_password;
	std::mutex m_mutex;
private:
	fs::path m_encryptedFile;
	fs::path m_plainFile;
	fs::path m_logFile;
	bool m_configIsUsed = false;
	bool m_passwordLogIsUsed = false;
private:
	void SetDefaultConfig();
	std::vector<std::string> GetBunchOfPasswords(size_t amount);
	bool IndexIncrement();
	void Tracker();
	void FindPassword();
	void Decrypt();
};