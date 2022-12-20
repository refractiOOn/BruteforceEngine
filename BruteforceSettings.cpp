#include "BruteforceSettings.hpp"
#include "magic_enum/magic_enum.hpp"
#include <fstream>
#include <string>
#include <algorithm>

BruteforceSettings::BruteforceSettings(std::filesystem::path configFilePath) : 
	m_configFilePath(configFilePath)
{
}

void BruteforceSettings::Load()
{
	std::unordered_map<std::string, std::string> config;
	std::fstream file(m_configFilePath);
	std::string temp;
	while (std::getline(file, temp, ';'))
	{
		// CONFIG FORMAT
		// Key: value;
		// Key: value;
		temp.erase(std::remove(temp.begin(), temp.end(), ' '), temp.end());
		temp.erase(std::remove(temp.begin(), temp.end(), '\n'), temp.end());

		std::string key(temp.begin(), std::find(temp.begin(), temp.end(), ':'));
		std::string value(++std::find(temp.begin(), temp.end(), ':'), temp.end());

		config.emplace(key, value);
	}

	auto it = config.find("AdmissibleSymbols");
	for (char el : it->second)
	{
		m_admissibleSymbols.push_back(el);
	}
	m_maxPasswordLength = std::stoi(config.find("MaxPasswordLength")->second);

	SetAmountOfCPUCores();
}

void BruteforceSettings::SetFilePath(std::filesystem::path configFilePath)
{
	m_configFilePath = configFilePath;
}

void BruteforceSettings::SetAmountOfCPUCores()
{
	char *buf;
	size_t size;
	if (_dupenv_s(&buf, &size, "NUMBER_OF_PROCESSORS") == 0 && buf != nullptr)
	{
		m_amountOfCPUCores = atoi(buf);
	}
	else
	{
		m_amountOfCPUCores = 1;
	}
}

std::filesystem::path BruteforceSettings::GetFilePath() const
{
	return m_configFilePath;
}

int BruteforceSettings::GetAmountOfCPUCores() const
{
	return m_amountOfCPUCores;
}

int BruteforceSettings::GetMaxPasswordLength() const
{
	return m_maxPasswordLength;
}

const std::vector<char> &BruteforceSettings::GetAdmissibleSymbols() const
{
	return m_admissibleSymbols;
}