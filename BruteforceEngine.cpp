#include "BruteforceEngine.h"
#include <iostream>
#include <vector>
#include <map>
#include <filesystem>
#include <fstream>
#include <thread>
#include <mutex>
#include <chrono>
#include <ranges>

BruteforceEngine::BruteforceEngine(std::filesystem::path encrypted, std::filesystem::path plain) :
	m_encryptedFile(encrypted),
	m_plainFile(plain)
{

}

void BruteforceEngine::AddConfig(BruteforceSettings *config)
{
	m_config = config;
}

void BruteforceEngine::EnablePasswordLog(std::filesystem::path path)
{
	m_passwordLogIsUsed = true;
	m_logFile = path;
}

void BruteforceEngine::Start()
{
	if (!m_config)
	{
		throw std::runtime_error("Config was not included");
	}
	if (m_passwordLogIsUsed)
	{
		std::ofstream log(m_logFile, std::ios::trunc);
		if (!log.is_open())
		{
			std::cout << "Could not open file" << std::endl;
		}
	}

	for (size_t i = m_config->GetMaxPasswordLength(); i > 0; --i)
	{
		m_possiblePasswordsAmount += pow(m_config->GetAdmissibleSymbols().size(), i);
	}
	OpenSSL_add_all_algorithms();

	ReadFile(m_encryptedFile, m_encryptedText);
	CutHash(m_encryptedText, m_correctHash);
	m_correctHash.resize(SHA256_DIGEST_LENGTH);

	std::jthread tracker(&BruteforceEngine::Tracker, this);
	tracker.detach();

	std::vector<std::jthread> threads(m_config->GetAmountOfCPUCores());
	std::ranges::for_each(threads, [&](auto &el) {el = std::move(std::jthread(&BruteforceEngine::FindPassword, this)); });

	std::ranges::for_each(threads, [&](auto &el) {el.join(); });

	if (!m_password.empty())
	{
		Decrypt();
		std::cout << "Password: " << m_password << std::endl;
	}
	else
	{
		std::cout << "Password not found" << std::endl;
	}
}

std::vector<std::string> BruteforceEngine::GetBunchOfPasswords(size_t amount)
{
	std::vector<std::string> bunch;
	for (size_t i = 0; i < amount; ++i)
	{
		if (!m_lastIndexIsReached)
		{
			std::lock_guard lg(m_mutex);
			std::string pass;
			for (size_t j = 0; j < m_currentPasswordLength; ++j)
			{
				pass += m_config->GetAdmissibleSymbols()[m_index[j]];
			}
			bunch.emplace_back(pass);
			if (!IndexIncrement())
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
	return bunch;
}

bool BruteforceEngine::IndexIncrement()
{
	if (m_index.size() == m_config->GetMaxPasswordLength())
	{
		int last = m_config->GetAdmissibleSymbols().size() - 1;
		auto it = std::find_if(m_index.begin(), m_index.end(), [last](int index)
			{
				return index != last;
			});
		if (it == m_index.end())
		{
			m_lastIndexIsReached = true;
			return 0;
		}
	}
	for (int i = m_index.size() - 1; i >= 0; --i)
	{
		if (m_index[i] < m_config->GetAdmissibleSymbols().size() - 1)
		{
			++m_index[i];
			return 1;
		}
		m_index[i] = 0;
	}
	if (m_currentPasswordLength < m_config->GetMaxPasswordLength())
	{
		++m_currentPasswordLength;
		m_index.push_back(0);
		return 1;
	}
	return 0;
}

void BruteforceEngine::Tracker()
{
	std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	double elapsedSeconds = 0;
	while (!m_passwordIsFound)
	{
		std::cout << m_checkedPasswordsAmount << " from " << m_possiblePasswordsAmount << " passwords checked [" << m_checkedPasswordsAmount * 100 / m_possiblePasswordsAmount << "%]" << std::endl;
		std::chrono::system_clock::time_point current = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed = current - start;
		elapsedSeconds = elapsed.count();
		std::cout << "Time elapsed: " << elapsedSeconds << "s" << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void BruteforceEngine::FindPassword()
{
	std::fstream log;
	if (m_passwordLogIsUsed)
	{
		log.open(m_logFile, std::ios::app);
		if (!log.is_open())
		{
			std::cout << "Could not open file" << std::endl;
			return;
		}
	}
	while (!m_passwordIsFound)
	{
		std::vector<std::string> passwords = GetBunchOfPasswords(100000);
		if (passwords.empty())
		{
			break;
		}

		for (uint32_t i = 0; i < passwords.size() && !m_passwordIsFound; ++i)
		{
			m_mutex.lock();
			++m_checkedPasswordsAmount;
			if (m_passwordLogIsUsed)
			{
				log.write(&passwords[i][0], passwords[i].length());
				log << '\n';
			}
			m_mutex.unlock();
			std::vector<unsigned char> key(EVP_MAX_KEY_LENGTH);
			std::vector<unsigned char> iv(EVP_MAX_IV_LENGTH);
			std::vector<unsigned char> plainText;
			SetPassword(passwords[i], key, iv);
			if (DecryptAes(m_encryptedText, plainText, key, iv))
			{
				std::vector<unsigned char> hash(SHA256_DIGEST_LENGTH);
				CalculateHash(plainText, hash);
				if (m_correctHash == hash)
				{
					m_password = passwords[i];
					m_passwordIsFound = true;
					break;
				}
			}
		}
	}
}

void BruteforceEngine::Decrypt()
{
	std::vector<unsigned char> key(EVP_MAX_KEY_LENGTH);
	std::vector<unsigned char> iv(EVP_MAX_IV_LENGTH);
	SetPassword(m_password, key, iv);
	DecryptAes(m_encryptedText, m_plainText, key, iv);
	WriteFile(m_plainFile, m_plainText);
}