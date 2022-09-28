#include "BruteforceEngine.h"

size_t GetPCCoresAmount()
{
	char* buf;
	size_t size;
	if (_dupenv_s(&buf, &size, "NUMBER_OF_PROCESSORS") == 0 && buf != nullptr)
	{
		return atoi(buf);
	}
	else
	{
		return 1;
	}
}

BruteforceEngine::BruteforceEngine(fs::path encrypted, fs::path plain) :
	m_encryptedFile(encrypted),
	m_plainFile(plain)
{
}

void BruteforceEngine::SetConfig(fs::path path)
{
	m_configIsUsed = true;
	std::map<std::string, std::string> config;
	std::fstream file(path);
	std::string temp;
	while (std::getline(file, temp, ';'))
	{
		temp.erase(std::remove(temp.begin(), temp.end(), ' '), temp.end());
		temp.erase(std::remove(temp.begin(), temp.end(), '\n'), temp.end());
		std::string key(temp.begin(), std::find(temp.begin(), temp.end(), ':'));
		std::string value(++std::find(temp.begin(), temp.end(), ':'), temp.end());
		config.emplace(key, value);
	}
	auto it = config.find("symbols");
	for (size_t i = 0; i < it->second.length(); ++i)
	{
		std::string temp;
		temp += it->second[i];
		m_symbols.push_back(temp);
	}
	m_maxPasswordLength = std::stoi(config.find("length")->second);
}

void BruteforceEngine::EnablePasswordLog(fs::path path)
{
	m_passwordLogIsUsed = true;
	m_logFile = path;
}

void BruteforceEngine::Start()
{
	if (!m_configIsUsed)
	{
		SetDefaultConfig();
	}
	if (m_passwordLogIsUsed)
	{
		std::ofstream log(m_logFile, std::ios::trunc);
		if (!log.is_open())
		{
			std::cout << "Could not open file" << std::endl;
			return;
		}
	}
	for (size_t i = m_maxPasswordLength; i > 0; --i)
	{
		m_possiblePasswordsAmount += pow(m_symbols.size(), i);
	}
	const size_t threadsAmount = GetPCCoresAmount() * 2;
	OpenSSL_add_all_algorithms();

	ReadFile(m_encryptedFile, m_encryptedText);
	CutHash(m_encryptedText, m_correctHash);
	m_correctHash.resize(SHA256_DIGEST_LENGTH);

	std::thread tracker(&BruteforceEngine::Tracker, this);
	std::vector<std::thread> threads;
	for (size_t i = 0; i < threadsAmount; ++i)
	{
		threads.push_back(std::thread(&BruteforceEngine::FindPassword, this));
	}
	for (size_t i = 0; i < threads.size(); ++i)
	{
		threads[i].join();
	}
	tracker.join();
	if (!m_password.empty())
	{
		std::cout << "Password: " << m_password << std::endl;
		Decrypt();
	}
	else
	{
		std::cout << "Password not found" << std::endl;
	}
}

void BruteforceEngine::SetDefaultConfig()
{
	m_maxPasswordLength = 1;
	m_symbols = { "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
}

std::vector<std::string> BruteforceEngine::GetBunchOfPasswords(size_t amount)
{
	std::vector<std::string> bunch;
	for (size_t i = 0; i < amount; ++i)
	{
		if (!m_lastIndexIsReached && !m_passwordIsFound)
		{
			std::string pass;
			std::lock_guard lg(m_mutex);
			for (size_t j = 0; j < m_currentPasswordLength; ++j)
			{
				pass += m_symbols[m_index[j]];
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
	if (m_index.size() == m_maxPasswordLength)
	{
		int last = m_symbols.size() - 1;
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
		if (m_index[i] < m_symbols.size() - 1)
		{
			++m_index[i];
			return 1;
		}
		m_index[i] = 0;
	}
	if (m_currentPasswordLength < m_maxPasswordLength)
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
	while (!m_lastIndexIsReached && !m_passwordIsFound)
	{
		std::cout << m_checkedPasswordsAmount << " from " << m_possiblePasswordsAmount << " passwords checked [" << static_cast<double>(m_checkedPasswordsAmount) / static_cast<double>(m_possiblePasswordsAmount) * 100 << "%]" << std::endl;
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
	while (!m_lastIndexIsReached && !m_passwordIsFound)
	{
		std::vector<std::string> passwords = GetBunchOfPasswords(100000);
		if (!passwords.empty())
		{
			for (uint32_t i = 0; i < passwords.size(); ++i)
			{
				if (!m_lastIndexIsReached && !m_passwordIsFound)
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
				else
				{
					break;
				}
			}
		}
		else
		{
			break;
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